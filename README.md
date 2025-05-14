# Zedboard UART games

This is a PetaLinux project for the Zedboard with a custom UART module running on the Zynq PL (FPGA), controlled by the Zynq PS (ARM CPU) via AXI4. It includes a Linux driver for the UART at /dev/myuart and a collection of open source ncurses games customized to run on a hardware terminal VT510 (running on VT100 emulation mode).

The custom UART module runs at 19200 baud rate, with 1 stop bit and no parity bits. Its RX line is at JA1 and its TX line is at JA7.

You can build the project with make but it requires PetaLinux 2021.2 to be installed on your system.

Running make creates a PetaLinux Zynq project, configures it based on the .XSA hardware file exported from Vivado, adds the custom kernel module (called myuart) and recipes for running games on it, builds the project, and finally packages the required boot files in images/linux/. Keep in mind this takes a while, at least 20-30 minutes.

After make is done, you can copy the boot files onto an SD card. The SD card needs to have two partitions, one small FAT32 (256MB is enough) and one EXT4 which should be at least ~400MB to fit the rootfs.ext4 filesystem image. The files BOOT.BIN, boot.scr, and image.ub should be copied to the FAT32 partition. The rootfs.ext4 file should be copied directly to the EXT4 partition with something like dd. Make sure your Zedboard is setup for SD card boot.

The myuart driver is automatically loaded at boot (at /dev/myuart). The driver can handle open(), read(), write(), poll() system calls. There is also a helper program called myuart_run which will first redirect STDIN/STDOUT/STDERR to /dev/myuart and then run an executable given as argument. For playing games, there is an executable called gameselection, which is essentially a menu, written with ncurses, that gives the option to launch several games. These two executables can both be found in /usr/bin/. The games are in /usr/bin/myuart-games/. There is also a startup script which runs "myuart_run gameselection" at boot.
