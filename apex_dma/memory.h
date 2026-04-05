#pragma once
#include "memflow.hpp"
#include <cstring>
#include <stdio.h>
#include <mutex>
#include <memory>

#define INRANGE(x, a, b) (x >= a && x <= b)
#define getBits(x) (INRANGE(x, '0', '9') ? (x - '0') : ((x & (~0x20)) - 'A' + 0xa))
#define getByte(x) (getBits(x[0]) << 4 | getBits(x[1]))

typedef uint8_t *PBYTE;
typedef uint8_t BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef WORD *PWORD;

extern ConnectorInstance<> conn;
extern OsInstance<> kernel;
extern Inventory *inventory;

// set MAX_PHYADDR to a reasonable value, larger values will take more time to traverse.
constexpr uint64_t MAX_PHYADDR = 0xFFFFFFFFF;

inline uint64_t GetFurtherDistance(uint64_t A, uint64_t Min, uint64_t Max)
{
	uint64_t distanceToMin = (A > Min) ? (A - Min) : (Min - A);
	uint64_t distanceToMax = (A > Max) ? (A - Max) : (Max - A);

	return (distanceToMin > distanceToMax) ? distanceToMin : distanceToMax;
}

inline bool isMatch(const PBYTE addr, const PBYTE pat, const PBYTE msk)
{
	size_t n = 0;
	while (addr[n] == pat[n] || msk[n] == (BYTE)'?')
	{
		if (!msk[++n])
		{
			return true;
		}
	}
	return false;
}

size_t findPattern(const PBYTE rangeStart, size_t len, const char *pattern);

typedef struct Process
{
	IntoProcessInstance<> hProcess;
	uint64_t baseaddr = 0;
} Process;

enum class process_status : BYTE
{
	NOT_FOUND,
	FOUND_NO_ACCESS,
	FOUND_READY
};

struct DTBCache {
	uint64_t Address;
	uint64_t Value;
};

class Memory
{
private:
	Process proc;
	process_status status = process_status::NOT_FOUND;
	std::mutex m;
	uint64_t lastCorrectDtbPhysicalAddress = 0x0;
	DTBCache pml4_cache[512];

public:
	Memory() {
		memset(pml4_cache, 0, sizeof(pml4_cache));
	}
	~Memory() = default;

	uint64_t get_proc_baseaddr();

	process_status get_proc_status();

	void check_proc();

	void open_proc(const char *name);

	void close_proc();

	template <typename T>
	bool Read(uint64_t address, T &out);

	template <typename T>
	bool ReadArray(uint64_t address, T out[], size_t len);

	template <typename T>
	bool Write(uint64_t address, const T &value);

	template <typename T>
	bool WriteArray(uint64_t address, const T value[], size_t len);

	bool ResolveDtb();

	uint64_t ScanPointer(uint64_t ptr_address, const uint32_t offsets[], int level);

	bool bruteforceDtb(uint64_t dtbStartPhysicalAddr, const uint64_t stepPage);

	bool testDtbValue(const uint64_t &dtb_val);

	bool Dump(const char *filename);

	uint64_t VTranslate(uint64_t dtb, uint64_t vaddr);

	bool IsMovedByEAC(uint64_t pml4e_addr, uint64_t pml4e);

	uint64_t ReadCachedPML4E(uint64_t dtb, uint64_t pml4e_index);

	bool ReadPhysical(uint64_t address, void* buffer, size_t size);
};

template <typename T>
inline bool Memory::Read(uint64_t address, T &out)
{
	std::lock_guard<std::mutex> l(m);
	if (!proc.baseaddr) return false;
	if (proc.hProcess.read_raw_into(address, CSliceMut<uint8_t>((char *)&out, sizeof(T))) == 0)
		return true;
	if (ResolveDtb())
		return proc.hProcess.read_raw_into(address, CSliceMut<uint8_t>((char *)&out, sizeof(T))) == 0;
	return false;
}

template <typename T>
inline bool Memory::ReadArray(uint64_t address, T out[], size_t len)
{
	std::lock_guard<std::mutex> l(m);
	if (!proc.baseaddr) return false;
	if (proc.hProcess.read_raw_into(address, CSliceMut<uint8_t>((char *)out, sizeof(T) * len)) == 0)
		return true;
	if (ResolveDtb())
		return proc.hProcess.read_raw_into(address, CSliceMut<uint8_t>((char *)out, sizeof(T) * len)) == 0;
	return false;
}

template <typename T>
inline bool Memory::Write(uint64_t address, const T &value)
{
	std::lock_guard<std::mutex> l(m);
	if (!proc.baseaddr) return false;
	if (proc.hProcess.write_raw(address, CSliceRef<uint8_t>((char *)&value, sizeof(T))) == 0)
		return true;
	if (ResolveDtb())
		return proc.hProcess.write_raw(address, CSliceRef<uint8_t>((char *)&value, sizeof(T))) == 0;
	return false;
}

template <typename T>
inline bool Memory::WriteArray(uint64_t address, const T value[], size_t len)
{
	std::lock_guard<std::mutex> l(m);
	if (!proc.baseaddr) return false;
	if (proc.hProcess.write_raw(address, CSliceRef<uint8_t>((char *)value, sizeof(T) * len)) == 0)
		return true;
	if (ResolveDtb())
		return proc.hProcess.write_raw(address, CSliceRef<uint8_t>((char *)value, sizeof(T) * len)) == 0;
	return false;
}
