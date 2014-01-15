cmd_drivers/mstar/mpool/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/mstar/mpool/built-in.o drivers/mstar/mpool/mdrv-mpool.o ; scripts/mod/modpost drivers/mstar/mpool/built-in.o
