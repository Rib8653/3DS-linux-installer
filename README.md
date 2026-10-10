# Linux for 3DS - Installer



| RomFS file | SD destination |
|---|---|
| zImage, nintendo3ds_ctr.dtb, nintendo3ds_ktr.dtb, arm9linuxfw.bin, initramfs.cpio.gz | `sdmc:/linux/` |
| firm_linux_loader.firm | `sdmc:/luma/payloads/` |

## Build

Requires devkitPro (devkitARM + libctru):

    make            # -> linux-installer.3dsx
