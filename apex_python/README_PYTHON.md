# Apex Legends Python Rewrite

This is a Python-based implementation of the Apex DMA project, designed to run on a Host (Linux/KVM) and a Guest (Windows).

## Project Structure

- `apex_python/server/`: Python Host logic (Aimbot, ESP calculation, Glow).
- `apex_python/client/`: Python Guest overlay (ImGui, Config).
- `apex_python/communication.py`: Shared memory bridge definitions.

## Requirements

- Python 3.8+
- `pip install -r apex_python/requirements.txt`
- `memflow` with `kvm` or `qemu` connector installed on the Host.

## Usage

1. **Guest**: Run `python -m apex_python.client.main`
2. **Host**: Run `sudo python -m apex_python.server.main`

## Notes

- This rewrite retains the original DMA architecture where the Host performs memory scanning and the Guest provides the overlay.
- Ensure the `add_off` in `apex_python/server/main.py` matches the offset displayed by the Guest client if necessary.
