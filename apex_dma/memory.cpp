#include "memory.h"
#include <unistd.h>
#include <vector>
#include <cstring>

static std::recursive_mutex static_m;

static uint64_t g_mm_pfn_database = 0;
static uint64_t g_pte_base = 0;
static uint64_t g_pxe_base = 0;
static uint64_t g_index_offset = 0;

bool GetKernelInfo(uint64_t& base, uint32_t& size)
{
	ModuleInfo info;
	if (kernel && kernel.get()->vtbl_os && kernel.get()->module_by_name("ntoskrnl.exe", &info) == 0)
	{
		base = info.base;
		size = (uint32_t)info.size;
		return true;
	}
	return false;
}

bool InitializePfnData()
{
	if (g_mm_pfn_database && g_pxe_base) return true;

	uint64_t ntos_base;
	uint32_t ntos_size;
	if (!GetKernelInfo(ntos_base, ntos_size)) return false;

	uint8_t* buffer = (uint8_t*)malloc(ntos_size);
	if (!buffer) return false;

	if (kernel.get()->read_raw_into(ntos_base, CSliceMut<uint8_t>((char*)buffer, ntos_size)) != 0)
	{
		free(buffer);
		return false;
	}

	// Signature for MmPteBase (Win10/11)
	size_t pte_base_off = findPattern(buffer, ntos_size, "48 8B 05 ? ? ? ? 48 8B 00 48 8B 00 48 89 05");
	if (pte_base_off != -1)
	{
		int32_t rel = *(int32_t*)(buffer + pte_base_off + 3);
		uint64_t pte_base_ptr = ntos_base + pte_base_off + 7 + rel;
		kernel.get()->read_raw_into(pte_base_ptr, CSliceMut<uint8_t>((char*)&g_pte_base, 8));

		if (g_pte_base)
		{
			uint64_t pde_base = g_pte_base + ((g_pte_base & 0xffffffffffffULL) >> 9);
			uint64_t ppe_base = g_pte_base + ((pde_base & 0xffffffffffffULL) >> 9);
			g_pxe_base = g_pte_base + ((ppe_base & 0xffffffffffffULL) >> 9);
			g_index_offset = (g_pte_base >> 39) - 0x1FFFE00ULL;
		}
	}

	// Signature for MmPfnDatabase (Win10/11)
	size_t pfn_db_off = findPattern(buffer, ntos_size, "48 8B 05 ? ? ? ? 48 83 C0 08 48 89 05");
	if (pfn_db_off == -1)
	{
		// Alternate signature
		pfn_db_off = findPattern(buffer, ntos_size, "48 8B 05 ? ? ? ? 48 03 C0 48 8B 00 48 8B 00");
	}

	if (pfn_db_off != -1)
	{
		int32_t rel = *(int32_t*)(buffer + pfn_db_off + 3);
		uint64_t pfn_db_ptr = ntos_base + pfn_db_off + 7 + rel;
		kernel.get()->read_raw_into(pfn_db_ptr, CSliceMut<uint8_t>((char*)&g_mm_pfn_database, 8));
	}

	free(buffer);
	return g_mm_pfn_database != 0 && g_pxe_base != 0;
}

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
	std::lock_guard<std::recursive_mutex> l(static_m);

	ConnectorInstance<> temp_conn;
	if (mf_inventory_create_connector(inv, connector_name, "", &temp_conn))
	{
		printf("Can't create %s connector\n", connector_name);
		return false;
	}
	else
	{
		printf("%s connector created\n", connector_name);
	}

	// Clone the temporary connector into our static global 'conn'
	conn = std::make_unique<ConnectorInstance<>>();
	mf_connector_clone(&temp_conn, conn.get());

	kernel = std::make_unique<OsInstance<>>();
	// Important: mf_inventory_create_os MOVES the temp_conn, zeroing its vtables.
	if (mf_inventory_create_os(inv, "win32", "", &temp_conn, kernel.get()))
	{
		printf("Unable to initialize kernel using %s connector\n", connector_name);
		mf_connector_drop(&temp_conn);
		mf_connector_drop(conn.get());
		kernel.reset();
		conn.reset();
		return false;
	}

	// mf_inventory_create_os MOVED the temp_conn. We must forget its container locally to avoid double-drop
	// when 'temp_conn' goes out of scope and its destructor is called.
	mem_forget(temp_conn.container);

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
	std::lock_guard<std::recursive_mutex> l(static_m);
	if (!conn || !conn.get()->vtbl_physicalmemory) return false;

	if (address + len > MAX_PHYADDR) return false;

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

	// PML4 entries 256-511 are kernel space and must be a near-exact match across all processes
	// using PID 4 (System) as the ground truth.
	uint64_t signature[256];
	int signature_entries = 0;
	int first_entry_idx = -1;
	for (int i = 256; i < 512; i++)
	{
		signature[i - 256] = system_pml4[i];
		if (system_pml4[i] != 0)
		{
			signature_entries++;
			if (first_entry_idx == -1) first_entry_idx = i - 256;
		}
	}

	if (signature_entries < 3 || first_entry_idx == -1) return 0; // Sanity check

	printf("[*] Scanning physical memory for PML4 signature (%d entries, first at index %d)...\n", signature_entries, 256 + first_entry_idx);
	auto start = std::chrono::high_resolution_clock::now();

	// Scan physical memory in 2MB chunks for efficiency
	const size_t scan_size = 2 * 1024 * 1024;
	uint8_t* buffer = (uint8_t*)malloc(scan_size);
	if (!buffer) return 0;

	uint64_t found_dtb = 0;
	// Use 64GB as the limit for modern systems with high RAM or memory holes
	uint64_t search_limit = 0x1000000000; // 64GB
	if (search_limit > MAX_PHYADDR) search_limit = MAX_PHYADDR;

	const uint64_t MASK = 0x000FFFFFFFFFF001; // PFN + Present bit

	for (uint64_t addr = 0; addr < search_limit; addr += scan_size)
	{
		bool block_read_success = ReadPhysical(addr, buffer, scan_size);

		for (size_t offset = 0; offset < scan_size; offset += 0x1000)
		{
			uint64_t candidate_dtb = addr + offset;
			uint64_t* candidate_pml4;
			uint64_t local_page[512];

			if (block_read_success)
			{
				candidate_pml4 = (uint64_t*)(buffer + offset);
			}
			else
			{
				// Block read failed, likely due to a gap. Try reading just this page.
				if (!ReadPhysical(candidate_dtb, local_page, sizeof(local_page))) continue;
				candidate_pml4 = local_page;
			}

			// Quick check: first non-zero kernel entry should match (masking volatile flags)
			if ((candidate_pml4[256 + first_entry_idx] & MASK) != (signature[first_entry_idx] & MASK)) continue;

			int current_matches = 0;
			for (int i = 256; i < 512; i++)
			{
				if (signature[i - 256] == 0) continue;
				if ((candidate_pml4[i] & MASK) == (signature[i - 256] & MASK))
				{
					current_matches++;
				}
			}

			// Require at least 90% match for robustness
			if (current_matches >= (signature_entries * 9 / 10))
			{
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

uint64_t Memory::GetPfnDtb(uint64_t eprocess_ptr)
{
	std::lock_guard<std::recursive_mutex> l(static_m);
	if (!kernel || !conn) return 0;

	if (!g_mm_pfn_database || !g_pxe_base)
	{
		InitializePfnData();
	}

	if (!g_mm_pfn_database || !g_pxe_base) return 0;

	const uint64_t cr3_pte = g_index_offset * 8 + g_pxe_base;

	PhysicalMemoryMetadata meta = conn.get()->metadata();
	uint64_t search_limit = meta.max_address;
	if (search_limit > MAX_PHYADDR) search_limit = MAX_PHYADDR;

	printf("[*] Searching PFN database for EPROCESS 0x%lx (cr3_pte: 0x%lx)...\n", eprocess_ptr, cr3_pte);
	auto start = std::chrono::high_resolution_clock::now();

	const size_t entries_per_chunk = 4096;
	std::vector<_MMPFN> chunk(entries_per_chunk);

	for (uint64_t pfn_start = 0; pfn_start < (search_limit >> 12); pfn_start += entries_per_chunk)
	{
		uint64_t chunk_ptr = g_mm_pfn_database + sizeof(_MMPFN) * pfn_start;
		if (kernel.get()->read_raw_into(chunk_ptr, CSliceMut<uint8_t>((char*)chunk.data(), sizeof(_MMPFN) * entries_per_chunk)) != 0)
			continue;

		for (size_t i = 0; i < entries_per_chunk; i++)
		{
			if (chunk[i].pte_address == cr3_pte)
			{
				uint64_t eproc_candidate = (((chunk[i].flags | 0xF000000000000000ULL) >> 0xD) | 0xFFFF000000000000ULL);
				if (eproc_candidate == eprocess_ptr)
				{
					uint64_t dtb = (pfn_start + i) << 12;
					if (testDtbValue(dtb))
					{
						auto end_time = std::chrono::high_resolution_clock::now();
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start);
						printf("[+] Found DTB via PFN: 0x%lx, time: %ldms\n", dtb, duration.count());
						return dtb;
					}
				}
			}
		}
	}

	return 0;
}

uint64_t Memory::TranslateVirtualToPhysical(uint64_t dtb, uint64_t virtualAddr)
{
	dtb &= ~0xf;

	uint64_t pageOffset = virtualAddr & 0xFFF;
	uint64_t pte = (virtualAddr >> 12) & 0x1ff;
	uint64_t pt = (virtualAddr >> 21) & 0x1ff;
	uint64_t pd = (virtualAddr >> 30) & 0x1ff;
	uint64_t pdp = (virtualAddr >> 39) & 0x1ff;

	const uint64_t PMASK = 0x000FFFFFFFFFF000;

	uint64_t pml4e_addr = dtb + 8 * pdp;
	uint64_t pml4e = 0;
	if (!ReadPhysical(pml4e_addr, &pml4e, sizeof(pml4e))) return 0;

	// Shadow CR3 Check from UnknownCheats
	if (IsMovedByEAC(pml4e_addr, pml4e)) return 0;

	if (!(pml4e & 1)) return 0;

	uint64_t pdpe_addr = (pml4e & PMASK) + 8 * pd;
	uint64_t pdpe = 0;
	if (!ReadPhysical(pdpe_addr, &pdpe, sizeof(pdpe)) || !(pdpe & 1)) return 0;

	// 1GB Large Page
	if (pdpe & 0x80) return (pdpe & 0x000FFFFFC0000000) + (virtualAddr & 0x3FFFFFFF);

	uint64_t pde_addr = (pdpe & PMASK) + 8 * pt;
	uint64_t pde = 0;
	if (!ReadPhysical(pde_addr, &pde, sizeof(pde)) || !(pde & 1)) return 0;

	// 2MB Large Page
	if (pde & 0x80) return (pde & 0x000FFFFFFFE00000) + (virtualAddr & 0x1FFFFF);

	uint64_t pte_addr = (pde & PMASK) + 8 * pte;
	uint64_t pte_val = 0;
	if (!ReadPhysical(pte_addr, &pte_val, sizeof(pte_val)) || !(pte_val & 1)) return 0;

	return (pte_val & PMASK) + pageOffset;
}


void Memory::open_proc(const char *name)
{
	std::lock_guard<std::recursive_mutex> l(static_m);
	if (!conn)
	{
		printf("[*] Opening connector...\n");
		conn = std::make_unique<ConnectorInstance<>>();
		Inventory *inv = mf_inventory_scan();
		mf_inventory_add_dir(inv, ".");

		printf("Init with kvm connector...\n");
		if (!kernel_init(inv, "kvm"))
		{
			printf("[*] KVM failed, trying QEMU...\n");
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
	memset(&info, 0, sizeof(info));
	info.dtb2 = Address_INVALID;

	if (!kernel || !kernel.get()->vtbl_os)
	{
		status = process_status::NOT_FOUND;
		return;
	}

	if (kernel.get()->process_info_by_name(name, &info))
	{
		printf("[-] Process %s not found\n", name);
		status = process_status::NOT_FOUND;
		return;
	}

	auto base_section = std::make_unique<char[]>(8);
	uint64_t *base_section_value = (uint64_t *)base_section.get();
	CSliceMut<uint8_t> slice;
	slice.data = (uint8_t*)base_section.get();
	slice.len = 8;

	uint32_t EPROCESS_SectionBaseAddress_off = 0x520; // win10 >= 20H1
	if (info.address != 0 && kernel && kernel.get()->vtbl_memoryview && kernel.get()->read_raw_into(info.address + EPROCESS_SectionBaseAddress_off, slice) == 0)
	{
		proc.baseaddr = *base_section_value;
	}
	else
	{
		proc.baseaddr = 0;
	}

	if (lastCorrectDtbPhysicalAddress && testDtbValue(lastCorrectDtbPhysicalAddress))
	{
		printf("[+] Using last correct DTB: 0x%lx\n", lastCorrectDtbPhysicalAddress);
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

	// If standard failed or module not found (EAC protection), try PFN resolver then PML4 scanner
	uint64_t found_dtb = GetPfnDtb(info.address);
	if (found_dtb == 0)
	{
		found_dtb = scanPml4();
	}

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
	std::lock_guard<std::recursive_mutex> l(static_m);
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
