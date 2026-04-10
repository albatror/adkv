use memflow::prelude::v1::*;

pub struct Memory {
    pub os: Option<OsInstanceArcBox<'static>>,
    pub process: Option<IntoProcessInstanceArcBox<'static>>,
    pub base_addr: u64,
}

impl Memory {
    pub fn new() -> Self {
        Self {
            os: None,
            process: None,
            base_addr: 0,
        }
    }

    pub fn open_process(&mut self, name: &str) -> anyhow::Result<()> {
        let mut inventory = Inventory::scan();

        if self.os.is_none() {
            let connector = inventory.instantiate_connector("kvm", None, None)
                .or_else(|_| inventory.instantiate_connector("qemu", None, None))?;

            let os = inventory.instantiate_os("win32", Some(connector), None)?;
            self.os = Some(os);
        }

        let os = self.os.as_mut().unwrap();
        let process_info = os.process_info_by_name(name)?;

        let mut process = os.clone().into_process_by_info(process_info)?;

        let module_info = process.module_by_name(name)?;
        self.base_addr = module_info.base.to_umem();
        self.process = Some(process);

        Ok(())
    }

    pub fn read<T: Pod>(&mut self, addr: u64) -> memflow::error::Result<T> {
        if let Some(proc) = &mut self.process {
            proc.read(Address::from(addr)).map_err(|e| e.into())
        } else {
            Err(memflow::error::Error(memflow::error::ErrorOrigin::Other, memflow::error::ErrorKind::Uninitialized))
        }
    }

    pub fn read_array<T: Pod + Clone>(&mut self, addr: u64, len: usize) -> memflow::error::Result<Vec<T>> {
        if let Some(proc) = &mut self.process {
            let mut buf = vec![unsafe { std::mem::zeroed() }; len];
            proc.read_into(Address::from(addr), &mut buf[..]).map_err(|e| memflow::error::Error::from(e))?;
            Ok(buf)
        } else {
            Err(memflow::error::Error(memflow::error::ErrorOrigin::Other, memflow::error::ErrorKind::Uninitialized))
        }
    }

    pub fn write<T: Pod>(&mut self, addr: u64, value: &T) -> memflow::error::Result<()> {
        if let Some(proc) = &mut self.process {
            proc.write(Address::from(addr), value).map_err(|e| e.into())
        } else {
            Err(memflow::error::Error(memflow::error::ErrorOrigin::Other, memflow::error::ErrorKind::Uninitialized))
        }
    }
}
