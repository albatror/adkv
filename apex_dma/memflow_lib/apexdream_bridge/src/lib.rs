use apexdream::Interface;
use std::time::Instant;
use std::ffi::CStr;
use libc::{c_char, c_void};

pub type ReadMemFn = unsafe extern "C" fn(ctx: *mut c_void, addr: u64, buf: *mut u8, len: usize) -> i32;
pub type WriteMemFn = unsafe extern "C" fn(ctx: *mut c_void, addr: u64, buf: *const u8, len: usize) -> i32;
pub type GetScreenSizeFn = unsafe extern "C" fn(ctx: *mut c_void, width: *mut i32, height: *mut i32);

pub struct CppInterface {
    pub ctx: *mut c_void,
    pub read_fn: ReadMemFn,
    pub write_fn: WriteMemFn,
    pub get_screen_size_fn: GetScreenSizeFn,
    pub start_time: Instant,
    pub base_address: u64,
}

impl Interface for CppInterface {
    fn get_time(&mut self) -> f64 {
        self.start_time.elapsed().as_secs_f64()
    }

    fn sleep(&mut self, ms: u32) {
        std::thread::sleep(std::time::Duration::from_millis(ms as u64));
    }

    fn log(&mut self, args: std::fmt::Arguments) {
        println!("{}", args);
    }

    fn visualize(&mut self, _scope: &str, _args: std::fmt::Arguments) {
        // Not implemented
    }

    fn dump_bin(&mut self, path: &str, data: &[u8]) {
        let _ = std::fs::write(path, data);
    }

    fn mouse_move(&mut self, _dx: i32, _dy: i32) {
        // Not implemented (DMA host)
    }

    fn base_address(&mut self) -> u64 {
        self.base_address
    }

    fn read_memory(&mut self, address: u64, dest: &mut [u8]) -> i32 {
        unsafe { (self.read_fn)(self.ctx, address, dest.as_mut_ptr(), dest.len()) }
    }

    fn gather_memory(&mut self, address: u64, _size: u32, indices: &mut [u32]) -> i32 {
        for index in indices.iter_mut() {
            let mut buf = [0u8; 4];
            if self.read_memory(address + *index as u64, &mut buf) == 0 {
                *index = u32::from_ne_bytes(buf);
            } else {
                *index = 0;
            }
        }
        0
    }

    fn write_memory(&mut self, address: u64, src: &[u8]) -> i32 {
        unsafe { (self.write_fn)(self.ctx, address, src.as_ptr(), src.len()) }
    }

    fn r_begin(&mut self, screen: &mut [i32; 2]) -> bool {
        unsafe { (self.get_screen_size_fn)(self.ctx, &mut screen[0], &mut screen[1]) };
        false // Return false to skip drawing for now as we don't have a renderer bridge
    }

    fn r_rect(&mut self, _x: f32, _y: f32, _width: f32, _height: f32, _fill: u32, _stroke: u32) {}
    fn r_ellipse(&mut self, _x: f32, _y: f32, _width: f32, _height: f32, _fill: u32, _stroke: u32) {}
    fn r_text(&mut self, _font: u32, _flags: u32, _x: f32, _y: f32, _width: f32, _height: f32, _color: u32, _color2: u32, _text: &str) {}
    fn r_line(&mut self, _color: u32, _x1: f32, _y1: f32, _x2: f32, _y2: f32) {}
    fn r_lines(&mut self, _color: u32, _points: &[[f32; 2]], _lines: &[[u16; 2]]) {}
    fn r_image(&mut self, _image: u32, _sx: f32, _sy: f32, _swidth: f32, _sheight: f32, _dx: f32, _dy: f32, _dwidth: f32, _dheight: f32, _opacity: f32) {}
    fn r_end(&mut self) {}
}

pub struct Bridge {
    pub instance: apexdream::Instance,
    pub interface: CppInterface,
}

#[no_mangle]
pub unsafe extern "C" fn apexdream_init(
    ctx: *mut c_void,
    read_fn: ReadMemFn,
    write_fn: WriteMemFn,
    get_screen_size_fn: GetScreenSizeFn,
    base_address: u64,
    gamedata_ini: *const c_char,
) -> *mut Bridge {
    let gd_str = CStr::from_ptr(gamedata_ini).to_str().unwrap_or("");

    let mut interface = CppInterface {
        ctx,
        read_fn,
        write_fn,
        get_screen_size_fn,
        start_time: Instant::now(),
        base_address,
    };

    let mut instance = apexdream::Instance::default();
    if instance.attach(&mut interface, gd_str) {
        Box::into_raw(Box::new(Bridge { instance, interface }))
    } else {
        std::ptr::null_mut()
    }
}

#[no_mangle]
pub unsafe extern "C" fn apexdream_tick(bridge_ptr: *mut Bridge) {
    if bridge_ptr.is_null() { return; }
    let bridge = &mut *bridge_ptr;
    bridge.instance.tick(&mut bridge.interface);
}

#[no_mangle]
pub unsafe extern "C" fn apexdream_load_config(bridge_ptr: *mut Bridge, config_ini: *const c_char) {
    if bridge_ptr.is_null() { return; }
    let bridge = &mut *bridge_ptr;
    let config_str = CStr::from_ptr(config_ini).to_str().unwrap_or("");
    bridge.instance.load_config(&mut bridge.interface, config_str);
}

#[no_mangle]
pub unsafe extern "C" fn apexdream_free(bridge_ptr: *mut Bridge) {
    if !bridge_ptr.is_null() {
        let _ = Box::from_raw(bridge_ptr);
    }
}
