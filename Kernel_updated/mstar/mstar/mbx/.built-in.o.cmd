cmd_drivers/mstar/mbx/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/mstar/mbx/built-in.o drivers/mstar/mbx/mdrv-mbx.o ; scripts/mod/modpost drivers/mstar/mbx/built-in.o
