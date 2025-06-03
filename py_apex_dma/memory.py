import ctypes
import threading
from .structs import ProcessStatus
from . import memflow_ffi_bindings as mf # Import the FFI bindings

# Removed placeholder classes MemflowConnector, MemflowOsInstance, MemflowProcess

# This ProcessData class will now hold actual FFI ProcessInstance_p
class ProcessData:
    def __init__(self):
        self.hProcess: mf.ProcessInstance_p = None # Actual FFI process instance
        self.info: mf.ProcessInfo = None # Store ProcessInfo for baseaddr, pid, name
        self.baseaddr: mf.Address = 0
        self.pid: mf.Pid = 0
        self.name: str = ""

    def is_valid(self):
        return self.hProcess is not None and self.hProcess.value is not None

    def update_from_info(self, proc_info_ptr: ctypes.POINTER(mf.ProcessInfo)):
        if proc_info_ptr and proc_info_ptr.contents:
            self.info = proc_info_ptr.contents
            # The base address in ProcessInfo is typically 'address' field,
            # which is the address of the process object in kernel space, NOT the module base.
            # Module base address (image base) needs to be found by iterating modules or from specific OS calls.
            # For now, let's assume DTB might be related or a placeholder.
            # In memflow, primary module base is often found via `main_module_info()` on ProcessInstance
            # For now, baseaddr will be 0 until we implement module iteration.
            self.baseaddr = 0 # Placeholder - this needs to be module base
            self.pid = self.info.pid
            if self.info.name:
                self.name = self.info.name.decode('utf-8', 'ignore')
            print(f"ProcessData: Updated from ProcessInfo. PID: {self.pid}, Name: {self.name}, KernelAddr: {hex(self.info.address)}")


class Memory:
    def __init__(self):
        self._bindings = mf # Store the bindings module
        self._proc = ProcessData()
        self._status = ProcessStatus.NOT_FOUND
        self._lock = threading.Lock() # Thread safety for operations

        self._inventory: mf.Inventory_p = None
        self._connector: mf.ConnectorInstance_p = None
        self._os_instance: mf.OsInstance_p = None

        try:
            if self._bindings.memflow_lib: # Check if library was loaded
                self._bindings.mf_log_init(self._bindings.LevelFilter_Info) # Example log level
                self._inventory = self._bindings.mf_inventory_scan()
                if not self._inventory:
                    print("Memory Error: Failed to scan memflow inventory.")
                    # Potentially raise an error or set a permanent error state
                else:
                    print("Memory: Memflow inventory scanned successfully.")
            else:
                print("Memory Error: Memflow library not loaded. Memory operations will fail.")
        except Exception as e:
            print(f"Memory Error: Exception during memflow initialization: {e}")
            self._inventory = None # Ensure inventory is None on error

    def __del__(self):
        self.close_proc() # Frees connector, OS, process instances
        if self._inventory and self._bindings.memflow_lib:
            print("Memory: Releasing memflow inventory.")
            self._bindings.mf_inventory_free(self._inventory)
            self._inventory = None

    def get_proc_baseaddr(self) -> int: # Return type should match Address (Python int)
        with self._lock:
            return self._proc.baseaddr if self._proc.is_valid() else 0

    def get_proc_pid(self) -> int:
        with self._lock:
            return self._proc.pid if self._proc.is_valid() else 0

    def get_proc_name(self) -> str:
        with self._lock:
            return self._proc.name if self._proc.is_valid() else ""

    def get_proc_status(self) -> ProcessStatus:
        with self._lock:
            return self._status

    def check_proc(self):
        with self._lock:
            if not self._proc.is_valid():
                self._status = ProcessStatus.NOT_FOUND
                self._proc.baseaddr = 0
                self.pid = 0
                self.name = ""
                # TODO: Add logic to actually check if process is still running
                # This might involve an FFI call to memflow to check process state by PID/name
                print(f"Memory.check_proc: Process not valid or not found (placeholder check)")
            else:
                # print(f"Memory.check_proc: Process {self._proc.name} still considered valid (placeholder check)")
                pass


    def open_proc(self, name: str) -> bool:
        with self._lock:
            if self._proc.is_valid() and self._proc.name == name: # TODO: Check if name matches current process name
                # print(f"Memory.open_proc: Process {name} already open and valid.")
                return True # Already open to the correct process

            self.close_proc() # Close any existing process first

            if not self._inventory or not self._bindings.memflow_lib:
                print("Memory.open_proc: Memflow inventory not available or library not loaded.")
                self._status = ProcessStatus.NOT_FOUND
                return False

            print(f"Memory.open_proc: Attempting to open process '{name}' via memflow...")

            # 1. Create Connector (e.g., "kvm", "qemu_procfs", "pcileech")
            #    These should ideally come from configuration or auto-detection.
            #    For now, let's try a common one like "kvm" or "qemu_procfs" if on Linux,
            #    or allow it to be specified. Using a placeholder name for now.
            #    A real implementation would likely try a list of known connectors.
            connector_name = "kvm" # Example: This should be configurable
            connector_args_c = None # No specific args for kvm by default, pass NULL

            # If using Physical Memory connector like "pcileech", args might be needed.
            # connector_name = "pcileech"
            # connector_args = "fpga" # Example argument
            # connector_args_c = connector_args.encode('utf-8')


            connector_out = mf.ConnectorInstance_p()
            ret = self._bindings.mf_inventory_create_connector(
                self._inventory,
                connector_name.encode('utf-8'),
                connector_args_c, # Pass None or actual args
                ctypes.byref(connector_out)
            )

            if ret != 0 or not connector_out: # 0 is success for most memflow C API calls
                print(f"Memory.open_proc: Failed to create connector '{connector_name}'. Status: {ret}")
                self._status = ProcessStatus.NOT_FOUND
                return False
            self._connector = connector_out
            print(f"Memory.open_proc: Connector '{connector_name}' created successfully.")

            # 2. Create OS Layer (e.g., "win32", "linux")
            os_name = "win32" # Example: This should be auto-detected or configurable
            os_args_c = None    # No specific args usually

            os_out = mf.OsInstance_p()
            ret = self._bindings.mf_inventory_create_os(
                self._inventory,
                os_name.encode('utf-8'),
                os_args_c,
                self._connector, # Pass the created connector
                ctypes.byref(os_out)
            )

            if ret != 0 or not os_out:
                print(f"Memory.open_proc: Failed to create OS instance '{os_name}'. Status: {ret}")
                self.close_proc() # Clean up connector
                return False
            self._os_instance = os_out
            print(f"Memory.open_proc: OS instance '{os_name}' created successfully.")

            # 3. Get Process by name
            process_info_out = mf.ProcessInfo()
            name_slice = self._bindings.str_to_c_slice_ref(name)

            ret = self._bindings.mf_osinstance_process_by_name(
                self._os_instance,
                name_slice, # Process name as CSliceRef_u8
                ctypes.byref(process_info_out)
            )

            if ret != 0:
                print(f"Memory.open_proc: Failed to find process by name '{name}'. Status: {ret}")
                self.close_proc() # Clean up os and connector
                return False

            # 4. Attach to the process (get a ProcessInstance)
            process_out = mf.ProcessInstance_p()
            ret = self._bindings.mf_osinstance_into_process_by_info(
                self._os_instance,
                process_info_out, # Pass the ProcessInfo struct by value
                ctypes.byref(process_out)
            )

            if ret != 0 or not process_out:
                print(f"Memory.open_proc: Failed to attach to process '{name}'. Status: {ret}")
                self.close_proc()
                return False

            self._proc.hProcess = process_out

            # Get full ProcessInfo from the ProcessInstance (might contain more details like proper base)
            # The ProcessInfo from by_name might be minimal.
            # The one from ProcessInstance is usually more complete for the attached process.
            final_proc_info_ptr = self._bindings.mf_processinstance_info(self._proc.hProcess)
            if final_proc_info_ptr:
                self._proc.update_from_info(final_proc_info_ptr)
                # IMPORTANT: The base address for reads/writes is usually the module base of r5apex.exe,
                # not process_info_out.address (which is kernel virtual address of process object).
                # We need to implement module iteration to find the actual base address.
                # For now, self._proc.baseaddr will be 0 or incorrect.
                # This needs to be fixed for Read/Write to work correctly.
                # TODO: Implement module iteration to find main module base address.
                # For now, we'll assume for testing that reads might be from absolute addresses if known,
                # or that a temporary fixed base is used. This is a CRITICAL point.

                # Attempt to get the primary module base address
                primary_module_info = mf.ModuleInfo()
                ret_mod = self._bindings.mf_processinstance_primary_module(
                    self._proc.hProcess,
                    ctypes.byref(primary_module_info)
                )
                if ret_mod == 0 and primary_module_info.base != 0:
                    self._proc.baseaddr = primary_module_info.base
                    print(f"Memory.open_proc: Primary module for '{self._proc.name}' found. Base: {hex(self._proc.baseaddr)}, Size: {hex(primary_module_info.size)}")
                    if primary_module_info.name:
                         print(f"Memory.open_proc: Primary module name: {primary_module_info.name.decode('utf-8', 'ignore')}")
                else:
                    print(f"WARNING: Failed to get primary module info for '{self._proc.name}'. Status: {ret_mod}. Reads relative to base will fail.")
                    self._proc.baseaddr = mf.Address(0) # Ensure it's 0 if not found

                self._status = ProcessStatus.FOUND_READY
                print(f"Memory.open_proc: Successfully attached to process '{self._proc.name}', PID: {self._proc.pid}, Kernel Addr: {hex(self.info.address if self.info else 0)}") # Access self.info after ensuring it's set
                print(f"Memory.open_proc: Process Base (Primary Module) Address set to: {hex(self._proc.baseaddr)}")
                return True
            else: # Failed to get final_proc_info_ptr
                print(f"Memory.open_proc: Failed to get ProcessInfo from process instance for '{name}'.")
                self.close_proc()
                return False


    def close_proc(self):
        with self._lock:
            if self._proc.hProcess and self._bindings.memflow_lib:
                print(f"Memory.close_proc: Releasing process instance for '{self._proc.name}'")
                self._bindings.mf_processinstance_drop(self._proc.hProcess)
            self._proc = ProcessData() # Reset process data

            if self._os_instance and self._bindings.memflow_lib:
                print("Memory.close_proc: Releasing OS instance.")
                self._bindings.mf_os_drop(self._os_instance)
                self._os_instance = None

            if self._connector and self._bindings.memflow_lib:
                print("Memory.close_proc: Releasing Connector instance.")
                self._bindings.mf_connector_drop(self._connector)
                self._connector = None

            self._status = ProcessStatus.NOT_FOUND
            # print("Memory.close_proc: Process resources released.")


    def Read(self, address: int, ctype_obj) -> bool:
        '''
        Reads data from memory into a provided ctypes object.
        address: The absolute memory address to read from.
        ctype_obj: An instance of a ctypes.Structure or a ctypes basic type (e.g., ctypes.c_uint64()).
                   This object will be updated in place.
        '''
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib:
                # print(f"Memory.Read: Process not open/invalid or memflow not loaded. Addr: {hex(address)}")
                return False

            size = ctypes.sizeof(ctype_obj)
            # Create a mutable buffer of bytes
            buffer = (ctypes.c_uint8 * size)()

            # Create CSliceMut_u8 for the FFI call
            # The data pointer of CSliceMut_u8 should point to our Python-managed buffer
            c_slice = mf.CSliceMut_u8(data=ctypes.cast(buffer, ctypes.POINTER(ctypes.c_uint8)), len=size)

            # Addresses for memflow are absolute. If self._proc.baseaddr is used, ensure 'address' is an offset.
            # For now, 'address' is assumed absolute.
            # To read relative to module base: read_addr = self._proc.baseaddr + address
            read_addr = mf.Address(address)

            result = self._bindings.mf_processinstance_read_raw_into(self._proc.hProcess, read_addr, c_slice)

            if result == 0: # Success
                # Copy data from our temporary buffer to the user-provided ctype_obj
                ctypes.memmove(ctypes.byref(ctype_obj), buffer, size)
                return True
            else:
                # print(f"Memory.Read: Failed to read {size} bytes from {hex(address)}. Error code: {result}")
                return False

    def ReadArray(self, address: int, ctype_array) -> bool:
        '''
        Reads data from memory into a provided ctypes array.
        address: The absolute memory address to read from.
        ctype_array: An instance of a ctypes array (e.g., (ctypes.c_uint8 * 10)()).
                     This array will be updated in place.
        '''
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib:
                return False

            size = ctypes.sizeof(ctype_array)
            # Ctype arrays can be directly cast to POINTER(c_uint8) for CSliceMut_u8
            # No intermediate buffer needed as ctype_array is already a contiguous memory block.
            c_slice = mf.CSliceMut_u8(data=ctypes.cast(ctype_array, ctypes.POINTER(ctypes.c_uint8)), len=size)

            read_addr = mf.Address(address)
            result = self._bindings.mf_processinstance_read_raw_into(self._proc.hProcess, read_addr, c_slice)

            if result == 0:
                return True
            else:
                # print(f"Memory.ReadArray: Failed to read array of {size} bytes from {hex(address)}. Error: {result}")
                return False

    def Write(self, address: int, ctype_obj) -> bool:
        '''
        Writes data from a ctypes object to memory.
        address: The absolute memory address to write to.
        ctype_obj: An instance of a ctypes.Structure or a ctypes basic type containing the data to write.
        '''
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib:
                return False

            size = ctypes.sizeof(ctype_obj)
            # For writing, the data comes directly from the ctype_obj.
            # We need a pointer to the object's data for CSliceRef_u8.
            data_ptr = ctypes.cast(ctypes.byref(ctype_obj), ctypes.POINTER(ctypes.c_uint8))
            c_slice = mf.CSliceRef_u8(data=data_ptr, len=size)

            write_addr = mf.Address(address)
            result = self._bindings.mf_processinstance_write_raw(self._proc.hProcess, write_addr, c_slice)

            if result == 0:
                return True
            else:
                # print(f"Memory.Write: Failed to write {size} bytes to {hex(address)}. Error: {result}")
                return False

    def WriteArray(self, address: int, ctype_array) -> bool:
        '''
        Writes data from a ctypes array to memory.
        address: The absolute memory address to write to.
        ctype_array: An instance of a ctypes array containing the data to write.
        '''
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib:
                return False

            size = ctypes.sizeof(ctype_array)
            # Ctype arrays can be directly cast for the data pointer.
            data_ptr = ctypes.cast(ctype_array, ctypes.POINTER(ctypes.c_uint8))
            c_slice = mf.CSliceRef_u8(data=data_ptr, len=size)

            write_addr = mf.Address(address)
            result = self._bindings.mf_processinstance_write_raw(self._proc.hProcess, write_addr, c_slice)

            if result == 0:
                return True
            else:
                # print(f"Memory.WriteArray: Failed to write array of {size} bytes to {hex(address)}. Error: {result}")
                return False

    def ScanPointer(self, ptr_address: int, offsets: list[int]) -> int:
        '''
        Dereferences a multi-level pointer.
        ptr_address: The initial base address (often a static pointer).
        offsets: A list of offsets to add at each level of dereferencing.
        '''
        with self._lock:
            if not self._proc.is_valid():
                return 0

            current_address = ptr_address
            # print(f"ScanPointer: Base {hex(current_address)}, Offsets {offsets}")

            temp_ptr = ctypes.c_uint64() # Assuming pointers are 64-bit

            for i, offset in enumerate(offsets):
                if i == 0: # First read is from the initial ptr_address
                    if not self.Read(current_address, temp_ptr):
                        # print(f"ScanPointer: Failed to read initial pointer at {hex(current_address)}")
                        return 0
                # For subsequent levels, current_address already holds the value read from previous level

                # print(f"ScanPointer: Level {i}, Value before offset: {hex(temp_ptr.value)}")
                current_address = temp_ptr.value + offset
                # print(f"ScanPointer: Level {i}, Address to read for next level: {hex(current_address)}")

                # If it's not the last offset, read the next pointer value
                if i < len(offsets) - 1:
                    if not self.Read(current_address, temp_ptr):
                        # print(f"ScanPointer: Failed to read pointer at level {i+1} from {hex(current_address)}")
                        return 0
                # Else, current_address is the final value (address after last offset)

            # print(f"ScanPointer: Final address {hex(current_address)}")
            return current_address

    # These methods are more complex and specific to memflow's internals or advanced features.
    # They might require deeper integration with memflow FFI calls beyond simple read/write.
    # For now, they will be placeholders.
    def bruteforceDtb(self, dtb_start_physical_addr: int, step_page: int) -> bool:
        print(f"Memory.bruteforceDtb: Called with start_addr={hex(dtb_start_physical_addr)}, step={hex(step_page)} (Not Implemented)")
        # This would involve complex FFI calls to memflow's DTB bruteforcing capabilities
        return False

    def testDtbValue(self, dtb_val: int) -> bool:
        print(f"Memory.testDtbValue: Called with dtb_val={hex(dtb_val)} (Not Implemented)")
        # This would involve FFI calls to test a DTB value
        return False

if __name__ == '__main__':
    # Example Usage (Placeholders will run)
    mem = Memory()
    proc_name = "notepad.exe" # Example

    if mem.open_proc(proc_name):
        print(f"Successfully attached to {mem.get_proc_name()} (PID: {mem.get_proc_pid()})")
        base_addr = mem.get_proc_baseaddr()
        print(f"Base Address: {hex(base_addr)}")

        # Example Read (reading a uint64_t from base address)
        some_data = ctypes.c_uint64()
        if mem.Read(base_addr, some_data):
            print(f"Read from base address: {hex(some_data.value)}")
        else:
            print(f"Failed to read from base address.")

        # Example Write (writing a uint32_t)
        data_to_write = ctypes.c_uint32(0xDEADBEEF)
        write_addr = base_addr + 0x1000 # Dummy address
        if mem.Write(write_addr, data_to_write):
            print(f"Wrote {hex(data_to_write.value)} to {hex(write_addr)}")
            # Verify by reading back
            read_back_data = ctypes.c_uint32()
            if mem.Read(write_addr, read_back_data):
                print(f"Read back: {hex(read_back_data.value)}")
            else:
                print(f"Failed to read back for verification.")

        else:
            print(f"Failed to write to {hex(write_addr)}")

        # Example ScanPointer
        # This requires knowing a valid multi-level pointer structure in the target process
        # For demonstration, let's assume a hypothetical structure:
        # StaticPtrToStruct -> StructBaseAddr
        # StructBaseAddr + 0x10 -> PtrToValue
        # PtrToValue + 0x8 -> ActualValueAddress

        # These would be actual addresses from the game/application
        # For now, let's use dummy addresses that might be valid within the placeholder's range
        # static_ptr_addr = base_addr + 0x1A2B3C # Dummy static pointer address
        # offsets = [0x10, 0x8]

        # To make ScanPointer testable with placeholders, we need to ensure writes happen first
        # or that the placeholder Read can provide consistent dummy values.
        # The current placeholder Read doesn't fill data, so ScanPointer will likely fail.
        # To test ScanPointer properly, one would need a more interactive placeholder or real FFI.

        mem.close_proc()
    else:
        print(f"Failed to open {proc_name}")
