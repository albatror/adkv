import ctypes
import threading
import logging # Added logging
from .structs import ProcessStatus
from . import memflow_ffi_bindings as mf # Import the FFI bindings

# Basic logger setup for this module
logger = logging.getLogger(__name__)
if not logger.hasHandlers(): # Avoid adding multiple handlers if reloaded
    handler = logging.StreamHandler()
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.setLevel(logging.INFO) # Set default level (e.g., INFO, DEBUG)


class ProcessData:
    def __init__(self):
        self.hProcess: mf.ProcessInstance_p = None
        self.info: mf.ProcessInfo = None
        self.baseaddr: mf.Address = mf.Address(0) # Initialize with c_uint64(0)
        self.pid: mf.Pid = mf.Pid(0)
        self.name: str = ""

    def is_valid(self):
        return self.hProcess is not None and self.hProcess.value is not None

    def update_from_info(self, proc_info_ptr: ctypes.POINTER(mf.ProcessInfo)):
        if proc_info_ptr and proc_info_ptr.contents:
            self.info = proc_info_ptr.contents
            self.baseaddr = mf.Address(0) # Will be updated by primary_module call
            self.pid = self.info.pid
            if self.info.name:
                try:
                    self.name = self.info.name.decode('utf-8', 'ignore')
                except Exception: # Should not happen if name is valid C string from memflow
                    self.name = "DecodeError"
            logger.info(f"ProcessData updated: PID: {self.pid}, Name: {self.name}, KernelAddr: {hex(self.info.address)}")


class Memory:
    def __init__(self):
        self._bindings = mf
        self._proc = ProcessData()
        self._status = ProcessStatus.NOT_FOUND
        self._lock = threading.Lock()

        self._inventory: mf.Inventory_p = None
        self._connector: mf.ConnectorInstance_p = None
        self._os_instance: mf.OsInstance_p = None

        try:
            if self._bindings.memflow_lib:
                self._bindings.mf_log_init(self._bindings.LevelFilter_Info)
                self._inventory = self._bindings.mf_inventory_scan()
                if not self._inventory:
                    logger.error("Failed to scan memflow inventory.")
                else:
                    logger.info("Memflow inventory scanned successfully.")
            else:
                logger.critical("Memflow library not loaded. Memory operations will fail.")
        except Exception as e:
            logger.exception(f"Exception during memflow initialization: {e}")
            self._inventory = None

    def __del__(self):
        logger.debug("Memory object destructor called.")
        self.close_proc()
        if self._inventory and self._bindings.memflow_lib and self._bindings.mf_inventory_free:
            logger.info("Releasing memflow inventory.")
            try:
                self._bindings.mf_inventory_free(self._inventory)
            except Exception as e:
                logger.exception(f"Error releasing memflow inventory: {e}")
            self._inventory = None

    def get_proc_baseaddr(self) -> int:
        with self._lock:
            return self._proc.baseaddr.value if self._proc.is_valid() else 0

    def get_proc_pid(self) -> int:
        with self._lock:
            return self._proc.pid.value if self._proc.is_valid() else 0 # Access .value for ctypes

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
                # self._proc fields already reset by close_proc or init
                # logger.info(f"Memory.check_proc: Process not valid or not found.") # Can be noisy
            else:
                # TODO: Add a lightweight FFI call if available to check if process is still running,
                # e.g., by re-reading some basic info or checking its state.
                # For now, assume valid if hProcess is set.
                pass

    def open_proc(self, name: str) -> bool:
        with self._lock:
            if self._proc.is_valid() and self._proc.name == name:
                logger.debug(f"Process {name} already open and valid.")
                return True

            self.close_proc()

            if not self._inventory or not self._bindings.memflow_lib:
                logger.error("Memflow inventory not available or library not loaded.")
                self._status = ProcessStatus.NOT_FOUND
                return False

            logger.info(f"Attempting to open process '{name}' via memflow...")

            connector_name = "kvm"
            connector_args_c = None
            connector_out = mf.ConnectorInstance_p()
            ret = self._bindings.mf_inventory_create_connector(
                self._inventory, connector_name.encode('utf-8'),
                connector_args_c, ctypes.byref(connector_out)
            )
            if ret != mf.MF_SUCCESS or not connector_out:
                logger.error(f"Failed to create connector '{connector_name}'. Status: {ret}")
                self._status = ProcessStatus.NOT_FOUND; self.close_proc(); return False
            self._connector = connector_out
            logger.info(f"Connector '{connector_name}' created successfully.")

            os_name = "win32"
            os_args_c = None
            os_out = mf.OsInstance_p()
            ret = self._bindings.mf_inventory_create_os(
                self._inventory, os_name.encode('utf-8'),
                os_args_c, self._connector, ctypes.byref(os_out)
            )
            if ret != mf.MF_SUCCESS or not os_out:
                logger.error(f"Failed to create OS instance '{os_name}'. Status: {ret}")
                self._status = ProcessStatus.NOT_FOUND; self.close_proc(); return False
            self._os_instance = os_out
            logger.info(f"OS instance '{os_name}' created successfully.")

            process_info_out = mf.ProcessInfo()
            name_slice = self._bindings.str_to_c_slice_ref(name)
            ret = self._bindings.mf_osinstance_process_by_name(
                self._os_instance, name_slice, ctypes.byref(process_info_out)
            )
            if ret != mf.MF_SUCCESS:
                logger.error(f"Failed to find process by name '{name}'. Status: {ret}")
                self._status = ProcessStatus.NOT_FOUND; self.close_proc(); return False

            process_out = mf.ProcessInstance_p()
            ret = self._bindings.mf_osinstance_into_process_by_info(
                self._os_instance, process_info_out, ctypes.byref(process_out)
            )
            if ret != mf.MF_SUCCESS or not process_out:
                logger.error(f"Failed to attach to process '{name}'. Status: {ret}")
                self._status = ProcessStatus.NOT_FOUND; self.close_proc(); return False
            self._proc.hProcess = process_out

            final_proc_info_ptr = self._bindings.mf_processinstance_info(self._proc.hProcess)
            if final_proc_info_ptr:
                self._proc.update_from_info(final_proc_info_ptr)

                primary_module_info = mf.ModuleInfo()
                ret_mod = self._bindings.mf_processinstance_primary_module(
                    self._proc.hProcess, ctypes.byref(primary_module_info)
                )
                if ret_mod == mf.MF_SUCCESS and primary_module_info.base != 0:
                    self._proc.baseaddr = primary_module_info.base
                    logger.info(f"Primary module for '{self._proc.name}' found. Base: {hex(self._proc.baseaddr.value)}, Size: {hex(primary_module_info.size.value)}")
                    if primary_module_info.name:
                         logger.info(f"Primary module name: {primary_module_info.name.decode('utf-8', 'ignore')}")
                else:
                    logger.warning(f"Failed to get primary module info for '{self._proc.name}'. Status: {ret_mod}. Reads relative to base may fail.")
                    self._proc.baseaddr = mf.Address(0)

                self._status = ProcessStatus.FOUND_READY
                logger.info(f"Successfully attached to process '{self._proc.name}', PID: {self._proc.pid.value}, Kernel Addr: {hex(self._proc.info.address.value if self._proc.info else 0)}")
                logger.info(f"Process Base (Primary Module) Address set to: {hex(self._proc.baseaddr.value)}")
                return True
            else:
                logger.error(f"Failed to get ProcessInfo from process instance for '{name}'.")
                self._status = ProcessStatus.NOT_FOUND; self.close_proc(); return False

    def close_proc(self):
        with self._lock:
            if self._proc.hProcess and self._bindings.memflow_lib and self._bindings.mf_processinstance_drop:
                logger.info(f"Releasing process instance for '{self._proc.name}'")
                try: self._bindings.mf_processinstance_drop(self._proc.hProcess)
                except Exception as e: logger.exception(f"Error dropping process instance: {e}")
            self._proc = ProcessData()

            if self._os_instance and self._bindings.memflow_lib and self._bindings.mf_os_drop:
                logger.info("Releasing OS instance.")
                try: self._bindings.mf_os_drop(self._os_instance)
                except Exception as e: logger.exception(f"Error dropping OS instance: {e}")
                self._os_instance = None

            if self._connector and self._bindings.memflow_lib and self._bindings.mf_connector_drop:
                logger.info("Releasing Connector instance.")
                try: self._bindings.mf_connector_drop(self._connector)
                except Exception as e: logger.exception(f"Error dropping connector instance: {e}")
                self._connector = None

            self._status = ProcessStatus.NOT_FOUND

    def Read(self, address: int, ctype_obj) -> bool:
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib:
                logger.debug(f"Memory.Read: Process not open/invalid or memflow not loaded. Addr: {hex(address)}")
                return False
            size = ctypes.sizeof(ctype_obj)
            buffer = (ctypes.c_uint8 * size)()
            c_slice = mf.CSliceMut_u8(data=ctypes.cast(buffer, ctypes.POINTER(ctypes.c_uint8)), len=size)
            read_addr = mf.Address(address) # Assuming address is absolute for now
            result = self._bindings.mf_processinstance_read_raw_into(self._proc.hProcess, read_addr, c_slice)
            if result == mf.MF_SUCCESS:
                ctypes.memmove(ctypes.byref(ctype_obj), buffer, size)
                return True
            else:
                logger.warning(f"Memory.Read: Failed to read {size} bytes from {hex(address)}. Error code: {result}")
                return False

    def ReadArray(self, address: int, ctype_array) -> bool:
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib: return False
            size = ctypes.sizeof(ctype_array)
            c_slice = mf.CSliceMut_u8(data=ctypes.cast(ctype_array, ctypes.POINTER(ctypes.c_uint8)), len=size)
            read_addr = mf.Address(address)
            result = self._bindings.mf_processinstance_read_raw_into(self._proc.hProcess, read_addr, c_slice)
            if result == mf.MF_SUCCESS: return True
            else:
                logger.warning(f"Memory.ReadArray: Failed to read array of {size} bytes from {hex(address)}. Error: {result}")
                return False

    def Write(self, address: int, ctype_obj) -> bool:
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib: return False
            size = ctypes.sizeof(ctype_obj)
            data_ptr = ctypes.cast(ctypes.byref(ctype_obj), ctypes.POINTER(ctypes.c_uint8))
            c_slice = mf.CSliceRef_u8(data=data_ptr, len=size)
            write_addr = mf.Address(address)
            result = self._bindings.mf_processinstance_write_raw(self._proc.hProcess, write_addr, c_slice)
            if result == mf.MF_SUCCESS: return True
            else:
                logger.warning(f"Memory.Write: Failed to write {size} bytes to {hex(address)}. Error: {result}")
                return False

    def WriteArray(self, address: int, ctype_array) -> bool:
        with self._lock:
            if not self._proc.is_valid() or not self._bindings.memflow_lib: return False
            size = ctypes.sizeof(ctype_array)
            data_ptr = ctypes.cast(ctype_array, ctypes.POINTER(ctypes.c_uint8))
            c_slice = mf.CSliceRef_u8(data=data_ptr, len=size)
            write_addr = mf.Address(address)
            result = self._bindings.mf_processinstance_write_raw(self._proc.hProcess, write_addr, c_slice)
            if result == mf.MF_SUCCESS: return True
            else:
                logger.warning(f"Memory.WriteArray: Failed to write array of {size} bytes to {hex(address)}. Error: {result}")
                return False

    def ScanPointer(self, ptr_address: int, offsets: list[int]) -> int:
        with self._lock:
            if not self._proc.is_valid(): return 0
            current_address = ptr_address
            temp_ptr = ctypes.c_uint64()
            for i, offset in enumerate(offsets):
                if i == 0:
                    if not self.Read(current_address, temp_ptr): return 0
                current_address = temp_ptr.value + offset
                if i < len(offsets) - 1:
                    if not self.Read(current_address, temp_ptr): return 0
            return current_address

    def bruteforceDtb(self, dtb_start_physical_addr: int, step_page: int) -> bool:
        logger.info(f"Memory.bruteforceDtb: Called with start_addr={hex(dtb_start_physical_addr)}, step={hex(step_page)} (Not Implemented)")
        return False

    def testDtbValue(self, dtb_val: int) -> bool:
        logger.info(f"Memory.testDtbValue: Called with dtb_val={hex(dtb_val)} (Not Implemented)")
        return False

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG) # Enable debug logging for standalone test
    logger.info("Memory module standalone test.")
    mem = Memory()
    proc_name = "notepad.exe"
    # proc_name = "calc.exe" # Try another common process
    # proc_name = "r5apex_dx12.exe" # For actual game testing

    if mem.open_proc(proc_name):
        logger.info(f"Successfully attached to {mem.get_proc_name()} (PID: {mem.get_proc_pid()})")
        base_addr = mem.get_proc_baseaddr()
        logger.info(f"Base Address (from primary module): {hex(base_addr)}")

        # Example: Read a known structure or value if possible, e.g., PE header if base_addr is image base
        if base_addr != 0: # Only if base_addr was successfully found
            dos_header_magic = ctypes.c_uint16()
            if mem.Read(base_addr, dos_header_magic): # Read first 2 bytes (MZ)
                logger.info(f"Read from base address + 0x0 (DOS Header Magic): {hex(dos_header_magic.value)}")
                if dos_header_magic.value == 0x5a4d: # 'MZ'
                    logger.info("PE Header 'MZ' signature confirmed!")
                else:
                    logger.warning("PE Header 'MZ' signature NOT found. Base address might not be image base or read failed.")
            else:
                logger.error(f"Failed to read from base address {hex(base_addr)}.")
        else:
            logger.warning("Base address is 0, cannot perform relative reads for PE header test.")

        # Test ScanPointer (if you know a multi-level pointer in the target)
        # For notepad, it's hard to give a generic example.
        # This would be highly specific to the process and OS version.
        # Example: known_static_ptr_in_notepad = 0x...
        # offsets_to_value = [0x10, 0x20, 0x8]
        # final_value_addr = mem.ScanPointer(known_static_ptr_in_notepad, offsets_to_value)
        # if final_value_addr != 0:
        #     logger.info(f"ScanPointer result: {hex(final_value_addr)}")
        #     some_val = ctypes.c_int()
        #     if mem.Read(final_value_addr, some_val):
        #         logger.info(f"Value at pointer scan result: {some_val.value}")

        mem.close_proc()
    else:
        logger.error(f"Failed to open {proc_name}")

    logger.info("Memory module test finished.")

```
