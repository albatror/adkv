# ⚡ Apex Legends DMA Hack v3.0 (QEMU/KVM)

[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-blueviolet?style=for-the-badge&logo=linux)](https://github.com/albatror/adkv)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=c%2B%2B)](https://github.com/albatror/adkv)
[![Technology](https://img.shields.io/badge/Powered%20By-Memflow-orange?style=for-the-badge)](https://memflow.io/)
[![Version](https://img.shields.io/badge/Version-v3.0.3.55-green?style=for-the-badge)](https://github.com/albatror/adkv)

A high-performance Apex Legends DMA hack designed for **QEMU/KVM** environments using the **memflow** library. This project provides a stealthy and efficient way to enhance your gameplay with minimal footprint.
We use kvm connector for best performances.

![Showcase](https://github.com/albatror/adkv/raw/master/demo/optimize.gif)

---

GameVersion v3.0.3.55

## 🚀 Features

### 🛡️ Visuals (ESP)
- **ESP Boxes:** Clear 2D boxes around players.
- **Health & Shields:** Integrated Seer-style health and shield bars.
- **Skeleton:** Real-time bone rendering with configurable line thickness.
- **Glow:** Fully customizable per-state player glow with RGB color pickers:
  - Green: Visible | Red: Hidden | White: Knocked (all independently configurable).
- **Extra Info:** Player XP Level, Player Name, Player Distance, Player Weapons, Player Platform, and Snap-lines.
- **Spectator List:** Track who is watching you — Count + Names for both enemy and allied spectators.
- **Target Indicator:** Visual DOT with configurable FOV showing the best nearby target.
- **Glow Items:** Loba-style item glow.
- **FOV Circles:** Independent overlay circles for ADS (Cyan), Hipfire (Magenta), and Triggerbot FOV.
- **Info Window:** Always-on base info panel with optional logo display.

### 🔫 Combat & Aimbot
- **Advanced Aimbot:** Smooth and adjustable aiming with dedicated settings for ADS and Hipfire.
- **Dynamic Switching:** Automatically toggles between ADS and Hipfire FOV/Smoothing based on zoom state.
- **Target Priority System:** Intelligent target selection using a weighted formula (`FOV + distance`) for sticky, stable tracking.
- **Lock On Target:** Prevents target switching during active combat.
- **No Recoil / Sway:** Compensates for weapon recoil and sway.
- **Visibility Check:** Optional line-of-sight filter (aimbot mode 2).

#### 🎯 Smoothing System (6 modes)
Select the algorithm in the **Config** tab:

| # | Mode | Description |
|---|------|-------------|
| 0 | **SmoothDamp** | Spring-damper — fluid, natural feel. Best for AR / LMG. |
| 1 | **Linear** | Constant-fraction, direct/raw tracking. |
| 2 | **Bezier** | Quadratic ease-out — fast approach, gentle arrival. |
| 3 | **Cubic Bezier** | Ease-in-out — smooth acceleration and deceleration. |
| 4 | **S-Curve** | Smootherstep — ultra-smooth, zero 2nd derivative. Best for snipers. |
| 5 | **Auto** | Weapon + distance aware — picks the best algorithm automatically: |
|   |        | • AR (R-301, Flatline, Havoc, Hemlok, Nemesis) → **SmoothDamp** |
|   |        | • LMG (Spitfire, Devotion, Rampage, L-STAR) → **SmoothDamp** |
|   |        | • SMG / Pistols / Shotguns → **Linear** (≤2500u) or **Bezier** (medium range) |
|   |        | • Snipers / Marksman → **S-Curve** |

#### 🤖 Aim Assist (RMB)
- Graduated pull based on crosshair proximity: inner 35% of bubble = full strength, outer zone = linear falloff.
- Configurable: distance, FOV bubble, smoothing, strength (0.0 = off → 1.0 = direct snap).
- At strength 1.0: snap mode bypasses smoothing for instant lock.

#### 🔫 Triggerbot (LSHIFT)
- Non-blocking state machine for high-performance, responsive automated firing.
- Configurable FOV and hitbox target:

| Option | Target |
|--------|--------|
| NONE | AbsOrigin + static offset |
| Head (0) | Bone 0 |
| Neck (1) | Bone 1 |
| Upper Chest (2) | Bone 2 |
| Lower Chest (3) | Bone 3 |
| Stomach (4) | Bone 4 |
| Hip (11) | Bone 11 |
| NEAR3 | Bones 0, 1, 2 |
| NEAR6 | Bones 0–4 + 11 |
| NEAR12 | 12 major bones |
| ALL | All 17 bones |

#### 📐 Ballistic Prediction
- Configurable bullet speed offset, bullet gravity offset, and aim offset multiplier.
- Target FPS selector: 60 / 75 / 144 FPS.

#### 🛑 Dead Zone
- Suppresses micro-corrections below a configurable angular threshold (X and Y independently).

#### 🔧 Weapon Adjustments
- **Bow Adjust:** Compensation for Bocek arrow arc trajectory.
- **On Sheila / Hold Sheila:** Behavior flags for Rampart Turret operation mode.

#### 🎯 Aim Snap
- Locks the aimbot onto a specific target index slot to prevent switching mid-fight.

### 🏃 Movement & Misc
- **Auto SuperGlide (Mantle Boost):** Perfect glides every time.
- **Auto WallJump:** Fluid wall jumps (Slide → Jump → Wall → Auto-jump).
- **BunnyHop (BHOP):** Effortless movement across the map [HOLD SPACEBAR].
- **Firing Range Mode:** Enables all features in the training/firing range area.
- **1V1:** Battle in the training area against friends.
- **DirectX 12 Support:** Fully compatible with DX12 game mode.
- **Offset Dumper:** Built-in dumper to keep the hack updated with game patches (output in host `/build`).

### 🔍 Aiming Debug Indicator
- **Info Window** (Visuals tab → "Info window") displays a live `- Aiming` row:
  - **Green** while the aim button is held → confirms `aiming = true` is written and readable at `aiming_addr` (client `add[3]`).
  - **Red** when the button is released.
- Useful to verify the full DMA chain: `IsKeyDown(aim_key)` → `aiming` (client) → `add[3]` → `aiming_addr` (host read).

### 💾 Config System
- Full save/load to `Settings.txt` covering all parameters (aim, ESP, glow, smoothing, prediction, dead zone, snap, movement…).
- **INSERT** to open/close the menu; save/load buttons in the Config tab.

---

## 💻 Requirements

### Host (Linux)
- **Supported Distros:** Fedora, Ubuntu, Debian, Proxmox.
- **Hardware:** Second PC or KVM-compatible setup for DMA.
- **Dependencies:** Cargo, Rust, Memflow.

### Guest (Windows)
- **OS:** Windows 10 (Version 20H1 or less than 23H2).
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
    Memflow-qemu, memflow-kvm, other connectors and the dumper will be compiled at build and everything will be copied into the build folder on success.

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
2.  **Guest Side:** Nothing is shown from Client/Overlay at start to prevent screenshot detection.
3.  **Game:** Start Apex Legends.
4.  **Host Side:** Run the server with root privileges:
    ```bash
    cd build
    sudo -E ./apex_dma
    ```
5.  **Guest Side:** Press INSERT to open the menu and save your config. You can also choose to show Overlay Base Infos and/or the Spectator List at start by selecting them in the menu.

### Hotkeys
- **INSERT:** Open/Close Overlay Menu.
- **F1:** Activate/Deactivate Full Suite (Glow players, Glow items and ESP).
- **F5:** Activate/Deactivate ESP Only.
- **F8:** Activate/Deactivate Items Glow Only.
- **F9:** Run Full Offset Dump.
- **F10:** Dynamic Update Offsets from INI to offsets.h and to Game memory.
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
