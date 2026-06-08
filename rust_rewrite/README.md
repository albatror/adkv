# 🦀 Apex Legends Rust DMA Rewrite

This is a high-performance rewrite of the Apex Legends DMA cheat (Server and Client) in Rust. It utilizes the **memflow** library for native memory access and provides a modern, type-safe foundation for game enhancement.

## 🚀 Overview

The project is split into two main components:
- **Server (Host):** A Rust application running on the host machine (Linux) that performs DMA memory reads/writes via memflow.
- **Client (Guest):** A Rust application running on the guest machine (Windows) that handles the overlay rendering and user interface.

## 🛠️ Installation

### Host (Linux)

1.  **Install Rust:**
    ```bash
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
    ```

2.  **Setup Memflow:**
    Follow the official [memflow installation guide](https://github.com/memflow/memflow).
    Ensure you have the necessary connectors (e.g., `kvm` or `qemu`) installed.

3.  **Clone and Build:**
    ```bash
    cd rust_rewrite/server
    cargo build --release
    ```

### Guest (Windows)

1.  **Install Rust:**
    Download and run the installer from [rustup.rs](https://rustup.rs/).

2.  **Build the Client:**
    ```bash
    cd rust_rewrite/client
    cargo build --release
    ```

## 📖 Usage

1.  **Start the Guest Client:**
    Run the compiled `client.exe` on your Windows guest machine. It will allocate the shared memory bridge and print an "add offset".
2.  **Configure the Server:**
    Update the `add_off` in `server/src/main.rs` with the offset from the client if not dynamically detected.
3.  **Run the Host Server:**
    On your Linux host, run the server with root privileges:
    ```bash
    sudo ./target/release/server
    ```
4.  **In-Game:**
    Use the overlay menu (default key: **INSERT**) to configure your settings.

## ⚙️ Settings & Features

### Visuals (ESP)
- **Player ESP:** Render boxes, health bars, and skeletons.
- **Glow:** Customizable player glow for visible, hidden, and knocked states.
- **Spectator List:** Monitor who is watching you in real-time.

### Combat
- **Aimbot:** Smooth, target-prioritized aiming with adjustable FOV and smoothing algorithms.
- **Triggerbot:** Automated firing when a target is within the crosshair.
- **Aim Assist:** Subtle assistance for a more natural feel.

### Movement
- **Mantle Boost (Superglide):** Automated frame-perfect superglides.
- **Bhop:** Continuous bunny hopping while holding the spacebar.

## ⚠️ Safety Warning

- **DMA Stealth:** This project is designed for use with DMA hardware or KVM/QEMU for maximum stealth.
- **Use at Risk:** Cheating in online games can result in permanent bans. Always use obfuscation for the guest-side client.
