# ⚡ Apex Legends DMA Hack v3.0 (QEMU/KVM)

[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-blueviolet?style=for-the-badge&logo=linux)](https://github.com/albatror/adkv)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=c%2B%2B)](https://github.com/albatror/adkv)
[![Technology](https://img.shields.io/badge/Powered%20By-Memflow-orange?style=for-the-badge)](https://memflow.io/)
[![Version](https://img.shields.io/badge/Version-v3.0.1.35-green?style=for-the-badge)](https://github.com/albatror/adkv)

A high-performance Apex Legends DMA hack designed for **QEMU/KVM** environments using the **memflow** library. This project provides a stealthy and efficient way to enhance your gameplay with minimal footprint.
We use kvm connector for best performances.

![Showcase](https://github.com/albatror/adkv/raw/master/demo/optimize.gif)

---

GameVersion v3.0.2.49

## 🚀 Features

### 🛡️ Visuals (ESP)
- **ESP Boxes:** Clear 2D boxes around players.
- **Health & Shields:** Integrated Seer-style health and shield bars.
- **Skeleton:** Real-time bone rendering for precise movement tracking.
- **Glow:** Highly customizable player glow (Green: Visible, Red: Hidden, Yellow: Knocked).
- **Extra Info:** PLayer XP Level, Player Name, PLayer Distance, Players Weapons, Players Platforms and Snap-lines.
- **Spectator List:** Keep track of who is watching you (Count + List of Names).
- **Target Indicator:** A visual DOT with configurable FOV to show best near target.
- **Glow Items:** Glow items (Loba Style)

### 🔫 Combat & Aimbot
- **Advanced Aimbot:** Smooth and adjustable aiming with dedicated settings for ADS and Hipfire.
- **Dynamic Switching:** Automatically toggles between ADS and Hipfire FOV/Smoothing based on your zoom state.
- **Target Priority System:** Intelligent target selection using a weighted formula (`FOV + distance`) for sticky, stable tracking.
- **Lock On Target:** Prevents target switching during active combat.
- **Professional Triggerbot:** Refactored non-blocking state machine for high-performance, responsive automated firing [HOLD Left SHIFT Key to activate].
- **Visual FOV Feedback:** Independent overlay circles for ADS (Cyan) and Hipfire (Magenta) ranges.

### 🏃 Movement & Misc
- **Auto SuperGlide:** Perfect glides every time.
- **Auto WallJump:** Fluid wall jumps (Slide -> Jump -> Wall -> Auto-jump).
- **BunnyHop (BHOP):** Effortless movement across the map |HOLD SPACEBAR Key].
- **DirectX 12 Support:** Fully compatible with DX12 game mode.
- **Offset Dumper:** Built-in dumper to keep the hack updated with game patches (dump will be on host side /build).
- **1V1:** Battle in training against friends.
- **Monitor EDID Spoof:** Automated hardware identification spoofing (Serial, Name, Vendor) using the `EDID_OVERRIDE` registry method. Execution is forced at startup before any game connection for maximum safety.

---

## 💻 Requirements

### Host (Linux)
- **Supported Distros:** Fedora, Ubuntu, Debian, Proxmox.
- **Hardware:** Second PC or KVM-compatible setup for DMA.
- **Dependencies:** Cargo, Rust, Memflow.

### Guest (Windows)
- **OS:** Windows 10 (Version 20H1 or newer).
- **Tested:** Stable on 22H2 with CR3 fix.
- **Resolution:** 2560x1440 (Default) | 1920x1080 (Manual change required in `apex_dma.cpp`).

---

## 🛠️ Installation

1.  **Download Source:**
    ```bash
    git clone https://github.com/albatror/adkv.git
    cd adkv
    ```

2.  **Install Toolchain:**
    Install Rust & Cargo:
    ```bash
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
    ```

3.  **Setup Memflow:**
    Install memflow:
    ```bash
    curl --proto '=https' --tlsv1.2 -sSf https://sh.memflow.io/ | sh
    sudo modprobe memflow (necessary at start of host, you can add to cron)
    ```

4.  **Initial Build:**
    ```bash
    ./build.sh
    ```
    Memflow-qemu, memflow-kvm, others connectors and dumper will be compile at build and everything will be copied into build folder if succes.

5.  **Configure `add_off` (CRITICAL STEP):**
    - Start the **Overlay** and **Client** on the guest.
    - Note the offset displayed in the Client console.
    - Close them (F4).
    - Open `apex_dma/apex_dma.cpp`.
    - Find `uint64_t add_off = 0x000000;` and replace it with your offset.
    - Re-run `./build.sh`.

---

## 📖 Usage

### 🔄 Execution Flow (Automated Spoofing)
The project enforces a strict initialization sequence to ensure your monitor HWID is spoofed before Apex Legends is even detected:

1.  **Start Guest:** Run **Overlay.exe** and **Client.exe** on the game PC.
2.  **Start Host:** Run the server on the DMA PC:
    ```bash
    cd build && sudo -E ./apex_dma
    ```
3.  **Handshake:** The Server connects to the Client and automatically triggers the **EDID Spoof**.
4.  **Verification:** Once the Client confirms the spoof is applied, the Server begins searching for the **Apex Legends** process.
5.  **Game Start:** You can now start Apex Legends (if not already running). The hack will only connect once the spoof is confirmed.

> **Note:** The "Spoof" tab in the overlay menu shows your Real vs. Fake serials for verification.
6. **Guest Side:** Press INSERT to active the menu and save your config. You can aswell choose to show Overlay Base Infos and/or Spectators's List at start by selecting them in menu. 

### Hotkeys
- **INSERT:** Open/Close Overlay Menu.
- **F1:** Activate/Deactivate Full Suite (Glow players, Glow items and ESP).
- **F5:** Activate/Deactivate ESP Only.
- **F8:** Activate/Deactivate Items Glow Only
- **F9:** Run Full Offset Dump.
- **F10:** Update Offsets from INI.
- **F4:** Emergency Close (Guest).

---

## ⚠️ Safety & Warning

- **Obfuscation:** Always obfuscate `Client.exe` and `Overlay.exe` before use to prevent detection.
- **Detection:** Cheating in online games can lead to permanent bans. Use at your own risk.
- **External Links:**
  - [UnknownCheats Thread](https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html)
  - [Bone IDs Reference](https://www.unknowncheats.me/wiki/Apex_Legends_Bones_and_Hitboxes)

---
*Credits to the Memflow team and all contributors, Y33Tcoder, MisterY, Gerosity, ApexCV, KrackerCo, caochuang, and more... .*



## ⚠️ FAQ
- **set up memflow
Quote:
i can't set up memflow can you help me Warning: unrecognized protocol ''=https''
Hey, your shell strips quotes incorrectly
or you're on a slightly different curl version / environment (common on some Linux setups, WSL, or minimal distros)

Just remove the problematic --proto part:

curl -sSf https://sh.memflow.io/ | sh

And same for Rust:

curl -sSf https://sh.rustup.rs | sh and test it
