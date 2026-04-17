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

static std::unique_ptr<ConnectorInstance<>> conn = nullptr;
static std::unique_ptr<OsInstance<>> kernel = nullptr;

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

class Memory
{
private:
	Process proc;
	process_status status = process_status::NOT_FOUND;
	std::mutex m;
	uint64_t lastCorrectDtbPhysicalAddress = 0x0;

public:
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

	uint64_t ScanPointer(uint64_t ptr_address, const uint32_t offsets[], int level);

	bool testDtbValue(const uint64_t &dtb_val);

	bool ReadPhysical(uint64_t address, void* buffer, size_t len);

	uint64_t scanPml4();

	bool IsMovedByEAC(uint64_t pml4eAddr, uint64_t pml4eContent);

	uint64_t TranslateVirtualToPhysical(uint64_t dtb, uint64_t virtualAddr);

	uint64_t GetPfnDtb(uint64_t eprocess_ptr);

	bool Dump(const char *filename);
};

typedef struct _MMPFN {
	uintptr_t flags;
	uintptr_t pte_address;
	uintptr_t Unused_1;
	uintptr_t Unused_2;
	uintptr_t Unused_3;
	uintptr_t Unused_4;
} _MMPFN;

typedef struct _LIST_ENTRY64 {
	uint64_t Flink;
	uint64_t Blink;
} LIST_ENTRY64;

typedef struct _DBGKD_DEBUG_DATA_HEADER64 {
	LIST_ENTRY64 List;
	uint32_t OwnerTag;
	uint32_t Size;
} DBGKD_DEBUG_DATA_HEADER64, *PDBGKD_DEBUG_DATA_HEADER64;

typedef struct _KDDEBUGGER_DATA64 {
	DBGKD_DEBUG_DATA_HEADER64 Header;
	uint64_t KernBase;
	uint64_t BreakpointWithStatus;
	uint64_t SavedContext;
	uint16_t ThCallbackStack;
	uint16_t NextCallback;
	uint16_t FramePointer;
	uint16_t PaeEnabled : 1;
	uint16_t KiBugCheckRecoveryActive : 1;
	uint16_t PagingLevels : 4;
	uint64_t KiCallUserMode;
	uint64_t KeUserCallbackDispatcher;
	uint64_t PsLoadedModuleList;
	uint64_t PsActiveProcessHead;
	uint64_t PspCidTable;
	uint64_t ExpSystemResourcesList;
	uint64_t ExpPagedPoolDescriptor;
	uint64_t ExpNumberOfPagedPools;
	uint64_t KeTimeIncrement;
	uint64_t KiBugcheckData;
	uint64_t IopErrorLogListHead;
	uint64_t ObpRootDirectoryObject;
	uint64_t ObpTypeObjectType;
	uint64_t MmSystemCacheStart;
	uint64_t MmSystemCacheEnd;
	uint64_t MmSystemCacheWs;
	uint64_t MmPfnDatabase;
	uint64_t MmSystemPtesStart;
	uint64_t MmSystemPtesEnd;
	uint64_t MmSubsectionBase;
	uint64_t MmNumberOfPagingFiles;
	uint64_t MmLowestPhysicalPage;
	uint64_t MmHighestPhysicalPage;
	uint64_t MmNumberOfPhysicalPages;
	uint64_t MmMaximumNonPagedPoolInBytes;
	uint64_t MmNonPagedSystemStart;
	uint64_t MmNonPagedPoolStart;
	uint64_t MmNonPagedPoolEnd;
	uint64_t MmPagedPoolStart;
	uint64_t MmPagedPoolEnd;
	uint64_t MmPagedPoolInformation;
	uint64_t MmPageSize;
	uint64_t MmSizeOfPagedPoolInBytes;
	uint64_t MmTotalCommitLimit;
	uint64_t MmTotalCommittedPages;
	uint64_t MmSharedCommit;
	uint64_t MmDriverCommit;
	uint64_t MmProcessCommit;
	uint64_t MmPagedPoolCommit;
	uint64_t MmExtendedCommit;
	uint64_t MmZeroedPageListHead;
	uint64_t MmFreePageListHead;
	uint64_t MmStandbyPageListHead;
	uint64_t MmModifiedPageListHead;
	uint64_t MmModifiedNoWritePageListHead;
	uint64_t MmAvailablePages;
	uint64_t MmResidentAvailablePages;
	uint64_t PoolTrackTable;
	uint64_t NonPagedPoolDescriptor;
	uint64_t MmHighestUserAddress;
	uint64_t MmSystemRangeStart;
	uint64_t MmUserProbeAddress;
	uint64_t KdPrintCircularBuffer;
	uint64_t KdPrintCircularBufferEnd;
	uint64_t KdPrintWritePointer;
	uint64_t KdPrintRolloverCount;
	uint64_t MmLoadedUserImageList;
	uint64_t NtBuildLab;
	uint64_t KiNormalSystemCall;
	uint64_t KiProcessorBlock;
	uint64_t MmUnloadedDrivers;
	uint64_t MmLastUnloadedDriver;
	uint64_t MmTriageActionTaken;
	uint64_t MmSpecialPoolTag;
	uint64_t KernelVerifier;
	uint64_t MmVerifierData;
	uint64_t MmAllocatedNonPagedPool;
	uint64_t MmPeakCommitment;
	uint64_t MmTotalCommitLimitMaximum;
	uint64_t CmNtCSDVersion;
	uint64_t MmPhysicalMemoryBlock;
	uint64_t MmSessionBase;
	uint64_t MmSessionSize;
	uint64_t MmSystemParentTablePage;
	uint64_t MmVirtualTranslationBase;
	uint16_t OffsetKThreadNextProcessor;
	uint16_t OffsetKThreadTeb;
	uint16_t OffsetKThreadKernelStack;
	uint16_t OffsetKThreadInitialStack;
	uint16_t OffsetKThreadApcProcess;
	uint16_t OffsetKThreadState;
	uint16_t OffsetKThreadBStore;
	uint16_t OffsetKThreadBStoreLimit;
	uint16_t SizeEProcess;
	uint16_t OffsetEprocessPeb;
	uint16_t OffsetEprocessParentCID;
	uint16_t OffsetEprocessDirectoryTableBase;
	uint16_t SizePrcb;
	uint16_t OffsetPrcbDpcRoutine;
	uint16_t OffsetPrcbCurrentThread;
	uint16_t OffsetPrcbMhz;
	uint16_t OffsetPrcbCpuType;
	uint16_t OffsetPrcbVendorString;
	uint16_t OffsetPrcbProcStateContext;
	uint16_t OffsetPrcbNumber;
	uint16_t SizeEThread;
	uint8_t L1tfHighPhysicalBitIndex;
	uint8_t L1tfSwizzleBitIndex;
	uint32_t Padding0;
	uint64_t KdPrintCircularBufferPtr;
	uint64_t KdPrintBufferSize;
	uint64_t KeLoaderBlock;
	uint16_t SizePcr;
	uint16_t OffsetPcrSelfPcr;
	uint16_t OffsetPcrCurrentPrcb;
	uint16_t OffsetPcrContainedPrcb;
	uint16_t OffsetPcrInitialBStore;
	uint16_t OffsetPcrBStoreLimit;
	uint16_t OffsetPcrInitialStack;
	uint16_t OffsetPcrStackLimit;
	uint16_t OffsetPrcbPcrPage;
	uint16_t OffsetPrcbProcStateSpecialReg;
	uint16_t GdtR0Code;
	uint16_t GdtR0Data;
	uint16_t GdtR0Pcr;
	uint16_t GdtR3Code;
	uint16_t GdtR3Data;
	uint16_t GdtR3Teb;
	uint16_t GdtLdt;
	uint16_t GdtTss;
	uint16_t Gdt64R3CmCode;
	uint16_t Gdt64R3CmTeb;
	uint64_t IopNumTriageDumpDataBlocks;
	uint64_t IopTriageDumpDataBlocks;
	uint64_t VfCrashDataBlock;
	uint64_t MmBadPagesDetected;
	uint64_t MmZeroedPageSingleBitErrorsDetected;
	uint64_t EtwpDebuggerData;
	uint16_t OffsetPrcbContext;
	uint16_t OffsetPrcbMaxBreakpoints;
	uint16_t OffsetPrcbMaxWatchpoints;
	uint32_t OffsetKThreadStackLimit;
	uint32_t OffsetKThreadStackBase;
	uint32_t OffsetKThreadQueueListEntry;
	uint32_t OffsetEThreadIrpList;
	uint16_t OffsetPrcbIdleThread;
	uint16_t OffsetPrcbNormalDpcState;
	uint16_t OffsetPrcbDpcStack;
	uint16_t OffsetPrcbIsrStack;
	uint16_t SizeKDPC_STACK_FRAME;
	uint16_t OffsetKPriQueueThreadListHead;
	uint16_t OffsetKThreadWaitReason;
	uint16_t Padding1;
	uint64_t PteBase;
	uint64_t RetpolineStubFunctionTable;
	uint32_t RetpolineStubFunctionTableSize;
	uint32_t RetpolineStubOffset;
	uint32_t RetpolineStubSize;
	uint16_t OffsetEProcessMmHotPatchContext;
	uint32_t OffsetKThreadShadowStackLimit;
	uint32_t OffsetKThreadShadowStackBase;
	uint64_t ShadowStackEnabled;
	uint64_t PointerAuthMask;
	uint16_t OffsetPrcbExceptionStack;
} KDDEBUGGER_DATA64, *PKDDEBUGGER_DATA64;

template <typename T>
inline bool Memory::Read(uint64_t address, T &out)
{
	std::lock_guard<std::mutex> l(m);
	return proc.baseaddr && proc.hProcess.read_raw_into(address, CSliceMut<uint8_t>((char *)&out, sizeof(T))) == 0;
}

template <typename T>
inline bool Memory::ReadArray(uint64_t address, T out[], size_t len)
{
	std::lock_guard<std::mutex> l(m);
	return proc.baseaddr && proc.hProcess.read_raw_into(address, CSliceMut<uint8_t>((char *)out, sizeof(T) * len)) == 0;
}

template <typename T>
inline bool Memory::Write(uint64_t address, const T &value)
{
	std::lock_guard<std::mutex> l(m);
	return proc.baseaddr && proc.hProcess.write_raw(address, CSliceRef<uint8_t>((char *)&value, sizeof(T))) == 0;
}

template <typename T>
inline bool Memory::WriteArray(uint64_t address, const T value[], size_t len)
{
	std::lock_guard<std::mutex> l(m);
	return proc.baseaddr && proc.hProcess.write_raw(address, CSliceRef<uint8_t>((char *)value, sizeof(T) * len)) == 0;
}
