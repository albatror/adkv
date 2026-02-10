# apex_dma_kvm_pub 3.0
 Apex Legends QEMU/KVM hack memflow

UnknownCheats thread: https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html

Bone IDs reference: https://www.unknowncheats.me/wiki/Apex_Legends_Bones_and_Hitboxes

Game version (Steam & origin): 2026/02/08

That's made to work on QEMU/KVM, so we dont talk about Linux Host, Windows VM, passthrough and detection configuration

- Tested on Last release of Fedora, Ubuntu and ProxMox
- Working on Windows 10 20H1 (never below) - Tested with stable CR3 on 22H2

- 1920*1080 (need to be changed manually - search 2560 and/or 1440 in apex_dma.cpp)
- 2560*1440 (default)

- Now include and fix memflow-qemu and memflow-kvm (we use kvm for better perfomances) lib in the project
- Now include libmemflow_kvm.so, libmemflow_qemu.so and libmemflow_win32.so in build folder after success build
- Now include Skeleton ESP
- Now include Offsets Dumper (Full dump and update offsets.ini)

INSTALL :
 - Download sources from there *.zip or git
 - Extract it
 - Install Cargo & Rust with the famous curl https://sh.rustup.rs/ -sSf | sh (dont forget to add your USER)
 - Install memflow with the famous curl --proto '=https' --tlsv1.2 -sSf https://sh.memflow.io/ | sh (dont forget to add your USER)
 - execute : sudo modprobe memflow (at every start of the host - can be added with cron ..)
 - Compile with the build.sh to see if any errors.
 - When build is ok without errors, start Overlay (obfuscated) then Client (obfuscated)

 - Take the offset in the console and copy it then close Client and Overlay by pressing F4
 - Open apex_dma.cpp with editor then find uint64_t add_off = 0X0000000; at the end and replace with the offset from Client console : uint64_t add_off = YOUR_OFFSET; (You have to do that the first time after install only)
 - Compile again the server with ./build.sh

 - When build is ok without errors, start Overlay (obfuscated) then Client (obfuscated)
 - Start the game
 - On the Linux console (host) start the server by : sudo -E ./apex_dma

VISUALS :
 - ESP Box, XP Level, Line, Name, Distance, Skeleton
 - ESP Seer Health and Shield
 - Circle FOV
 - Players Glow (GREEN when visible, RED when not visible and YELLOW when knocked - Can be changed in the overlay's menu)
 - Items Glow (complete disable)
 - Spectators count + Name List (Name List temporary disable but you can active it)

FEATURES :
 - Work in BoderLess and FullScreen
 - work with DirectX 12
 - Added "Lock On Target" feature. This feature prevents the aimbot from switching targets mid-fight, focusing on the first enemy targeted as long as the aim button is held. Can be choose in Overlay menu.
 - Added a visual DOT to know if you're connected on the server (GREEN DOT) or disconnected (RED DOT)
 - Added the BruteForce and optimization CR3/DTB fix by MisterY
 - Press F1 (To Activate GloW, ESP Seer Health/Shield and Aimbot based on default conf)
 - Dynamic FOV/AIM/SMOOTH [70m by default] Process Updated & Optimized 04/12/2024 - Settings added in overlay
 - AutoSuperGlide (nothing to do, it do for you :) ) !Someone tell me to check that, i will.
 - AutoWallJump updated (Slide then jump on a wall, dont touch anything else and it'll auto-walljump)
 - Offsets Dumper included - F9 to full dump offsets and F10 for update. Thanks & Crédits to to Apex-CV

 * 1V1 (Temporay disable)
 * Training with dummies (Temporay disable)

!!! WARNING !!!

 - If using Client.exe and Overlay.exe - obfuscate both files to avoid detection.
 - If using Client.exe and Overlay from NVIDIA - obfuscate both files to avoid detection.

<img src="https://github.com/albatror/adkv/blob/master/demo/settingsS.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/settingsS2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/Demo2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/settings.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/settingsN2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/ingame1.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/ingame2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/show1.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/console.png" style="display: block; margin: auto;" />
