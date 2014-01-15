#!/bin/sh
#ln -s ../../../mstar/mips-boards ./kernel/arch/mips/mips-boards

#chmod 755 ./scripts/mkimage

[  -e ./include/linux/netfilter_ipv4/ipt_ECN_U.h ] && mv ./include/linux/netfilter_ipv4/ipt_ECN_U.h  ./include/linux/netfilter_ipv4/ipt_ECN.h
[  -e ./include/linux/netfilter_ipv4/ipt_TTL_U.h ] && mv ./include/linux/netfilter_ipv4/ipt_TTL_U.h ./include/linux/netfilter_ipv4/ipt_TTL.h
[  -e ./include/linux/netfilter_ipv6/ip6t_HL_U.h ] && mv ./include/linux/netfilter_ipv6/ip6t_HL_U.h ./include/linux/netfilter_ipv6/ip6t_HL.h
[  -e ./include/linux/netfilter/xt_CONNMARK_U.h ] && mv ./include/linux/netfilter/xt_CONNMARK_U.h ./include/linux/netfilter/xt_CONNMARK.h  
[  -e ./include/linux/netfilter/xt_DSCP_U.h ] && mv ./include/linux/netfilter/xt_DSCP_U.h ./include/linux/netfilter/xt_DSCP.h 
[  -e ./include/linux/netfilter/xt_MARK_U.h ] && mv ./include/linux/netfilter/xt_MARK_U.h ./include/linux/netfilter/xt_MARK.h 
[  -e ./include/linux/netfilter/xt_RATEEST_U.h ] && mv ./include/linux/netfilter/xt_RATEEST_U.h ./include/linux/netfilter/xt_RATEEST.h 
[  -e ./include/linux/netfilter/xt_TCPMSS_U.h ] && mv ./include/linux/netfilter/xt_TCPMSS_U.h ./include/linux/netfilter/xt_TCPMSS.h 
[  -e ./net/ipv4/netfilter/ipt_ECN_U.c  ] && mv ./net/ipv4/netfilter/ipt_ECN_U.c ./net/ipv4/netfilter/ipt_ECN.c
[  -e ./net/netfilter/xt_DSCP_U.c  ] && mv ./net/netfilter/xt_DSCP_U.c ./net/netfilter/xt_DSCP.c
[  -e ./net/netfilter/xt_HL_U.c ] && mv ./net/netfilter/xt_HL_U.c ./net/netfilter/xt_HL.c
[  -e ./net/netfilter/xt_RATEEST_U.c ] && mv ./net/netfilter/xt_RATEEST_U.c ./net/netfilter/xt_RATEEST.c
[  -e ./net/netfilter/xt_TCPMSS_U.c ] && mv ./net/netfilter/xt_TCPMSS_U.c ./net/netfilter/xt_TCPMSS.c


[ ! -e ./arch/arm/arm-boards ] && ln -s ../../../mstar/arm-boards/ ./arch/arm/arm-boards
[ ! -e ./include/linux/autoconf.h ] && ln -s ../generated/autoconf.h ./include/linux/autoconf.h
[ ! -e ./drivers/mstar ] && ln -s ../../mstar/mstar ./drivers/mstar

#[ ! -e ./drivers/mmc/host/mstar_mci.h ] && ln -s ../../../../mstar/mstar/mmc/host/mstar_mci.h ./drivers/mmc/host/mstar_mci.h
#[ ! -e ./drivers/mmc/host/mstar_mci.c ] && ln -s ../../../../mstar/mstar/mmc/host/mstar_mci.c ./drivers/mmc/host/mstar_mci.c
#[ ! -e ./arch/arm/include/asm/mach-mstar] && ln -s ../mstar/include/asm-mips/mach-mstar ./arch/arm/include/asm/mach-mstar
#[ ! -e ./mstar/mstar/firmware ] && ln -s ../../../mstar/mstar/firmware ./mstar/mstar/firmware
#[ ! -e ./mstar/mstar/flash ] && ln -s ../../../mstar/mstar/flash ./mstar/mstar/flash
#[ ! -e ./mstar/mstar/g3d ] &&  ln -s ../../../mstar/mstar/g3d ./mstar/mstar/g3d
#[ ! -e ./mstar/mstar/gpio ] && ln -s ../../../mstar/mstar/gpio ./mstar/mstar/gpio
#[ ! -e ./mstar/mstar/gflip ] &&  ln -s ../../../mstar/mstar/gflip ./mstar/mstar/gflip
#[ ! -e ./mstar/mstar/iic ] && ln -s ../../../mstar/mstar/iic ./mstar/mstar/iic
#[ ! -e ./mstar/mstar/msglog ] && ln -s ../../../mstar/mstar/msglog ./mstar/mstar/msglog
#[ ! -e ./mstar/mstar/msystem ] && ln -s ../../../mstar/mstar/msystem ./mstar/mstar/msystem
#[ ! -e ./mstar/mstar/ramdisk ] && ln -s ../../../mstar/mstar/ramdisk ./mstar/mstar/ramdisk
#[ ! -e ./mstar/mstar/rfid ] && ln -s ../../../mstar/mstar/rfid ./mstar/mstar/rfid
#[ ! -e ./mstar/mstar/shire ] && ln -s ../../../mstar/mstar/shire ./mstar/mstar/shire
#[ ! -e ./mstar/mstar/swreg ] && ln -s ../../../mstar/mstar/swreg ./mstar/mstar/swreg
#[ ! -e ./mstar/mstar/tsp ] && ln -s ../../../mstar/mstar/tsp ./mstar/mstar/tsp
#[ ! -e ./mstar/mstar/nand ] && ln -s ../../../mstar/mstar/nand ./mstar/mstar/nand
#[ ! -e ./mstar/mstar/onenand ] && ln -s ../../../mstar/mstar/onenand ./mstar/mstar/onenand
#[ ! -e ./mstar/mstar/emac  ] && ln -s ../../../mstar/mstar/emac ./mstar/mstar/emac
#[ ! -e ./mstar/mstar/system ] && ln -s ../../../mstar/mstar/system ./mstar/mstar/system
#[ ! -e ./mstar/mstar/include ] && ln -s ../../../mstar/mstar/include ./mstar/mstar/include
#[ ! -e ./mstar/mstar/mpool ] && ln -s ../../../mstar/mstar/mpool ./mstar/mstar/mpool
#[ ! -e ./mstar/mstar/iomap ] && ln -s ../../../mstar/mstar/iomap ./mstar/mstar/iomap
#[ ! -e ./mstar/mstar/fusion ] && ln -s ../../../mstar/mstar/fusion ./mstar/mstar/fusion
#[ ! -e ./mstar/mstar/sc ] &&  ln -s ../../../mstar/mstar/sc ./mstar/mstar/sc
#[ ! -e ./mstar/mstar/vid_pre_alloc_mem ] && ln -s ../../../mstar/mstar/vid_pre_alloc_mem ./mstar/mstar/vid_pre_alloc_mem
#[ ! -e ./mstar/mstar/ir ] && ln -s ../../../mstar/mstar/ir ./mstar/mstar/ir
#[ ! -e ./mstar/mstar/mbx ] && ln -s ../../../mstar/mstar/mbx ./mstar/mstar/mbx
#[ ! -e ./mstar/mstar/mpif ] && ln -s ../../../mstar/mstar/mpif ./mstar/mstar/mpif
#[ ! -e ./mstar/mstar/alsa_audio_driver ] && ln -s ../../../mstar/mstar/alsa_audio_driver ./mstar/mstar/alsa_audio_driver
