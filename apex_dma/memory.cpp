#include "memory.h"
#include <unistd.h>
#include <vector>

// Credits: learn_more, stevemk14ebr
size_t findPattern(const PBYTE rangeStart, size_t len, const char *pattern)
{
	size_t l = strlen(pattern);
	PBYTE patt_base = static_cast<PBYTE>(malloc(l >> 1));
	PBYTE msk_base = static_cast<PBYTE>(malloc(l >> 1));
	PBYTE pat = patt_base;
	PBYTE msk = msk_base;
	if (pat && msk)
	{
		l = 0;
		while (*pattern)
		{
			if (*pattern == ' ')
				pattern++;
			if (!*pattern)
				break;
			if (*(PBYTE)pattern == (BYTE)'\?')
			{
				*pat++ = 0;
				*msk++ = '?';
				pattern += ((*(PWORD)pattern == (WORD)'\?\?') ? 2 : 1);
			}
			else
			{
				*pat++ = getByte(pattern);
				*msk++ = 'x';
				pattern += 2;
			}
			l++;
		}
		*msk = 0;
		pat = patt_base;
		msk = msk_base;
		for (size_t n = 0; n < (len - l); ++n)
		{
			if (isMatch(rangeStart + n, patt_base, msk_base))
			{
				free(patt_base);
				free(msk_base);
				return n;
			}
		}
		free(patt_base);
		free(msk_base);
	}
	return -1;
}

uint64_t Memory::get_proc_baseaddr()
{
	return proc.baseaddr;
}

process_status Memory::get_proc_status()
{
	return status;
}

void Memory::check_proc()
{
	if (status == process_status::FOUND_READY || status == process_status::FOUND_NO_ACCESS)
	{
		short c;
		Read<short>(proc.baseaddr, c);

		if (c != 0x5A4D)
		{
			status = process_status::FOUND_NO_ACCESS;
		}
		else
		{
			status = process_status::FOUND_READY;
		}
	}
}

bool kernel_init(Inventory *inv, const char *connector_name)
{
	if (mf_inventory_create_connector(inv, connector_name, "", conn.get()))
	{
		printf("Can't create %s connector\n", connector_name);
		return false;
	}
	else
	{
		printf("%s connector created\n", connector_name);
	}

	kernel = std::make_unique<OsInstance<>>();
	if (mf_inventory_create_os(inv, "win32", "", conn.get(), kernel.get()))
	{
		printf("Unable to initialize kernel using %s connector\n", connector_name);
		mf_connector_drop(conn.get());
		kernel.reset();
		return false;
	}

	return true;
}

bool Memory::testDtbValue(const uint64_t &dtb_val)
{
	if (proc.baseaddr == 0) return false;

	uint64_t phys_base = TranslateVirtualToPhysical(dtb_val, proc.baseaddr);
	if (phys_base == 0) return false;

	short c = 0;
	if (!ReadPhysical(phys_base, &c, sizeof(c))) return false;

	if (c == 0x5A4D)
	{
		lastCorrectDtbPhysicalAddress = dtb_val;
		return true;
	}

	return false;
}

bool Memory::ReadPhysical(uint64_t address, void* buffer, size_t len)
{
	if (!conn) return false;

	CSliceMut<uint8_t> slice;
	slice.data = (uint8_t*)buffer;
	slice.len = len;

	return conn.get()->phys_view().read_raw_into(address, slice) == 0;
}

uint64_t Memory::scanPml4()
{
	if (!kernel || !conn) return 0;

	// Reset status for scanning
	status = process_status::NOT_FOUND;

	ProcessInfo system_info;
	if (kernel.get()->process_info_by_pid(4, &system_info))
	{
		return 0;
	}

	uint64_t system_dtb = system_info.dtb1;
	uint64_t system_pml4[512];
	if (!ReadPhysical(system_dtb, system_pml4, sizeof(system_pml4)))
	{
		return 0;
	}

	// PML4 entries 256-511 are kernel space and must be an EXACT match across all processes
	// using PID 4 (System) as the ground truth.
	uint64_t signature[256];
	int signature_entries = 0;
	for (int i = 256; i < 512; i++)
	{
		signature[i - 256] = system_pml4[i];
		if (system_pml4[i] != 0) signature_entries++;
	}

	if (signature_entries < 5) return 0; // Sanity check

	printf("[*] Scanning physical memory for PML4 signature (%d entries)...\n", signature_entries);
	auto start = std::chrono::high_resolution_clock::now();

	// Scan physical memory in 2MB chunks for efficiency
	const size_t scan_size = 2 * 1024 * 1024;
	uint8_t* buffer = (uint8_t*)malloc(scan_size);
	if (!buffer) return 0;

	uint64_t found_dtb = 0;
	// Only scan up to a reasonable physical limit to avoid hangs (e.g., 32GB)
	uint64_t search_limit = 0x800000000; // 32GB
	if (search_limit > MAX_PHYADDR) search_limit = MAX_PHYADDR;

	for (uint64_t addr = 0; addr < search_limit; addr += scan_size)
	{
		if (!ReadPhysical(addr, buffer, scan_size)) continue;

		for (size_t offset = 0; offset < scan_size; offset += 0x1000)
		{
			uint64_t* candidate_pml4 = (uint64_t*)(buffer + offset);

			// Quick check: first kernel entry should match
			if (candidate_pml4[256] != signature[0]) continue;

			bool match = true;
			for (int i = 257; i < 512; i++)
			{
				if (candidate_pml4[i] != signature[i - 256])
				{
					match = false;
					break;
				}
			}

			if (match)
			{
				uint64_t candidate_dtb = addr + offset;
				if (testDtbValue(candidate_dtb))
				{
					found_dtb = candidate_dtb;
					goto end;
				}
			}
		}
	}

end:
	free(buffer);
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start);
	printf("[+] PML4 scan %s, found DTB: 0x%lx, time: %ldms\n", found_dtb ? "success" : "failed", found_dtb, duration.count());
	return found_dtb;
}

bool Memory::IsMovedByEAC(uint64_t pml4eAddr, uint64_t pml4eContent)
{
	// From UnknownCheats: https://www.unknowncheats.me/forum/4243560-post1.html
	return (pml4eAddr & 0xff0000) == (pml4eContent & 0xff0000) && ((pml4eContent >> 48) & 0xffff) == 0;
}

uint64_t Memory::TranslateVirtualToPhysical(uint64_t dtb, uint64_t virtualAddr)
{
	dtb &= ~0xf;

	uint64_t pageOffset = virtualAddr & 0xFFF;
	uint64_t pte = (virtualAddr >> 12) & 0x1ff;
	uint64_t pt = (virtualAddr >> 21) & 0x1ff;
	uint64_t pd = (virtualAddr >> 30) & 0x1ff;
	uint64_t pdp = (virtualAddr >> 39) & 0x1ff;

	uint64_t pml4e_addr = dtb + 8 * pdp;
	uint64_t pml4e = 0;
	if (!ReadPhysical(pml4e_addr, &pml4e, sizeof(pml4e))) return 0;

	// Shadow CR3 Check from UnknownCheats
	if (IsMovedByEAC(pml4e_addr, pml4e)) return 0;

	if (!(pml4e & 1)) return 0;

	uint64_t pdpe_addr = (pml4e & 0xFFFFFFFFF000) + 8 * pd;
	uint64_t pdpe = 0;
	if (!ReadPhysical(pdpe_addr, &pdpe, sizeof(pdpe)) || !(pdpe & 1)) return 0;

	// 1GB Large Page
	if (pdpe & 0x80) return (pdpe & 0xFFFFFC000000) + (virtualAddr & 0x3FFFFFFF);

	uint64_t pde_addr = (pdpe & 0xFFFFFFFFF000) + 8 * pt;
	uint64_t pde = 0;
	if (!ReadPhysical(pde_addr, &pde, sizeof(pde)) || !(pde & 1)) return 0;

	// 2MB Large Page
	if (pde & 0x80) return (pde & 0xFFFFFFE00000) + (virtualAddr & 0x1FFFFF);

	uint64_t pte_addr = (pde & 0xFFFFFFFFF000) + 8 * pte;
	uint64_t pte_val = 0;
	if (!ReadPhysical(pte_addr, &pte_val, sizeof(pte_val)) || !(pte_val & 1)) return 0;

	return (pte_val & 0xFFFFFFFFF000) + pageOffset;
}

// https://www.unknowncheats.me/forum/apex-legends/670570-quick-obtain-cr3-check.html
bool Memory::bruteforceDtb(uint64_t dtbStartPhysicalAddr, const uint64_t stepPage)
{
	// eac cr3 always end with 0x-----XX000
	// dtbStartPhysicalAddr should be a multiple of 0x1000
	if ((dtbStartPhysicalAddr & 0xFFF) != 0)
		return false;
	if (dtbStartPhysicalAddr > MAX_PHYADDR)
		return false;

	dtbStartPhysicalAddr -= dtbStartPhysicalAddr % stepPage;
	dtbStartPhysicalAddr += lastCorrectDtbPhysicalAddress % stepPage;

	auto start = std::chrono::high_resolution_clock::now();
	bool result = false;
	uint64_t furtherDistance = GetFurtherDistance(dtbStartPhysicalAddr, 0x0, MAX_PHYADDR);
	size_t maxStep = furtherDistance / stepPage;
	uint64_t guessDtbAddr = 0;

	for (size_t step = 0; step < maxStep; step++)
	{
		// bruteforce dtb from middle
		guessDtbAddr = dtbStartPhysicalAddr + step * stepPage;
		if (guessDtbAddr < MAX_PHYADDR)
		{
			if (testDtbValue(guessDtbAddr))
			{
				result = true;
				break;
			}
		}
		// dont forget the other side
		if (dtbStartPhysicalAddr > step * stepPage)
		{
			guessDtbAddr = dtbStartPhysicalAddr - step * stepPage;
			if (testDtbValue(guessDtbAddr))
			{
				result = true;
				break;
			}
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	printf("[+] bruteforce dtb %s to find dtb:0x%lx, time:%ldms\n", result ? "success" : "failed", result ? guessDtbAddr : 0x0, duration.count());

	// In case we cannot get the dtb through this shortcut method.
	if (result == false && stepPage != 0x1000)
	{
		return bruteforceDtb(dtbStartPhysicalAddr, 0x1000);
	}
	return result;
}

void Memory::open_proc(const char *name)
{
	std::lock_guard<std::mutex> l(m);
	if (!conn)
	{
		conn = std::make_unique<ConnectorInstance<>>();
		Inventory *inv = mf_inventory_scan();
		mf_inventory_add_dir(inv, ".");

		printf("Init with kvm connector...\n");
		if (!kernel_init(inv, "kvm"))
		{
		printf("Init with qemu connector...\n");
		if (!kernel_init(inv, "qemu"))
			{
				printf("Quitting\n");
				mf_inventory_free(inv);
				exit(1);
			}
		}

		printf("Kernel initialized: %p\n", kernel.get()->container.instance.instance);
	}


	ProcessInfo info;
	info.dtb2 = Address_INVALID;

	if (kernel.get()->process_info_by_name(name, &info))
	{
		status = process_status::NOT_FOUND;
		return;
	}

	auto base_section = std::make_unique<char[]>(8);
	uint64_t *base_section_value = (uint64_t *)base_section.get();
	CSliceMut<uint8_t> slice(base_section.get(), 8);
	uint32_t EPROCESS_SectionBaseAddress_off = 0x520; // win10 >= 20H1
	kernel.get()->read_raw_into(info.address + EPROCESS_SectionBaseAddress_off, slice);
	proc.baseaddr = *base_section_value;

	if (lastCorrectDtbPhysicalAddress && testDtbValue(lastCorrectDtbPhysicalAddress))
	{
		info.dtb1 = lastCorrectDtbPhysicalAddress;
		if (kernel.get()->clone().into_process_by_info(info, &proc.hProcess) == 0)
		{
			status = process_status::FOUND_READY;
			return;
		}
	}

	// Try standard initialization first
	if (kernel.get()->clone().into_process_by_info(info, &proc.hProcess) == 0)
	{
		ModuleInfo module_info;
		if (proc.hProcess.module_by_name(name, &module_info) == 0)
		{
			proc.baseaddr = module_info.base;
			status = process_status::FOUND_READY;
			return;
		}
	}

	// If standard failed or module not found (EAC protection), use PML4 scanner
	uint64_t found_dtb = scanPml4();
	if (found_dtb)
	{
		info.dtb1 = found_dtb;
		if (kernel.get()->clone().into_process_by_info(info, &proc.hProcess) == 0)
		{
			lastCorrectDtbPhysicalAddress = found_dtb;
			status = process_status::FOUND_READY;
			return;
		}
	}

	status = process_status::FOUND_NO_ACCESS;
	printf("Error: Unable to acquire process %s with valid DTB\n", name);
}

void Memory::close_proc()
{
	std::lock_guard<std::mutex> l(m);
	proc.baseaddr = 0;
	lastCorrectDtbPhysicalAddress = 0;
	status = process_status::NOT_FOUND;
	proc.hProcess = IntoProcessInstance<>();
}

uint64_t Memory::ScanPointer(uint64_t ptr_address, const uint32_t offsets[], int level)
{
	if (!ptr_address)
		return 0;

	uint64_t lvl = ptr_address;

	for (int i = 0; i < level; i++)
	{
		if (!Read<uint64_t>(lvl, lvl) || !lvl)
			return 0;
		lvl += offsets[i];
	}

	return lvl;
}

bool Memory::Dump(const char *filename)
{
	if (status != process_status::FOUND_READY)
		return false;

	printf("Dumping memory to %s...\n", filename);

	uint32_t e_lfanew = 0;
	if (!Read<uint32_t>(proc.baseaddr + 0x3C, e_lfanew))
		return false;

	uint32_t SizeOfImage = 0;
	if (!Read<uint32_t>(proc.baseaddr + e_lfanew + 0x50, SizeOfImage))
		return false;

	printf("Size of image: %x\n", SizeOfImage);

	if (SizeOfImage == 0 || SizeOfImage > 0x10000000) // 256MB limit for safety
		return false;

	char *buffer = (char *)malloc(SizeOfImage);
	if (!buffer)
		return false;

	// Read in chunks of 1MB
	uint32_t chunkSize = 1024 * 1024;
	for (uint32_t offset = 0; offset < SizeOfImage; offset += chunkSize)
	{
		uint32_t currentChunk = (SizeOfImage - offset < chunkSize) ? SizeOfImage - offset : chunkSize;
		ReadArray<char>(proc.baseaddr + offset, buffer + offset, currentChunk);
	}

	FILE *f = fopen(filename, "wb");
	if (f)
	{
		fwrite(buffer, 1, SizeOfImage, f);
		fclose(f);
		free(buffer);
		printf("Dump complete.\n");
		return true;
	}

	free(buffer);
	return false;
}
