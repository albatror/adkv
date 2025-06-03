import ctypes
import os

# --- Load Shared Library ---
lib_name = "memflow.dll" if os.name == "nt" else "libmemflow.so"
memflow_lib = None
try:
    # For development, ensure the library is in a path where CDLL can find it.
    # This might be system PATH, LD_LIBRARY_PATH, or next to the script.
    # Or provide an absolute path if needed.
    memflow_lib = ctypes.CDLL(lib_name)
    print(f"Successfully loaded {lib_name}")
except OSError as e:
    print(f"Error loading {lib_name}: {e}")
    # Allow script to continue for partial definition, but FFI calls will fail.
    # In a real application, you might want to raise this error or exit.
    pass

# --- Define Core Types from memflow.h (using ctypes) ---

Address = ctypes.c_uint64
Pid = ctypes.c_uint32
umem = ctypes.c_uint64 # Used for sizes, lengths

# LevelFilter (enum-like constants)
# Actual values might depend on memflow's definition, using common logging levels
LevelFilter_Error = 1
LevelFilter_Warn = 2
LevelFilter_Info = 3
LevelFilter_Debug = 4
LevelFilter_Trace = 5
LevelFilter = ctypes.c_uintptr # Correct type for the enum based on memflow.h

class CSliceRef_u8(ctypes.Structure):
    _fields_ = [
        ("data", ctypes.POINTER(ctypes.c_uint8)),
        ("len", umem), # umem is c_uint64, but CSlice uses size_t which is umem in memflow
    ]

class CSliceMut_u8(ctypes.Structure):
    _fields_ = [
        ("data", ctypes.POINTER(ctypes.c_uint8)),
        ("len", umem),
    ]

# PhysicalAddress - Simplified, actual might involve more fields or specific packing
class PhysicalAddress(ctypes.Structure):
    _fields_ = [
        ("address", Address),
        ("page_type", ctypes.c_uint8),    # Placeholder, needs actual values from PageType enum
        ("page_size_log2", ctypes.c_uint8), # Placeholder
    ]


# ProcessState (enum from memflow.h)
PROCESS_STATE_NEW = 0       # Unused
PROCESS_STATE_ALIVE = 1     # Process is alive and well
PROCESS_STATE_DEAD = 2      # Process is dead and about to be cleaned up
PROCESS_STATE_CLEANED_UP = 3 # Process is dead and cleaned up by the os

# ArchitectureIdent (enum from memflow.h)
ARCH_UNKNOWN = 0
ARCH_X86 = 1       # x86 16-bit
ARCH_X64 = 2       # x86_64 64-bit
ARCH_ARM32 = 3     # ARM 32-bit
ARCH_AARCH64 = 4   # ARM 64-bit

# Simplified Architecture struct (from ArchitectureIdent in memflow.h)
class Architecture(ctypes.Structure):
    _fields_ = [
        ("tag", ctypes.c_int), # Corresponds to ArchitectureIdent enum
        # Placeholder for union members, e.g. for x64:
        # ("x64_selector_gs", ctypes.c_uint16),
        # ("x64_selector_fs", ctypes.c_uint16)
    ]

class ProcessState(ctypes.Structure):
    _fields_ = [
        ("tag", ctypes.c_int), # Corresponds to ProcessState enum
        ("exit_code", ctypes.c_int32) # Relevant if tag is PROCESS_STATE_DEAD
    ]

# ProcessInfo - More complete, but still simplified for Python.
# Full mapping of OS-specific parts (like WindowsEprocessInfo) is extensive.
class ProcessInfo(ctypes.Structure):
    _fields_ = [
        ("address", Address),      # Address of the process structure in kernel memory
        ("pid", Pid),              # Process ID
        ("state", ProcessState),   # Process state (tag + exit_code)
        ("name", ctypes.c_char_p), # Name of the process (string owned by memflow)
        ("path", ctypes.c_char_p), # Path to the process binary (string owned by memflow)
        ("command_line", ctypes.c_char_p), # Command line (string owned by memflow)
        ("sys_arch", Architecture),# System architecture on which the process is running
        ("proc_arch", Architecture),# Process architecture (e.g. for WoW64 processes)
        ("dtb1", Address),         # Primary page table (CR3 / TTBR0/1)
        ("dtb2", Address),         # Secondary page table (Kernel CR3 / TTBR0_EL1 for KPTI)
        # OS-specific ProcessInfo members are omitted for simplicity.
        # E.g., on Windows: section_base, ethread, teb_wow64 etc.
        # E.g., on Linux: real_uid, effective_uid etc.
    ]

# ModuleInfo (simplified from memflow.h)
class ModuleInfo(ctypes.Structure):
    _fields_ = [
        ("address", Address),      # Address of the module structure in kernel memory (if applicable)
        ("parent_process", Address), # Parent process for kernel modules, Address::INVALID for usermode
        ("base", Address),         # Base address of the module
        ("size", umem),            # Size of the module
        ("name", ctypes.c_char_p), # Name of the module (string owned by memflow)
        ("path", ctypes.c_char_p), # Path to the module (string owned by memflow)
        ("arch", Architecture),    # Architecture of the module
        # Other fields like 'exports', 'mem_type' omitted for simplicity
    ]


# Opaque pointer types (handle types)
Inventory_p = ctypes.c_void_p
ConnectorInstance_p = ctypes.c_void_p
OsInstance_p = ctypes.c_void_p
ProcessInstance_p = ctypes.c_void_p


# --- Helper for String Slices ---
def str_to_c_slice_ref(text: str) -> CSliceRef_u8:
    if text is None: # Handle None input gracefully
        return CSliceRef_u8(None, 0)
    text_bytes = text.encode('utf-8')
    # Keep a reference to text_bytes if the CSliceRef_u8 needs to live longer
    # than this function call, or copy to a ctypes buffer.
    # For immediate use, this should be okay.
    # However, ctypes.c_char_p(text_bytes) creates a pointer to the Python byte string.
    # If text_bytes goes out of scope and is garbage collected, this pointer becomes dangling.
    # A safer way for longer-lived slices or if issues arise:
    # buffer = ctypes.create_string_buffer(text_bytes)
    # return CSliceRef_u8(ctypes.cast(buffer, ctypes.POINTER(ctypes.c_uint8)), len(text_bytes))
    # For now, assuming short-lived usage for names/args.
    return CSliceRef_u8(ctypes.cast(ctypes.c_char_p(text_bytes), ctypes.POINTER(ctypes.c_uint8)), len(text_bytes))


# --- Define Function Signatures (prototypes) ---
# Only define if memflow_lib was loaded successfully
if memflow_lib:
    mf_log_init = memflow_lib.mf_log_init
    mf_log_init.argtypes = [LevelFilter]
    mf_log_init.restype = None

    mf_inventory_scan = memflow_lib.mf_inventory_scan
    mf_inventory_scan.argtypes = []
    mf_inventory_scan.restype = Inventory_p

    mf_inventory_free = memflow_lib.mf_inventory_free
    mf_inventory_free.argtypes = [Inventory_p]
    mf_inventory_free.restype = None

    # mf_inventory_create_connector(Inventory *inventory, const char *name, const char *args, ConnectorInstance **out);
    mf_inventory_create_connector = memflow_lib.mf_inventory_create_connector
    mf_inventory_create_connector.argtypes = [
        Inventory_p,
        ctypes.c_char_p, # name
        ctypes.c_char_p, # args
        ctypes.POINTER(ConnectorInstance_p) # out
    ]
    mf_inventory_create_connector.restype = ctypes.c_int32 # Returns status code

    # void mf_connector_drop(ConnectorInstance *connector);
    mf_connector_drop = memflow_lib.mf_connector_drop
    mf_connector_drop.argtypes = [ConnectorInstance_p]
    mf_connector_drop.restype = None

    # mf_inventory_create_os(Inventory *inventory, const char *name, const char *args, ConnectorInstance *connector, OsInstance **out);
    mf_inventory_create_os = memflow_lib.mf_inventory_create_os
    mf_inventory_create_os.argtypes = [
        Inventory_p,
        ctypes.c_char_p, # name
        ctypes.c_char_p, # args
        ConnectorInstance_p, # connector (can be NULL if OS can find its own)
        ctypes.POINTER(OsInstance_p) # out
    ]
    mf_inventory_create_os.restype = ctypes.c_int32

    # void mf_os_drop(OsInstance *os);
    mf_os_drop = memflow_lib.mf_os_drop
    mf_os_drop.argtypes = [OsInstance_p]
    mf_os_drop.restype = None

    # int32_t mf_osinstance_process_by_name(OsInstance *instance, CSliceRef_u8 name, ProcessInfo *info_out);
    mf_osinstance_process_by_name = memflow_lib.mf_osinstance_process_by_name
    mf_osinstance_process_by_name.argtypes = [
        OsInstance_p,
        CSliceRef_u8, # name
        ctypes.POINTER(ProcessInfo) # info_out
    ]
    mf_osinstance_process_by_name.restype = ctypes.c_int32

    # int32_t mf_osinstance_into_process_by_info(OsInstance *instance, ProcessInfo info, ProcessInstance **out);
    # ProcessInfo is passed by value here.
    mf_osinstance_into_process_by_info = memflow_lib.mf_osinstance_into_process_by_info
    mf_osinstance_into_process_by_info.argtypes = [
        OsInstance_p,
        ProcessInfo, # info (passed by value)
        ctypes.POINTER(ProcessInstance_p) # out
    ]
    mf_osinstance_into_process_by_info.restype = ctypes.c_int32

    # void mf_processinstance_drop(ProcessInstance *process);
    mf_processinstance_drop = memflow_lib.mf_processinstance_drop
    mf_processinstance_drop.argtypes = [ProcessInstance_p]
    mf_processinstance_drop.restype = None

    # const ProcessInfo *mf_processinstance_info(ProcessInstance *process);
    # Returns a const pointer to ProcessInfo. The ProcessInfo struct is owned by the ProcessInstance.
    mf_processinstance_info = memflow_lib.mf_processinstance_info
    mf_processinstance_info.argtypes = [ProcessInstance_p]
    mf_processinstance_info.restype = ctypes.POINTER(ProcessInfo) # Correct: returns a pointer to ProcessInfo

    # int32_t mf_processinstance_primary_module(ProcessInstance *process, ModuleInfo *info_out);
    mf_processinstance_primary_module = memflow_lib.mf_processinstance_primary_module
    mf_processinstance_primary_module.argtypes = [
        ProcessInstance_p,
        ctypes.POINTER(ModuleInfo) # info_out
    ]
    mf_processinstance_primary_module.restype = ctypes.c_int32

    # int32_t mf_processinstance_read_raw_into(ProcessInstance *process, Address address, CSliceMut_u8 out_buf);
    mf_processinstance_read_raw_into = memflow_lib.mf_processinstance_read_raw_into
    mf_processinstance_read_raw_into.argtypes = [
        ProcessInstance_p,
        Address, # address
        CSliceMut_u8 # out_buf
    ]
    mf_processinstance_read_raw_into.restype = ctypes.c_int32

    # int32_t mf_processinstance_write_raw(ProcessInstance *process, Address address, CSliceRef_u8 data);
    mf_processinstance_write_raw = memflow_lib.mf_processinstance_write_raw
    mf_processinstance_write_raw.argtypes = [
        ProcessInstance_p,
        Address, # address
        CSliceRef_u8 # data
    ]
    mf_processinstance_write_raw.restype = ctypes.c_int32

else: # memflow_lib is None
    print(f"WARNING: {lib_name} could not be loaded. FFI bindings are not set up. Memory operations will fail.")
    # Define dummy functions or raise errors if called to make it clear.
    def _dummy_ffi_call(*args, **kwargs):
        raise RuntimeError(f"{lib_name} not loaded. Cannot make FFI calls.")

    mf_log_init = _dummy_ffi_call
    mf_inventory_scan = _dummy_ffi_call
    mf_inventory_free = _dummy_ffi_call
    mf_inventory_create_connector = _dummy_ffi_call
    mf_connector_drop = _dummy_ffi_call
    mf_inventory_create_os = _dummy_ffi_call
    mf_os_drop = _dummy_ffi_call
    mf_osinstance_process_by_name = _dummy_ffi_call
    mf_osinstance_into_process_by_info = _dummy_ffi_call
    mf_processinstance_drop = _dummy_ffi_call
    mf_processinstance_info = _dummy_ffi_call
    mf_processinstance_primary_module = _dummy_ffi_call # Dummy for this too
    mf_processinstance_read_raw_into = _dummy_ffi_call
    mf_processinstance_write_raw = _dummy_ffi_call


if __name__ == '__main__':
    print("memflow_ffi_bindings.py - FFI Definitions")
    if memflow_lib:
        print("Memflow library loaded. FFI functions defined.")
        # Example of using a helper
        test_slice = str_to_c_slice_ref("test_string")
        print(f"Test slice: data={test_slice.data}, len={test_slice.len}")
        if test_slice.len > 0:
            # To actually read the string back from the pointer (for verification)
            # Note: This is just for testing the slice creation, not a typical operation.
            try:
                # Create a char array of the correct length from the address
                # This assumes the data pointer from c_char_p(text_bytes) is still valid.
                # It's generally safer if the C function copies the data or if buffer is managed.
                # content = ctypes.string_at(test_slice.data, test_slice.len)
                # print(f"Slice content: {content.decode('utf-8')}")
                pass # string_at might be problematic if original python bytes object for c_char_p is gone
            except Exception as e:
                print(f"Error trying to read slice content for test: {e}")

    else:
        print("Memflow library NOT loaded. FFI functions are dummies.")

```
