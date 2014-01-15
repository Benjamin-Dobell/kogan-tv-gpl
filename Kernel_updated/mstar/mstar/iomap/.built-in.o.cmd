cmd_drivers/mstar/iomap/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/mstar/iomap/built-in.o drivers/mstar/iomap/mdrv-iomap.o ; scripts/mod/modpost drivers/mstar/iomap/built-in.o
