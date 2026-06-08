use memflow::prelude::v1::*;
use anyhow::Result;

pub struct Memory {
    pub os: OsInstanceArcBox<'static>,
    pub process: IntoProcessInstanceArcBox<'static>,
    pub base_addr: Address,
}

impl Memory {
    pub fn new(connector: &str, proc_name: &str) -> Result<Self> {
        let inventory = Inventory::scan();
        let connector = inventory.create_connector(connector, None, None)?;
        let mut os = inventory.create_os("win32", Some(connector), None)?;

        let process_info = os.process_info_by_name(proc_name)
            .map_err(|_| anyhow::anyhow!("Process {} not found", proc_name))?;

        let mut process = os.clone().into_process_by_info(process_info)?;

        let module_info = process.module_by_name(proc_name)
            .map_err(|_| anyhow::anyhow!("Module {} not found", proc_name))?;

        Ok(Self {
            os,
            process,
            base_addr: module_info.base,
        })
    }

    pub fn read<T: Pod>(&mut self, addr: u64) -> Result<T> {
        self.process.read::<T>(addr.into()).map_err(|e| anyhow::anyhow!("Read error: {}", e))
    }

    pub fn write<T: Pod>(&mut self, addr: u64, val: T) -> Result<()> {
        self.process.write::<T>(addr.into(), &val).map_err(|e| anyhow::anyhow!("Write error: {}", e))
    }

    pub fn read_array<T: Pod + Clone>(&mut self, addr: u64, len: usize) -> Result<Vec<T>> {
        let mut buf = vec![unsafe { std::mem::zeroed() }; len];
        self.process.read_into(addr.into(), buf.as_mut_slice()).map_err(|e| anyhow::anyhow!("Read error: {}", e))?;
        Ok(buf)
    }
}
