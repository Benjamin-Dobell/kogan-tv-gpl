cmd_drivers/mstar/gflip/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/mstar/gflip/built-in.o drivers/mstar/gflip/mdrv-gflip.o ; scripts/mod/modpost drivers/mstar/gflip/built-in.o
