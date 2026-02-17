# ⚡ Apex Legends DMA Hack v3.0 (QEMU/KVM)

[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-blueviolet?style=for-the-badge&logo=linux)](https://github.com/albatror/adkv)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=c%2B%2B)](https://github.com/albatror/adkv)
[![Technology](https://img.shields.io/badge/Powered%20By-Memflow-orange?style=for-the-badge)](https://memflow.io/)
[![Version](https://img.shields.io/badge/Version-v3.0.3.24-green?style=for-the-badge)](https://github.com/albatror/adkv)

A high-performance Apex Legends DMA hack designed for **QEMU/KVM** environments using the **memflow** library. This project provides a stealthy and efficient way to enhance your gameplay with minimal footprint.
We use kvm connector for best performances.

![Showcase](https://github.com/albatror/adkv/raw/master/demo/optimize.gif)

---

GameVersion v3.0.3.24

## 🚀 Features

### 🛡️ Visuals (ESP)
- **ESP Boxes:** Clear 2D boxes around players.
- **Health & Shields:** Integrated Seer-style health and shield bars.
- **Skeleton:** Real-time bone rendering for precise movement tracking.
- **Glow:** Highly customizable player glow (Green: Visible, Red: Hidden, Yellow: Knocked).
- **Extra Info:** XP Level, Player Name, Distance, and Snap-lines.
- **Spectator List:** Keep track of who is watching you (Count + Names).
- **Target Indicator:** A visual DOT and FOV circle for Aimbot targets.

### 🔫 Combat & Aimbot
- **Aimbot:** Smooth and adjustable aiming with FOV locking.
- **Lock On Target:** Prevents target switching during active combat.
- **FlickBot:** Fast target acquisition with configurable FOV and smoothness.
- **TriggerBot:** Automated firing when crosshairs are over an enemy.
- **Dynamic Settings (DDS):** On-the-fly adjustment of FOV and Smoothing (Default: 40m).

### 🏃 Movement & Misc
- **Auto SuperGlide:** Perfect glides every time.
- **Auto WallJump:** Fluid wall jumps (Slide -> Jump -> Wall -> Auto-jump).
- **BunnyHop (BHOP):** Effortless movement across the map.
- **DirectX 12 Support:** Fully compatible with DX12 game mode.
- **Offset Dumper:** Built-in dumper to keep the hack updated with game patches.

---

## 💻 Requirements

### Host (Linux)
- **Supported Distros:** Fedora, Ubuntu, Proxmox.
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
    sudo modprobe memflow
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

1.  **Guest Side:** Start the **Overlay** (obfuscated) and **Client** (obfuscated).
2.  **Guest Side:** Nothing is showed from Client/Overlay at start to prevent screenshoot detection.
3.  **Game:** Start Apex Legends.
4.  **Host Side:** Run the server with root privileges:
    ```bash
    sudo -E ./apex_dma
    ```
5. **Guest Side:** Press INSERT to active the menu and save your config. You can aswell choose to show Overlay Base Infos and/or Spectators's List at start by slecting them in menu. 

### Hotkeys
- **INSERT:** Open/Close Overlay Menu.
- **F1:** Activate Full Suite (Glow, ESP, Aimbot).
- **F5:** Activate ESP Only.
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
