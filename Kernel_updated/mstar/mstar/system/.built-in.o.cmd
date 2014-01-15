cmd_drivers/mstar/system/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/mstar/system/built-in.o drivers/mstar/system/mdrv-system.o ; scripts/mod/modpost drivers/mstar/system/built-in.o
