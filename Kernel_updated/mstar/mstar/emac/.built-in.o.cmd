cmd_drivers/mstar/emac/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/mstar/emac/built-in.o drivers/mstar/emac/mdrv-emac.o ; scripts/mod/modpost drivers/mstar/emac/built-in.o
