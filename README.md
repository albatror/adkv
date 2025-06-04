That's made to work on QEMU/KVM, so we dont talk about Linux Host, Windows VM and passthrough configuration

INSTALL : 
 - Download sources from there *.zip or git
 - Extract it
 - Install Cargo & Rust with the famous curl https://sh.rustup.rs/ -sSf | sh (dont forget to add your USER)
 - Install memflow with the famous curl --proto '=https' --tlsv1.2 -sSf https://sh.memflow.io/ | sh (dont forget to add your USER)
 - check https://github.com/memflow/memflow-kvm and/or https://github.com/memflow/memflowup (better) to install memflow-kvm connector.
 - Compile with the build.sh to see if any errors.
 - When build is ok without errors, start Overlay (obfuscated) then Client (obfuscated)
 - On the Linux console (host) start the server by : sudo ./apex_dma
    
