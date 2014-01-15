cmd_drivers/mstar/fusion/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/mstar/fusion/built-in.o drivers/mstar/fusion/fusion.o ; scripts/mod/modpost drivers/mstar/fusion/built-in.o
