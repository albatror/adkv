#include "memory.h"
#include <unistd.h>

ConnectorInstance<> conn;
OsInstance<> kernel;
Inventory *inventory = nullptr;
std::recursive_mutex global_mem_mutex;

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
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (status == process_status::FOUND_READY || status == process_status::FOUND_NO_ACCESS)
	{
		short c = 0;
		if (proc.baseaddr)
		{
			uint64_t phys_addr = VTranslate(lastCorrectDtbPhysicalAddress, proc.baseaddr);
			if (phys_addr && ReadPhysical(phys_addr, &c, sizeof(c)) && c == 0x5A4D)
			{
				status = process_status::FOUND_READY;
				return;
			}
		}
		status = process_status::FOUND_NO_ACCESS;
	}
}

bool Memory::ResolveDtb()
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (lastCorrectDtbPhysicalAddress && testDtbValue(lastCorrectDtbPhysicalAddress))
		return true;

	if (!proc.baseaddr) return false;

	// The 1MB step heuristic: EAC's hidden DTBs often share the same lower 20 bits
	// as the original system-assigned DTB.
	if (bruteforceDtb(0x0, 0x100000))
		return true;

	// Fallback to slower 4KB step PML4 scan if 1MB step fails
	uint64_t vaddr = proc.baseaddr;
	uint64_t pdp = (vaddr >> 39) & 0x1ff;

	for (uint64_t addr = 0x0; addr < MAX_PHYADDR; addr += 0x1000)
	{
		uint64_t pml4e = 0;
		uint64_t pml4e_addr = addr + 8 * pdp;
		if (ReadPhysical(pml4e_addr, &pml4e, sizeof(pml4e)))
		{
			if (pml4e != 0 && (pml4e & 1) != 0 && (pml4e & 0x000ffffffffff000) < MAX_PHYADDR)
			{
				if (!IsMovedByEAC(pml4e_addr, pml4e))
				{
					if (testDtbValue(addr))
						return true;
				}
			}
		}
	}

	return false;
}

bool kernel_init(Inventory *inv, const char *connector_name)
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (mf_inventory_create_connector(inv, connector_name, "", &conn))
	{
		printf("Can't create %s connector\n", connector_name);
		return false;
	}
	else
	{
		printf("%s connector created\n", connector_name);
	}

	ConnectorInstance<> conn_clone;
	mf_connector_clone(&conn, &conn_clone);

	if (mf_inventory_create_os(inv, "win32", "", &conn_clone, &kernel))
	{
		printf("Unable to initialize kernel using %s connector\n", connector_name);
		kernel = OsInstance<>();
		return false;
	}

	return true;
}

bool Memory::ReadPhysical(uint64_t address, void* buffer, size_t size)
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (kernel.vtbl_physicalmemory)
	{
		MemoryViewBase<> view = kernel.phys_view();
		if (view.vtbl)
			return view.read_raw_into(address, CSliceMut<uint8_t>((char*)buffer, size)) == 0;
	}

	if (conn.vtbl_physicalmemory)
	{
		MemoryViewBase<> view = conn.phys_view();
		if (view.vtbl)
			return view.read_raw_into(address, CSliceMut<uint8_t>((char*)buffer, size)) == 0;
	}
	return false;
}

bool Memory::WritePhysical(uint64_t address, const void* buffer, size_t size)
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (kernel.vtbl_physicalmemory)
	{
		MemoryViewBase<> view = kernel.phys_view();
		if (view.vtbl)
			return view.write_raw(address, CSliceRef<uint8_t>((const char*)buffer, size)) == 0;
	}

	if (conn.vtbl_physicalmemory)
	{
		MemoryViewBase<> view = conn.phys_view();
		if (view.vtbl)
			return view.write_raw(address, CSliceRef<uint8_t>((const char*)buffer, size)) == 0;
	}
	return false;
}

bool Memory::IsMovedByEAC(uint64_t pml4e_addr, uint64_t pml4e)
{
	return (pml4e_addr & 0xff0000) == (pml4e & 0xff0000) && ((pml4e >> 48) & 0xffff) == 0;
}

uint64_t Memory::ReadCachedPML4E(uint64_t dtb, uint64_t pml4e_index)
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	uint64_t pml4e_addr = dtb + 8 * pml4e_index;
	if (pml4_cache[pml4e_index].Address == pml4e_addr && pml4_cache[pml4e_index].Value != 0)
		return pml4_cache[pml4e_index].Value;

	uint64_t pml4e = 0;
	if (ReadPhysical(pml4e_addr, &pml4e, sizeof(pml4e)))
	{
		pml4_cache[pml4e_index].Address = pml4e_addr;
		pml4_cache[pml4e_index].Value = pml4e;
		return pml4e;
	}
	return 0;
}

uint64_t Memory::VTranslate(uint64_t dtb, uint64_t vaddr)
{
	dtb &= ~0xFFF;
	uint64_t pageOffset = vaddr & 0xFFF;
	uint64_t pte = (vaddr >> 12) & 0x1ff;
	uint64_t pt = (vaddr >> 21) & 0x1ff;
	uint64_t pd = (vaddr >> 30) & 0x1ff;
	uint64_t pdp = (vaddr >> 39) & 0x1ff;

	uint64_t pml4e_addr = dtb + 8 * pdp;
	uint64_t pdpe = ReadCachedPML4E(dtb, pdp);

	if (IsMovedByEAC(pml4e_addr, pdpe))
		return 0;

	if ((pdpe & 1) == 0)
		return 0;

	uint64_t pde = 0;
	if (!ReadPhysical((pdpe & 0x000ffffffffff000) + 8 * pd, &pde, sizeof(pde)) || (pde & 1) == 0)
		return 0;

	if (pde & 0x80) // 1GB Large Page
		return (pde & 0x000fffffc0000000) + (vaddr & 0x3FFFFFFF);

	uint64_t pteAddr = 0;
	if (!ReadPhysical((pde & 0x000ffffffffff000) + 8 * pt, &pteAddr, sizeof(pteAddr)) || (pteAddr & 1) == 0)
		return 0;

	if (pteAddr & 0x80) // 2MB Large Page
		return (pteAddr & 0x000fffffffe00000) + (vaddr & 0x1FFFFF);

	uint64_t finalAddr = 0;
	if (!ReadPhysical((pteAddr & 0x000ffffffffff000) + 8 * pte, &finalAddr, sizeof(finalAddr)) || (finalAddr & 1) == 0)
		return 0;

	return (finalAddr & 0x000ffffffffff000) + pageOffset;
}

bool Memory::testDtbValue(const uint64_t &dtb_val)
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (!proc.baseaddr) return false;

	uint64_t phys_base = VTranslate(dtb_val, proc.baseaddr);
	if (phys_base == 0)
		return false;

	// Verify MZ header
	short c = 0;
	if (!ReadPhysical(phys_base, &c, sizeof(c)) || c != 0x5A4D)
		return false;

	// Verify PE header to avoid false positives
	uint32_t pe_offset = 0;
	if (!ReadPhysical(phys_base + 0x3C, &pe_offset, sizeof(pe_offset)) || pe_offset > 0x1000)
		return false;

	uint16_t pe_header = 0;
	if (!ReadPhysical(phys_base + pe_offset, &pe_header, sizeof(pe_header)) || pe_header != 0x4550)
		return false;

	lastCorrectDtbPhysicalAddress = dtb_val;
	if (proc.hProcess.vtbl_process)
	{
		proc.hProcess.set_dtb(dtb_val, Address_INVALID);
	}
	return true;
}

// https://www.unknowncheats.me/forum/apex-legends/670570-quick-obtain-cr3-check.html
bool Memory::bruteforceDtb(uint64_t dtbStartPhysicalAddr, const uint64_t stepPage)
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (!proc.baseaddr) return false;

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
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	if (!conn.vtbl_clone)
	{
		if (!inventory)
		{
			inventory = mf_inventory_scan();
			mf_inventory_add_dir(inventory, ".");
		}

		printf("Init with kvm connector...\n");
		if (!kernel_init(inventory, "kvm"))
		{
			printf("Init with qemu connector...\n");
			if (!kernel_init(inventory, "qemu"))
			{
				printf("Quitting\n");
				exit(1);
			}
		}

		if (kernel.vtbl_os)
			printf("Kernel initialized: %p\n", kernel.container.instance.instance);
		else
			printf("Kernel initialization failed (no vtbl)\n");
	}

	if (!kernel.vtbl_os) return;

	printf("Opening process %s...\n", name);
	ProcessInfo info;
	memset(&info, 0, sizeof(info));
	info.dtb2 = Address_INVALID;

	if (kernel.process_info_by_name(name, &info))
	{
		printf("Process %s not found\n", name);
		status = process_status::NOT_FOUND;
		return;
	}

	printf("Process found: pid=%d, addr=%lx, dtb=%lx\n", info.pid, info.address, info.dtb1);

	// Read base address first using OsInstance
	uint64_t base_section_value = 0;
	CSliceMut<uint8_t> slice((char*)&base_section_value, 8);
	uint32_t EPROCESS_SectionBaseAddress_off = 0x520; // win10 >= 20H1
	if (kernel.vtbl_memoryview)
	{
		kernel.read_raw_into(info.address + EPROCESS_SectionBaseAddress_off, slice);
		proc.baseaddr = base_section_value;
		printf("Section base address: %lx\n", proc.baseaddr);
	}
	else
	{
		proc.baseaddr = info.address;
	}

	// Try the DTB from info first
	if (info.dtb1 != 0 && testDtbValue(info.dtb1))
	{
		lastCorrectDtbPhysicalAddress = info.dtb1;
	}

	if (ResolveDtb())
	{
		printf("DTB resolved: %lx\n", lastCorrectDtbPhysicalAddress);
		// If ResolveDtb succeeded, we found a DTB, now initialize hProcess
		if (kernel.vtbl_clone && kernel.clone().into_process_by_info(info, &proc.hProcess) == 0)
		{
			proc.hProcess.set_dtb(lastCorrectDtbPhysicalAddress, Address_INVALID);
			status = process_status::FOUND_READY;
			printf("Process handle initialized with DTB\n");
			return;
		}
	}

	printf("Falling back to full handle acquisition...\n");
	close_proc();

	if (!kernel.vtbl_clone || kernel.clone().into_process_by_info(info, &proc.hProcess) != 0)
	{
		status = process_status::FOUND_NO_ACCESS;
		printf("Error while opening process %s (clone failed)\n", name);
		return;
	}
	else
	{
		// Successfully initialized hProcess, ensure it has the system DTB or found DTB
		if (lastCorrectDtbPhysicalAddress)
			proc.hProcess.set_dtb(lastCorrectDtbPhysicalAddress, Address_INVALID);
	}

	proc.baseaddr = info.address; // Fallback or re-initialize

	ModuleInfo module_info;
	memset(&module_info, 0, sizeof(module_info));
	if (proc.hProcess.vtbl_process && proc.hProcess.module_by_name(name, &module_info) == 0)
	{
		proc.baseaddr = module_info.base;
	}
	else
	{
		// If module lookup fails, use the EPROCESS section base address we read earlier
		proc.baseaddr = base_section_value;
		if (!ResolveDtb())
		{
			close_proc();
			return;
		}
	}

	status = process_status::FOUND_READY;
}

void Memory::close_proc()
{
	std::lock_guard<std::recursive_mutex> l(global_mem_mutex);
	proc.hProcess = IntoProcessInstance<>();
	lastCorrectDtbPhysicalAddress = 0;
	proc.baseaddr = 0;
	memset(pml4_cache, 0, sizeof(pml4_cache));
}

void GracefulExit()
{
	_exit(0);
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
