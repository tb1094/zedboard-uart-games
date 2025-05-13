# Zedboard UART games

This is a PetaLinux project for the Zedboard with a custom UART module running on the Zynq PL (FPGA), controlled by the Zynq PS (ARM CPU) via AXI4. It includes a Linux driver for the UART at /dev/myuart and a collection of ncurses games customized to run on a hardware terminal VT510 (running on VT100 emulation mode).

You can build the project using the Makefile but it requires PetaLinux 2021.2 to be installed on your system. 
