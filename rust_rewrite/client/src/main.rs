mod overlay;

use windows::Win32::Foundation::*;
use windows::Win32::UI::WindowsAndMessaging::*;
use windows::Win32::Graphics::Gdi::*;
use windows::Win32::System::Memory::*;

fn main() -> anyhow::Result<()> {
    println!("Starting Rust Apex Guest Client...");

    // Basic window setup for overlay would go here
    // Using imgui-rs for the menu

    // Setup the shared 'add' array in memory for host communication
    unsafe {
        let h_module = windows::Win32::System::LibraryLoader::GetModuleHandleA(None)?;
        let add_array = VirtualAlloc(None, 71 * 8, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        println!("Add array allocated at: {:?}", add_array);

        let offset = (add_array as u64) - (h_module.0 as u64);
        println!("Add offset: 0x{:x}", offset);
    }

    loop {
        // Main overlay render loop
        std::thread::sleep(std::time::Duration::from_millis(16));
    }
}
