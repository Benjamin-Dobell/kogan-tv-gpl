cmd_drivers/mstar/mpool/mdrv_mpool.o := arm-none-linux-gnueabi-gcc -Wp,-MD,drivers/mstar/mpool/.mdrv_mpool.o.d  -nostdinc -isystem /opt/arm-2010.09/bin/../lib/gcc/arm-none-linux-gnueabi/4.5.1/include -I/home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/arm-boards/edison//include -Iarch/arm/arm-boards/plat-mstar//include -Iarch/arm/arm-boards/edison/ -Iarch/arm/arm-boards/plat-mstar/ -I drivers/mstar -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -pg -fno-inline-functions-called-once -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -Idrivers/mstar/include -Idrivers/mstar/mpool -Idrivers/mstar/mpool/edison    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(mdrv_mpool)"  -D"KBUILD_MODNAME=KBUILD_STR(mdrv_mpool)" -c -o drivers/mstar/mpool/mdrv_mpool.o drivers/mstar/mpool/mdrv_mpool.c

source_drivers/mstar/mpool/mdrv_mpool.o := drivers/mstar/mpool/mdrv_mpool.c

deps_drivers/mstar/mpool/mdrv_mpool.o := \
    $(wildcard include/config/mips.h) \
    $(wildcard include/config/arm.h) \
    $(wildcard include/config/mstar/titania3.h) \
    $(wildcard include/config/mstar/titania10.h) \
    $(wildcard include/config/mstar/titania8.h) \
    $(wildcard include/config/mstar/titania12.h) \
    $(wildcard include/config/mstar/titania9.h) \
    $(wildcard include/config/mstar/janus2.h) \
    $(wildcard include/config/mstar/kronus.h) \
    $(wildcard include/config/mstar/titania11.h) \
    $(wildcard include/config/mstar/titania13.h) \
    $(wildcard include/config/mstar/amber1.h) \
    $(wildcard include/config/mstar/amber2.h) \
    $(wildcard include/config/mstar/amber5.h) \
    $(wildcard include/config/mstar/amber6.h) \
    $(wildcard include/config/mstar/amber7.h) \
    $(wildcard include/config/mstar/amethyst.h) \
    $(wildcard include/config/mstar/kaiserin.h) \
    $(wildcard include/config/mstar/agate.h) \
    $(wildcard include/config/mstar/amber3.h) \
    $(wildcard include/config/mstar/eagle.h) \
    $(wildcard include/config/mstar/emerald.h) \
    $(wildcard include/config/mstar/edison.h) \
    $(wildcard include/config/mstar/eiffel.h) \
    $(wildcard include/config/mstar/einstein.h) \
    $(wildcard include/config/mstar/titania4.h) \
    $(wildcard include/config/mstar/titania7.h) \
    $(wildcard include/config/mstar/uranus4.h) \
    $(wildcard include/config/mstar/xxxx.h) \
    $(wildcard include/config/mstar/mpool.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/include/linux/kconfig.h \
    $(wildcard include/config/h.h) \
    $(wildcard include/config/.h) \
    $(wildcard include/config/foo.h) \
  include/linux/autoconf.h \
    $(wildcard include/config/ip6/nf/match/ah.h) \
    $(wildcard include/config/arch/omap.h) \
    $(wildcard include/config/nls/codepage/861.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/nf/conntrack/h323.h) \
    $(wildcard include/config/dma/api/debug.h) \
    $(wildcard include/config/scsi/dma.h) \
    $(wildcard include/config/tcp/md5sig.h) \
    $(wildcard include/config/kernel/gzip.h) \
    $(wildcard include/config/input/keyboard.h) \
    $(wildcard include/config/kprobe/event.h) \
    $(wildcard include/config/usb/c67x00/hcd.h) \
    $(wildcard include/config/rfs/accel.h) \
    $(wildcard include/config/ip/nf/target/redirect.h) \
    $(wildcard include/config/mstar/pm/wdt.h) \
    $(wildcard include/config/serio/altera/ps2.h) \
    $(wildcard include/config/crc32.h) \
    $(wildcard include/config/i2c/boardinfo.h) \
    $(wildcard include/config/have/aout.h) \
    $(wildcard include/config/dm/snapshot.h) \
    $(wildcard include/config/mfd/wm831x/i2c.h) \
    $(wildcard include/config/vfp.h) \
    $(wildcard include/config/mstar/alsa/audio/driver/drv.h) \
    $(wildcard include/config/caif.h) \
    $(wildcard include/config/ir/jvc/decoder.h) \
    $(wildcard include/config/fscache.h) \
    $(wildcard include/config/netfilter/xt/match/cpu.h) \
    $(wildcard include/config/ext3/fs/xattr.h) \
    $(wildcard include/config/aeabi.h) \
    $(wildcard include/config/icst.h) \
    $(wildcard include/config/fb/tileblitting.h) \
    $(wildcard include/config/apanic.h) \
    $(wildcard include/config/ir/mce/kbd/decoder.h) \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/mstar/str/count/max.h) \
    $(wildcard include/config/video/wm8739.h) \
    $(wildcard include/config/blk/dev/dm.h) \
    $(wildcard include/config/r3964.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/mstar/sdmmc1/hotp.h) \
    $(wildcard include/config/flatmem/manual.h) \
    $(wildcard include/config/bt/rfcomm.h) \
    $(wildcard include/config/uaccess/with/memcpy.h) \
    $(wildcard include/config/ext3/defaults/to/ordered.h) \
    $(wildcard include/config/mstar/gflip.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/nf/conntrack/netbios/ns.h) \
    $(wildcard include/config/scsi/dh.h) \
    $(wildcard include/config/scsi/logging.h) \
    $(wildcard include/config/samples.h) \
    $(wildcard include/config/network/filesystems.h) \
    $(wildcard include/config/module/force/unload.h) \
    $(wildcard include/config/crypto/md4.h) \
    $(wildcard include/config/arch/tegra.h) \
    $(wildcard include/config/ftgmac100.h) \
    $(wildcard include/config/mstar/sdmmc1/gpio/pm16.h) \
    $(wildcard include/config/bt/hcibfusb.h) \
    $(wildcard include/config/net/ethernet.h) \
    $(wildcard include/config/experimental.h) \
    $(wildcard include/config/cgroup/device.h) \
    $(wildcard include/config/ppp/sync/tty.h) \
    $(wildcard include/config/arch/suspend/possible.h) \
    $(wildcard include/config/mfd/tps65090.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/rc/core.h) \
    $(wildcard include/config/mp/compiler/error.h) \
    $(wildcard include/config/hid/speedlink.h) \
    $(wildcard include/config/blk/dev/nbd.h) \
    $(wildcard include/config/arm/unwind.h) \
    $(wildcard include/config/ext4/fs/posix/acl.h) \
    $(wildcard include/config/scsi/debug.h) \
    $(wildcard include/config/mstar/sdmmc1/maxclk.h) \
    $(wildcard include/config/arch/versatile.h) \
    $(wildcard include/config/binfmt/misc.h) \
    $(wildcard include/config/netfilter/xt/match/helper.h) \
    $(wildcard include/config/ssb/possible.h) \
    $(wildcard include/config/nf/nat/sip.h) \
    $(wildcard include/config/sysv68/partition.h) \
    $(wildcard include/config/netfilter/xt/match/statistic.h) \
    $(wildcard include/config/mp/usb/mstar.h) \
    $(wildcard include/config/nls/codepage/855.h) \
    $(wildcard include/config/mfd/t7l66xb.h) \
    $(wildcard include/config/mp/debug/tool/watchdog.h) \
    $(wildcard include/config/video/saa7127.h) \
    $(wildcard include/config/uhid.h) \
    $(wildcard include/config/bridge/nf/ebtables.h) \
    $(wildcard include/config/ibm/new/emac/rgmii.h) \
    $(wildcard include/config/reiserfs/fs.h) \
    $(wildcard include/config/arch/shmobile.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/crypto/rmd128.h) \
    $(wildcard include/config/blk/dev/loop/min/count.h) \
    $(wildcard include/config/stp.h) \
    $(wildcard include/config/usb/gspca/sq905.h) \
    $(wildcard include/config/inet6/tunnel.h) \
    $(wildcard include/config/nf/conntrack/sip.h) \
    $(wildcard include/config/stack/tracer.h) \
    $(wildcard include/config/media/attach.h) \
    $(wildcard include/config/usb/suspend.h) \
    $(wildcard include/config/crypto/manager/disable/tests.h) \
    $(wildcard include/config/hidraw.h) \
    $(wildcard include/config/have/kernel/lzma.h) \
    $(wildcard include/config/hid/prodikeys.h) \
    $(wildcard include/config/ip/nf/queue.h) \
    $(wildcard include/config/mp/nand/spansion/debug.h) \
    $(wildcard include/config/fb/s1d13xxx.h) \
    $(wildcard include/config/dm/flakey.h) \
    $(wildcard include/config/pl310/errata/588369.h) \
    $(wildcard include/config/arch/zynq.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/default/security/dac.h) \
    $(wildcard include/config/fib/rules.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/hid/ems/ff.h) \
    $(wildcard include/config/ktime/scalar.h) \
    $(wildcard include/config/vlan/8021q/gvrp.h) \
    $(wildcard include/config/ip6/nf/mangle.h) \
    $(wildcard include/config/arch/ks8695.h) \
    $(wildcard include/config/scsi/constants.h) \
    $(wildcard include/config/mp/mmap/bufferable/debug.h) \
    $(wildcard include/config/netfilter/xt/match/realm.h) \
    $(wildcard include/config/netdev/1000.h) \
    $(wildcard include/config/bt/hcibpa10x.h) \
    $(wildcard include/config/squashfs/xattr.h) \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/mstar/arm.h) \
    $(wildcard include/config/mp/platform/arch/general/debug.h) \
    $(wildcard include/config/net/9p.h) \
    $(wildcard include/config/crypto/aead.h) \
    $(wildcard include/config/mstar/janus.h) \
    $(wildcard include/config/mp/sd/plug.h) \
    $(wildcard include/config/input/mousedev/psaux.h) \
    $(wildcard include/config/default/tcp/cong.h) \
    $(wildcard include/config/arch/ebsa110.h) \
    $(wildcard include/config/uevent/helper/path.h) \
    $(wildcard include/config/usb/uas.h) \
    $(wildcard include/config/usb/devicefs.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/arch/kirkwood.h) \
    $(wildcard include/config/usb/storage/usbat.h) \
    $(wildcard include/config/mp/android/mstar/rc/map/define.h) \
    $(wildcard include/config/nls/iso8859/7.h) \
    $(wildcard include/config/android/binder/ipc.h) \
    $(wildcard include/config/crypto/pcbc.h) \
    $(wildcard include/config/ir/nec/decoder.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/wlan.h) \
    $(wildcard include/config/affs/fs.h) \
    $(wildcard include/config/namespaces.h) \
    $(wildcard include/config/video/ak881x.h) \
    $(wildcard include/config/inet6/xfrm/mode/routeoptimization.h) \
    $(wildcard include/config/default/message/loglevel.h) \
    $(wildcard include/config/kgdb/serial/console.h) \
    $(wildcard include/config/have/arm/scu.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/security/dmesg/restrict.h) \
    $(wildcard include/config/blk/dev/bsg.h) \
    $(wildcard include/config/connector.h) \
    $(wildcard include/config/netfilter/xt/target/rateest.h) \
    $(wildcard include/config/mstar/oberon.h) \
    $(wildcard include/config/serial/input/default/setup/enable.h) \
    $(wildcard include/config/debug/rt/mutexes.h) \
    $(wildcard include/config/snd/sbawe/seq.h) \
    $(wildcard include/config/legacy/ptys.h) \
    $(wildcard include/config/jfs/fs.h) \
    $(wildcard include/config/xfrm/ipcomp.h) \
    $(wildcard include/config/crypto/rng2.h) \
    $(wildcard include/config/netfilter/netlink/queue.h) \
    $(wildcard include/config/mstar/chip/name.h) \
    $(wildcard include/config/network/phy/timestamping.h) \
    $(wildcard include/config/msdos/fs.h) \
    $(wildcard include/config/tun.h) \
    $(wildcard include/config/usb/gspca/sunplus.h) \
    $(wildcard include/config/mp/ntfs/ioctl/debug.h) \
    $(wildcard include/config/enclosure/services.h) \
    $(wildcard include/config/ipv6/pimsm/v2.h) \
    $(wildcard include/config/mstar/sdmmc/tcard.h) \
    $(wildcard include/config/can.h) \
    $(wildcard include/config/cfg80211.h) \
    $(wildcard include/config/equalizer.h) \
    $(wildcard include/config/nls/codepage/1250.h) \
    $(wildcard include/config/mp/webcam/init/debug.h) \
    $(wildcard include/config/mstar/spi/flash.h) \
    $(wildcard include/config/jbd/debug.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/dm/crypt.h) \
    $(wildcard include/config/serial/8250.h) \
    $(wildcard include/config/rtc/drv/isl12022.h) \
    $(wildcard include/config/bcmdhd.h) \
    $(wildcard include/config/net/tcpprobe.h) \
    $(wildcard include/config/ncp/fs.h) \
    $(wildcard include/config/have/proc/cpu.h) \
    $(wildcard include/config/lzo/decompress.h) \
    $(wildcard include/config/usb/ehci/root/hub/tt.h) \
    $(wildcard include/config/iommu/support.h) \
    $(wildcard include/config/hid/belkin.h) \
    $(wildcard include/config/video/ir/i2c.h) \
    $(wildcard include/config/romfs/backed/by/block.h) \
    $(wildcard include/config/nls/codepage/862.h) \
    $(wildcard include/config/iwm.h) \
    $(wildcard include/config/nfsd.h) \
    $(wildcard include/config/sensors/tsl2550.h) \
    $(wildcard include/config/wireless/ext/sysfs.h) \
    $(wildcard include/config/mp/android/mstar/rc/map/define/debug.h) \
    $(wildcard include/config/hid/acrux.h) \
    $(wildcard include/config/rd/lzma.h) \
    $(wildcard include/config/usb/gspca/se401.h) \
    $(wildcard include/config/backtrace/self/test.h) \
    $(wildcard include/config/crypto/salsa20.h) \
    $(wildcard include/config/usb.h) \
    $(wildcard include/config/mp/miu/mapping/debug.h) \
    $(wildcard include/config/i2c/xilinx.h) \
    $(wildcard include/config/crypto/hmac.h) \
    $(wildcard include/config/bcm4329/rfkill.h) \
    $(wildcard include/config/usb/gspca/jeilinj.h) \
    $(wildcard include/config/arch/mv78xx0.h) \
    $(wildcard include/config/branch/profile/none.h) \
    $(wildcard include/config/scsi/scan/async.h) \
    $(wildcard include/config/crc/itu/t.h) \
    $(wildcard include/config/net/drop/monitor.h) \
    $(wildcard include/config/snd/hrtimer.h) \
    $(wildcard include/config/ftrace/startup/test.h) \
    $(wildcard include/config/framebuffer/console.h) \
    $(wildcard include/config/dm/zero.h) \
    $(wildcard include/config/ip6/nf/target/log.h) \
    $(wildcard include/config/snd/sequencer.h) \
    $(wildcard include/config/mp/nand/ubifs/debug.h) \
    $(wildcard include/config/ext4/debug.h) \
    $(wildcard include/config/arch/mmp.h) \
    $(wildcard include/config/ip/nf/arptables.h) \
    $(wildcard include/config/arm/errata/751472.h) \
    $(wildcard include/config/touchscreen/synaptics/i2c/rmi4.h) \
    $(wildcard include/config/hid/cherry.h) \
    $(wildcard include/config/arm/errata/720789.h) \
    $(wildcard include/config/battery/max17040.h) \
    $(wildcard include/config/accessibility.h) \
    $(wildcard include/config/snd/armaaci.h) \
    $(wildcard include/config/hid/sunplus.h) \
    $(wildcard include/config/input/adxl34x.h) \
    $(wildcard include/config/hid/picolcd.h) \
    $(wildcard include/config/bcma/possible.h) \
    $(wildcard include/config/af/rxrpc.h) \
    $(wildcard include/config/zram.h) \
    $(wildcard include/config/crypto/cast5.h) \
    $(wildcard include/config/usb/ehci/onoff/feature.h) \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/snd/soc.h) \
    $(wildcard include/config/sync.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
    $(wildcard include/config/net/ipgre/demux.h) \
    $(wildcard include/config/media/tuner/xc5000.h) \
    $(wildcard include/config/net/poll/controller.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/nf/conntrack/proc/compat.h) \
    $(wildcard include/config/wifi/control/func.h) \
    $(wildcard include/config/arch/prima2.h) \
    $(wildcard include/config/usb/otg/whitelist.h) \
    $(wildcard include/config/fb/sys/fillrect.h) \
    $(wildcard include/config/timerfd.h) \
    $(wildcard include/config/hid/thrustmaster.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/ethernet/albany.h) \
    $(wildcard include/config/usb/dynamic/minors.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/bcm4330/rfkill.h) \
    $(wildcard include/config/crypto/authenc.h) \
    $(wildcard include/config/mstar/coocaa/auth.h) \
    $(wildcard include/config/usb/storage/ene/ub6250.h) \
    $(wildcard include/config/irda.h) \
    $(wildcard include/config/bounce.h) \
    $(wildcard include/config/zone/normal/miu1.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/mtd.h) \
    $(wildcard include/config/mmc/block/minors.h) \
    $(wildcard include/config/devtmpfs/mount.h) \
    $(wildcard include/config/usb/ipheth.h) \
    $(wildcard include/config/nls/codepage/850.h) \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/mstar/str/acoff/on/err.h) \
    $(wildcard include/config/crypto/cts.h) \
    $(wildcard include/config/input/mousedev.h) \
    $(wildcard include/config/usb/isp1362/hcd.h) \
    $(wildcard include/config/ata.h) \
    $(wildcard include/config/keyboard/sunkbd.h) \
    $(wildcard include/config/mfd/wm8994.h) \
    $(wildcard include/config/crypto/des.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/ceph/lib.h) \
    $(wildcard include/config/mmc/armmmci.h) \
    $(wildcard include/config/nls/codepage/437.h) \
    $(wildcard include/config/cfg80211/internal/regdb.h) \
    $(wildcard include/config/exportfs.h) \
    $(wildcard include/config/fiq/debugger.h) \
    $(wildcard include/config/snd/mixer/oss.h) \
    $(wildcard include/config/ip6/nf/match/mh.h) \
    $(wildcard include/config/scsi/osd/initiator.h) \
    $(wildcard include/config/mp/nand/mtd.h) \
    $(wildcard include/config/arm/gic.h) \
    $(wildcard include/config/serio.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/input/mouse.h) \
    $(wildcard include/config/fb/sys/imageblit.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/netconsole.h) \
    $(wildcard include/config/rtc/intf/sysfs.h) \
    $(wildcard include/config/rtc/drv/rs5c372.h) \
    $(wildcard include/config/blk/dev/initrd.h) \
    $(wildcard include/config/usb/emi62.h) \
    $(wildcard include/config/binfmt/elf/comp.h) \
    $(wildcard include/config/rtc/drv/m48t59.h) \
    $(wildcard include/config/nf/conntrack/sane.h) \
    $(wildcard include/config/snd/ossemul.h) \
    $(wildcard include/config/nf/ct/proto/dccp.h) \
    $(wildcard include/config/zlib/inflate.h) \
    $(wildcard include/config/rtc/drv/v3020.h) \
    $(wildcard include/config/crc/t10dif.h) \
    $(wildcard include/config/bt/tibluesleep.h) \
    $(wildcard include/config/media/tuner/qt1010.h) \
    $(wildcard include/config/debug/ll.h) \
    $(wildcard include/config/hwmon.h) \
    $(wildcard include/config/arm/errata/764369.h) \
    $(wildcard include/config/crypto/twofish/common.h) \
    $(wildcard include/config/nls/iso8859/2.h) \
    $(wildcard include/config/ip/pnp.h) \
    $(wildcard include/config/usb/video/class.h) \
    $(wildcard include/config/rtc/intf/proc.h) \
    $(wildcard include/config/video/vp27smpx.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/mstar/sdmmc.h) \
    $(wildcard include/config/battery/bq27541.h) \
    $(wildcard include/config/crypto/serpent.h) \
    $(wildcard include/config/video/cs5345.h) \
    $(wildcard include/config/stacktrace/support.h) \
    $(wildcard include/config/ibm/new/emac/mal/common/err.h) \
    $(wildcard include/config/rcu/trace.h) \
    $(wildcard include/config/video/m52790.h) \
    $(wildcard include/config/emmc/partition.h) \
    $(wildcard include/config/usb/device/class.h) \
    $(wildcard include/config/lockd.h) \
    $(wildcard include/config/sound/oss/core.h) \
    $(wildcard include/config/blk/dev/cow/common.h) \
    $(wildcard include/config/mstar/oom/do/wdt/reset.h) \
    $(wildcard include/config/usb/sn9c102.h) \
    $(wildcard include/config/libertas.h) \
    $(wildcard include/config/usb/gspca/tv8532.h) \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/video/output/control.h) \
    $(wildcard include/config/arch/clps711x.h) \
    $(wildcard include/config/bt/rfcomm/tty.h) \
    $(wildcard include/config/libfcoe.h) \
    $(wildcard include/config/i2c/designware.h) \
    $(wildcard include/config/display/support.h) \
    $(wildcard include/config/arch/iop33x.h) \
    $(wildcard include/config/arch/u300.h) \
    $(wildcard include/config/mstar/arm/mmap/128mb/128mb.h) \
    $(wildcard include/config/ramoops.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/netfilter/xt/match/string.h) \
    $(wildcard include/config/input/tablet.h) \
    $(wildcard include/config/ip/nf/target/log.h) \
    $(wildcard include/config/media/tuner/max2165.h) \
    $(wildcard include/config/llc2.h) \
    $(wildcard include/config/has/wakelock.h) \
    $(wildcard include/config/logo.h) \
    $(wildcard include/config/mstar/ir/input/device.h) \
    $(wildcard include/config/usb/storage.h) \
    $(wildcard include/config/hmc6352.h) \
    $(wildcard include/config/standalone.h) \
    $(wildcard include/config/crypto/seed.h) \
    $(wildcard include/config/battery/ds2782.h) \
    $(wildcard include/config/mp/debug/tool/changelist/debug.h) \
    $(wildcard include/config/ir/lirc/codec.h) \
    $(wildcard include/config/i2c/mux.h) \
    $(wildcard include/config/mmc/embedded/sdio.h) \
    $(wildcard include/config/console/earlysuspend.h) \
    $(wildcard include/config/snd/usb/audio.h) \
    $(wildcard include/config/video/cx231xx.h) \
    $(wildcard include/config/x25.h) \
    $(wildcard include/config/mp/platform/fixme/debug.h) \
    $(wildcard include/config/ieee802154.h) \
    $(wildcard include/config/mstar/sdmmc/revwp.h) \
    $(wildcard include/config/ashmem.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/have/ide.h) \
    $(wildcard include/config/hid/apple.h) \
    $(wildcard include/config/media/tuner/tda827x.h) \
    $(wildcard include/config/init/env/arg/limit.h) \
    $(wildcard include/config/snd/rawmidi/seq.h) \
    $(wildcard include/config/usb/wusb.h) \
    $(wildcard include/config/rds.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/root/nfs.h) \
    $(wildcard include/config/input/mpu3050.h) \
    $(wildcard include/config/lkdtm.h) \
    $(wildcard include/config/user/ns.h) \
    $(wildcard include/config/nf/conntrack/pptp.h) \
    $(wildcard include/config/hfsplus/fs.h) \
    $(wildcard include/config/tmpfs/posix/acl.h) \
    $(wildcard include/config/snd/usb/ua101.h) \
    $(wildcard include/config/mstar/fb.h) \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/context/switch/tracer.h) \
    $(wildcard include/config/kdb/keyboard.h) \
    $(wildcard include/config/mp/nand/ubi.h) \
    $(wildcard include/config/usb/sl811/hcd.h) \
    $(wildcard include/config/crypto/cast6.h) \
    $(wildcard include/config/mp/bdi/mstar/bdi/patch.h) \
    $(wildcard include/config/i2c/diolan/u2c.h) \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/video/adv7343.h) \
    $(wildcard include/config/pps.h) \
    $(wildcard include/config/charger/max8903.h) \
    $(wildcard include/config/usb/lcd.h) \
    $(wildcard include/config/spi.h) \
    $(wildcard include/config/v4l/mem2mem/drivers.h) \
    $(wildcard include/config/bmp085.h) \
    $(wildcard include/config/tranzport.h) \
    $(wildcard include/config/nf/conntrack/irc.h) \
    $(wildcard include/config/mp/android/alarm/clock/boottime/patch.h) \
    $(wildcard include/config/switch.h) \
    $(wildcard include/config/arch/mxc.h) \
    $(wildcard include/config/devkmem.h) \
    $(wildcard include/config/mp/camera/plug/out.h) \
    $(wildcard include/config/mp/mm/mstar/3dalloc/miu0/1.h) \
    $(wildcard include/config/bootparam/hung/task/panic/value.h) \
    $(wildcard include/config/input/capella/cm3217.h) \
    $(wildcard include/config/usb/stv06xx.h) \
    $(wildcard include/config/ppp/deflate.h) \
    $(wildcard include/config/strip/asm/syms.h) \
    $(wildcard include/config/serio/ambakmi.h) \
    $(wildcard include/config/vt.h) \
    $(wildcard include/config/debug/block/ext/devt.h) \
    $(wildcard include/config/mp/jiffies/print/debug.h) \
    $(wildcard include/config/netfilter/xt/target/classify.h) \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/bt/sco.h) \
    $(wildcard include/config/power/supply.h) \
    $(wildcard include/config/cifs/xattr.h) \
    $(wildcard include/config/cpu/cache/vipt.h) \
    $(wildcard include/config/task/size/3g/less/24m.h) \
    $(wildcard include/config/i2c/pca/platform.h) \
    $(wildcard include/config/netfilter/xt/target/nfqueue.h) \
    $(wildcard include/config/v4l/usb/drivers.h) \
    $(wildcard include/config/wext/core.h) \
    $(wildcard include/config/usb/gspca/mr97310a.h) \
    $(wildcard include/config/video/wm8775.h) \
    $(wildcard include/config/nls.h) \
    $(wildcard include/config/mstar/sdmmc/dualcards.h) \
    $(wildcard include/config/snd/verbose/printk.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/prism2/usb.h) \
    $(wildcard include/config/arch/integrator.h) \
    $(wildcard include/config/power/supply/debug.h) \
    $(wildcard include/config/hid/magicmouse.h) \
    $(wildcard include/config/video/tda9840.h) \
    $(wildcard include/config/mfd/support.h) \
    $(wildcard include/config/pppoe.h) \
    $(wildcard include/config/xfrm/migrate.h) \
    $(wildcard include/config/mstar/none.h) \
    $(wildcard include/config/usb/gspca/spca508.h) \
    $(wildcard include/config/usb/legotower.h) \
    $(wildcard include/config/arch/at91.h) \
    $(wildcard include/config/netfilter/xt/match/owner.h) \
    $(wildcard include/config/sun/partition.h) \
    $(wildcard include/config/ip/nf/target/ecn.h) \
    $(wildcard include/config/mstar/prj/value/hardcode.h) \
    $(wildcard include/config/syn/cookies.h) \
    $(wildcard include/config/irq/work.h) \
    $(wildcard include/config/ip/advanced/router.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/video/usbvision.h) \
    $(wildcard include/config/input/gpio.h) \
    $(wildcard include/config/fb/sys/copyarea.h) \
    $(wildcard include/config/cleancache.h) \
    $(wildcard include/config/usb/storage/alauda.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/media/tuner/tda18271.h) \
    $(wildcard include/config/ip6/nf/iptables.h) \
    $(wildcard include/config/nl80211/testmode.h) \
    $(wildcard include/config/max1749/vibrator.h) \
    $(wildcard include/config/mfd/max8998.h) \
    $(wildcard include/config/inet/ipcomp.h) \
    $(wildcard include/config/kernel/lzma.h) \
    $(wildcard include/config/arch/s3c64xx.h) \
    $(wildcard include/config/mmc/sdhci/pxav2.h) \
    $(wildcard include/config/debug/sg.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/hid/keytouch.h) \
    $(wildcard include/config/hid/cypress.h) \
    $(wildcard include/config/n/gsm.h) \
    $(wildcard include/config/trace/sink.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
    $(wildcard include/config/arch/want/optional/gpiolib.h) \
    $(wildcard include/config/ata/over/eth.h) \
    $(wildcard include/config/video/saa711x.h) \
    $(wildcard include/config/show/pc/lr/info.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/nls/iso8859/1.h) \
    $(wildcard include/config/mfd/wm8400.h) \
    $(wildcard include/config/crypto/workqueue.h) \
    $(wildcard include/config/nfc.h) \
    $(wildcard include/config/atomic64/selftest.h) \
    $(wildcard include/config/sensors/bh1780.h) \
    $(wildcard include/config/hid/kensington.h) \
    $(wildcard include/config/mstar/fw.h) \
    $(wildcard include/config/usb/ehci/hcd.h) \
    $(wildcard include/config/mp/nand/bbt/size/debug.h) \
    $(wildcard include/config/hid/zydacron.h) \
    $(wildcard include/config/ppp/mppe.h) \
    $(wildcard include/config/input/keyreset.h) \
    $(wildcard include/config/snd/pcm/oss.h) \
    $(wildcard include/config/rfkill.h) \
    $(wildcard include/config/video/tvp5150.h) \
    $(wildcard include/config/arch/orion5x.h) \
    $(wildcard include/config/netdevices.h) \
    $(wildcard include/config/net/key.h) \
    $(wildcard include/config/video/adv7180.h) \
    $(wildcard include/config/iosched/deadline.h) \
    $(wildcard include/config/snd/opl3/lib/seq.h) \
    $(wildcard include/config/mstar/alsa/audio/driver/hal.h) \
    $(wildcard include/config/cgroup/freezer.h) \
    $(wildcard include/config/mstar/tsp.h) \
    $(wildcard include/config/video/upd64083.h) \
    $(wildcard include/config/serial/altera/jtaguart.h) \
    $(wildcard include/config/cpu/tlb/v7.h) \
    $(wildcard include/config/eventfd.h) \
    $(wildcard include/config/wan.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/xfrm.h) \
    $(wildcard include/config/defconfig/list.h) \
    $(wildcard include/config/list.h) \
    $(wildcard include/config/fpe/fastfpe.h) \
    $(wildcard include/config/mstar/euclid.h) \
    $(wildcard include/config/usb/mr800.h) \
    $(wildcard include/config/serial/8250/console.h) \
    $(wildcard include/config/ipv6/multiple/tables.h) \
    $(wildcard include/config/usb/announce/new/devices.h) \
    $(wildcard include/config/video/tvp514x.h) \
    $(wildcard include/config/keyboard/qt1070.h) \
    $(wildcard include/config/ip/nf/target/masquerade.h) \
    $(wildcard include/config/wan/router.h) \
    $(wildcard include/config/nf/conntrack/broadcast.h) \
    $(wildcard include/config/ir/rc5/sz/decoder.h) \
    $(wildcard include/config/proc/page/monitor.h) \
    $(wildcard include/config/serial/8250/extended.h) \
    $(wildcard include/config/cc/optimize/for/size.h) \
    $(wildcard include/config/tracedump.h) \
    $(wildcard include/config/arm/errata/754327.h) \
    $(wildcard include/config/usb/video/class/input/evdev.h) \
    $(wildcard include/config/c2port.h) \
    $(wildcard include/config/dm/delay.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/usb/ftdi/elan.h) \
    $(wildcard include/config/rd/lzo.h) \
    $(wildcard include/config/perf/counters.h) \
    $(wildcard include/config/android/low/memory/killer.h) \
    $(wildcard include/config/arch/has/cpu/idle/wait.h) \
    $(wildcard include/config/usb/iowarrior.h) \
    $(wildcard include/config/mp/mm/mali/mmu/notifier.h) \
    $(wildcard include/config/android/ram/console/enable/verbose.h) \
    $(wildcard include/config/arch/davinci.h) \
    $(wildcard include/config/scsi/spi/attrs.h) \
    $(wildcard include/config/econet.h) \
    $(wildcard include/config/cfg80211/developer/warnings.h) \
    $(wildcard include/config/crypto/sha512.h) \
    $(wildcard include/config/phonet.h) \
    $(wildcard include/config/scsi/wait/scan.h) \
    $(wildcard include/config/usb/m5602.h) \
    $(wildcard include/config/crypto/null.h) \
    $(wildcard include/config/nf/defrag/ipv4.h) \
    $(wildcard include/config/usb/idmouse.h) \
    $(wildcard include/config/select/memory/model.h) \
    $(wildcard include/config/inet/xfrm/mode/beet.h) \
    $(wildcard include/config/hid/lcpower.h) \
    $(wildcard include/config/mmc/unsafe/resume.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
    $(wildcard include/config/mp/nand/bbt.h) \
    $(wildcard include/config/bfs/fs.h) \
    $(wildcard include/config/pm/debug.h) \
    $(wildcard include/config/netfilter/advanced.h) \
    $(wildcard include/config/crypto/deflate.h) \
    $(wildcard include/config/mfd/wm8350/i2c.h) \
    $(wildcard include/config/usb/appledisplay.h) \
    $(wildcard include/config/netfilter/xt/match/tcpmss.h) \
    $(wildcard include/config/ipv6/router/pref.h) \
    $(wildcard include/config/bt/hcibtsdio.h) \
    $(wildcard include/config/cmdline/extend.h) \
    $(wildcard include/config/crypto/gcm.h) \
    $(wildcard include/config/netfilter/netlink/log.h) \
    $(wildcard include/config/have/dynamic/ftrace.h) \
    $(wildcard include/config/nls/iso8859/14.h) \
    $(wildcard include/config/magic/sysrq.h) \
    $(wildcard include/config/mfd/88pm860x.h) \
    $(wildcard include/config/arch/pnx4008.h) \
    $(wildcard include/config/arch/rpc.h) \
    $(wildcard include/config/input/lid.h) \
    $(wildcard include/config/thumb2/kernel.h) \
    $(wildcard include/config/ipv6/sit/6rd.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/netfilter/xt/match/mark.h) \
    $(wildcard include/config/isdn.h) \
    $(wildcard include/config/function/profiler.h) \
    $(wildcard include/config/sparse/irq.h) \
    $(wildcard include/config/netfilter/xt/target/tcpoptstrip.h) \
    $(wildcard include/config/have/arm/twd.h) \
    $(wildcard include/config/ip/nf/mangle.h) \
    $(wildcard include/config/rtc/drv/ds1286.h) \
    $(wildcard include/config/securityfs.h) \
    $(wildcard include/config/default/cfq.h) \
    $(wildcard include/config/inet6/xfrm/mode/tunnel.h) \
    $(wildcard include/config/media/support.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/i2c/debug/bus.h) \
    $(wildcard include/config/ip/nf/filter.h) \
    $(wildcard include/config/hid/zeroplus.h) \
    $(wildcard include/config/ext3/fs.h) \
    $(wildcard include/config/mp/platform/arm.h) \
    $(wildcard include/config/mstar/triton.h) \
    $(wildcard include/config/netfilter/xt/match/length.h) \
    $(wildcard include/config/mfd/tc6387xb.h) \
    $(wildcard include/config/fat/fs.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/ip6/nf/raw.h) \
    $(wildcard include/config/usb/zd1201.h) \
    $(wildcard include/config/rtc/drv/ds1742.h) \
    $(wildcard include/config/inet/tunnel.h) \
    $(wildcard include/config/netfilter/xt/target/notrack.h) \
    $(wildcard include/config/eeprom/93cx6.h) \
    $(wildcard include/config/mmc/block/bounce.h) \
    $(wildcard include/config/generic/clockevents.h) \
    $(wildcard include/config/romfs/fs.h) \
    $(wildcard include/config/iosched/cfq.h) \
    $(wildcard include/config/lib80211.h) \
    $(wildcard include/config/mfd/core.h) \
    $(wildcard include/config/mstar/ir/multiple/mode.h) \
    $(wildcard include/config/cpu/cp15/mmu.h) \
    $(wildcard include/config/serial/amba/pl011.h) \
    $(wildcard include/config/raid/attrs.h) \
    $(wildcard include/config/mp/platform/fixme.h) \
    $(wildcard include/config/console/translations.h) \
    $(wildcard include/config/phone.h) \
    $(wildcard include/config/stop/machine.h) \
    $(wildcard include/config/hw/random/timeriomem.h) \
    $(wildcard include/config/video/helper/chips/auto.h) \
    $(wildcard include/config/video/tea6420.h) \
    $(wildcard include/config/nf/nat/snmp/basic.h) \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/mp/remote/control/rc/register/retry.h) \
    $(wildcard include/config/usb/gspca.h) \
    $(wildcard include/config/mstar/alsa/audio/driver.h) \
    $(wildcard include/config/dm/mirror.h) \
    $(wildcard include/config/crypto/blowfish.h) \
    $(wildcard include/config/dummy/console.h) \
    $(wildcard include/config/module/force/load.h) \
    $(wildcard include/config/nls/ascii.h) \
    $(wildcard include/config/ibm/new/emac/mal/clr/icintstat.h) \
    $(wildcard include/config/usb/rio500.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/debug/perf/use/vmalloc.h) \
    $(wildcard include/config/nfs/v3/acl.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/crypto/ccm.h) \
    $(wildcard include/config/tcp/cong/advanced.h) \
    $(wildcard include/config/usb/s2255.h) \
    $(wildcard include/config/netfilter/xt/match/connmark.h) \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/raw/driver.h) \
    $(wildcard include/config/video/ks0127.h) \
    $(wildcard include/config/snd/usb.h) \
    $(wildcard include/config/rd/gzip.h) \
    $(wildcard include/config/have/regs/and/stack/access/api.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/snd/usb/caiaq.h) \
    $(wildcard include/config/usb/storage/cypress/atacb.h) \
    $(wildcard include/config/ext4/fs/security.h) \
    $(wildcard include/config/befs/fs.h) \
    $(wildcard include/config/hid/roccat.h) \
    $(wildcard include/config/inet/xfrm/mode/transport.h) \
    $(wildcard include/config/crypto/md5.h) \
    $(wildcard include/config/video/cs53l32a.h) \
    $(wildcard include/config/ir/redrat3.h) \
    $(wildcard include/config/usb/gspca/vicam.h) \
    $(wildcard include/config/mp/sysattr/show/debug.h) \
    $(wildcard include/config/iscsi/tcp.h) \
    $(wildcard include/config/mp/sd/mstar/debug.h) \
    $(wildcard include/config/nfsd/v3.h) \
    $(wildcard include/config/debug/user.h) \
    $(wildcard include/config/mfd/wl1273/core.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/media/tuner/tea5767.h) \
    $(wildcard include/config/batman/adv.h) \
    $(wildcard include/config/oabi/compat.h) \
    $(wildcard include/config/pccard.h) \
    $(wildcard include/config/have/generic/hardirqs.h) \
    $(wildcard include/config/usb/gspca/ov519.h) \
    $(wildcard include/config/binfmt/elf.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/scsi/proc/fs.h) \
    $(wildcard include/config/arch/mxs.h) \
    $(wildcard include/config/hotplug.h) \
    $(wildcard include/config/sched/tracer.h) \
    $(wildcard include/config/udf/nls.h) \
    $(wildcard include/config/mstar/nand.h) \
    $(wildcard include/config/ip/pimsm/v1.h) \
    $(wildcard include/config/inet6/ah.h) \
    $(wildcard include/config/cpu/cp15.h) \
    $(wildcard include/config/target/core.h) \
    $(wildcard include/config/mfd/max77663.h) \
    $(wildcard include/config/usb/serial.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/usb/mon.h) \
    $(wildcard include/config/vgastate.h) \
    $(wildcard include/config/mp/platform/arm/pmu.h) \
    $(wildcard include/config/video/saa717x.h) \
    $(wildcard include/config/ip/nf/target/ulog.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/inet/xfrm/tunnel.h) \
    $(wildcard include/config/rtc/drv/fm3130.h) \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/netfilter/xt/mark.h) \
    $(wildcard include/config/netfilter/xtables.h) \
    $(wildcard include/config/vmsplit/1g.h) \
    $(wildcard include/config/resource/counters.h) \
    $(wildcard include/config/usb/isp1760/hcd.h) \
    $(wildcard include/config/mfd/stmpe.h) \
    $(wildcard include/config/keyboard/stowaway.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
    $(wildcard include/config/usb/storage/datafab.h) \
    $(wildcard include/config/mfd/rc5t583.h) \
    $(wildcard include/config/debug/highmem.h) \
    $(wildcard include/config/crypto/user/api/hash.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/boot/printk/delay.h) \
    $(wildcard include/config/pm/sleep/smp.h) \
    $(wildcard include/config/crypto/hw.h) \
    $(wildcard include/config/ion.h) \
    $(wildcard include/config/usb/storage/karma.h) \
    $(wildcard include/config/use/of.h) \
    $(wildcard include/config/hid/greenasia.h) \
    $(wildcard include/config/hardirqs/sw/resend.h) \
    $(wildcard include/config/dcc/tty.h) \
    $(wildcard include/config/vt/hw/console/binding.h) \
    $(wildcard include/config/arch/bcmring.h) \
    $(wildcard include/config/charger/smb347.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/hid/gyration.h) \
    $(wildcard include/config/debug/kobject.h) \
    $(wildcard include/config/usb/wdm.h) \
    $(wildcard include/config/mfd/tmio.h) \
    $(wildcard include/config/bt/hcibcm203x.h) \
    $(wildcard include/config/mp/platform/arm/pmu/debug.h) \
    $(wildcard include/config/ax88796.h) \
    $(wildcard include/config/input/joydev.h) \
    $(wildcard include/config/keyboard/lkkbd.h) \
    $(wildcard include/config/netfilter/xt/target/tproxy.h) \
    $(wildcard include/config/ip/dccp.h) \
    $(wildcard include/config/earlysuspend.h) \
    $(wildcard include/config/usb/acm.h) \
    $(wildcard include/config/usb/rtl8150.h) \
    $(wildcard include/config/cmdline/force.h) \
    $(wildcard include/config/crc16.h) \
    $(wildcard include/config/debug/shirq.h) \
    $(wildcard include/config/generic/calibrate/delay.h) \
    $(wildcard include/config/crypto/gf128mul.h) \
    $(wildcard include/config/nls/iso8859/6.h) \
    $(wildcard include/config/cpu/has/pmu.h) \
    $(wildcard include/config/battery/bq20z75.h) \
    $(wildcard include/config/tmpfs.h) \
    $(wildcard include/config/msatr/ve/capture/support.h) \
    $(wildcard include/config/hostap.h) \
    $(wildcard include/config/anon/inodes.h) \
    $(wildcard include/config/video/em28xx.h) \
    $(wildcard include/config/nls/codepage/936.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/crypto/anubis.h) \
    $(wildcard include/config/ip/pnp/dhcp.h) \
    $(wildcard include/config/usb/gspca/sq930x.h) \
    $(wildcard include/config/mstar/ir.h) \
    $(wildcard include/config/netfilter/xt/target/hl.h) \
    $(wildcard include/config/dvb/core.h) \
    $(wildcard include/config/vmsplit/3g.h) \
    $(wildcard include/config/isl29003.h) \
    $(wildcard include/config/rtc/hctosys.h) \
    $(wildcard include/config/i2c/ocores.h) \
    $(wildcard include/config/serial/core/console.h) \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/cifs/weak/pw/hash.h) \
    $(wildcard include/config/usb/hid.h) \
    $(wildcard include/config/mp/sd/plug/debug.h) \
    $(wildcard include/config/mstar/g3d.h) \
    $(wildcard include/config/crypto/tgr192.h) \
    $(wildcard include/config/blk/dev/rbd.h) \
    $(wildcard include/config/serio/ps2mult.h) \
    $(wildcard include/config/android.h) \
    $(wildcard include/config/slub/debug.h) \
    $(wildcard include/config/blk/dev/md.h) \
    $(wildcard include/config/mmc/vub300.h) \
    $(wildcard include/config/bcma.h) \
    $(wildcard include/config/dmadevices.h) \
    $(wildcard include/config/nf/conntrack/events.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/hfs/fs.h) \
    $(wildcard include/config/rtc/drv/msm6242.h) \
    $(wildcard include/config/crypto/zlib.h) \
    $(wildcard include/config/nls/codepage/1251.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/test/list/sort.h) \
    $(wildcard include/config/smc911x.h) \
    $(wildcard include/config/altera/stapl.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/crypto/pcomp2.h) \
    $(wildcard include/config/mp/scsi/mstar/sd/card/hotplug.h) \
    $(wildcard include/config/smsc911x.h) \
    $(wildcard include/config/mp/ntfs/volume/id.h) \
    $(wildcard include/config/nls/codepage/863.h) \
    $(wildcard include/config/nf/conntrack/ftp.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/ip/nf/match/ecn.h) \
    $(wildcard include/config/cpu/has/asid.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/usb/gadget.h) \
    $(wildcard include/config/eeprom/legacy.h) \
    $(wildcard include/config/usb/gspca/spca506.h) \
    $(wildcard include/config/keyboard/newton.h) \
    $(wildcard include/config/i2c/versatile.h) \
    $(wildcard include/config/hardlockup/detector.h) \
    $(wildcard include/config/media/tuner/mxl5005s.h) \
    $(wildcard include/config/usb/hiddev.h) \
    $(wildcard include/config/mfd/tc3589x.h) \
    $(wildcard include/config/sensors/apds990x.h) \
    $(wildcard include/config/sound.h) \
    $(wildcard include/config/event/power/tracing/deprecated.h) \
    $(wildcard include/config/joliet.h) \
    $(wildcard include/config/media/tuner/tda9887.h) \
    $(wildcard include/config/crypto/tea.h) \
    $(wildcard include/config/have/pata/platform.h) \
    $(wildcard include/config/unix.h) \
    $(wildcard include/config/usb/catc.h) \
    $(wildcard include/config/arch/pxa.h) \
    $(wildcard include/config/mp/platform/arch/general.h) \
    $(wildcard include/config/minix/fs.h) \
    $(wildcard include/config/have/clk.h) \
    $(wildcard include/config/crypto/hash2.h) \
    $(wildcard include/config/default/hostname.h) \
    $(wildcard include/config/nls/koi8/r.h) \
    $(wildcard include/config/usb/gspca/sonixb.h) \
    $(wildcard include/config/usb/storage/isd200.h) \
    $(wildcard include/config/write/to/android/misc/emmc.h) \
    $(wildcard include/config/arm/errata/743622.h) \
    $(wildcard include/config/usb/gspca/ov534/9.h) \
    $(wildcard include/config/netdev/10000.h) \
    $(wildcard include/config/nfs/fs.h) \
    $(wildcard include/config/crash/dump.h) \
    $(wildcard include/config/arch/exynos4.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/arch/vexpress/ca9x4.h) \
    $(wildcard include/config/xps.h) \
    $(wildcard include/config/inet/esp.h) \
    $(wildcard include/config/mstar/coredump/reboot.h) \
    $(wildcard include/config/hid/quanta.h) \
    $(wildcard include/config/nf/conntrack/ipv6.h) \
    $(wildcard include/config/md.h) \
    $(wildcard include/config/crypto/algapi.h) \
    $(wildcard include/config/input/kxtj9.h) \
    $(wildcard include/config/bridge.h) \
    $(wildcard include/config/media/tuner.h) \
    $(wildcard include/config/misc/devices.h) \
    $(wildcard include/config/usb/cypress/cy7c63.h) \
    $(wildcard include/config/mstar/sdmmc1/gpio/pm10.h) \
    $(wildcard include/config/rd/bzip2.h) \
    $(wildcard include/config/blk/dev/xip.h) \
    $(wildcard include/config/input/uinput.h) \
    $(wildcard include/config/netfilter/xt/target/checksum.h) \
    $(wildcard include/config/arch/supports/msi.h) \
    $(wildcard include/config/media/tuner/simple.h) \
    $(wildcard include/config/keyboard/atkbd.h) \
    $(wildcard include/config/mstar/sdmmc1/maxdlvl.h) \
    $(wildcard include/config/arm/errata/742230.h) \
    $(wildcard include/config/nf/nat.h) \
    $(wildcard include/config/video/upd64031a.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
    $(wildcard include/config/cpu/idle.h) \
    $(wildcard include/config/nfs/common.h) \
    $(wildcard include/config/chr/dev/sch.h) \
    $(wildcard include/config/zone/normal/miu0.h) \
    $(wildcard include/config/mp/nand/ubifs.h) \
    $(wildcard include/config/regulator.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/crypto/hash.h) \
    $(wildcard include/config/efi/partition.h) \
    $(wildcard include/config/mstar/kernel/panic/do/wdt/reset.h) \
    $(wildcard include/config/log/buf/shift.h) \
    $(wildcard include/config/old/mcount.h) \
    $(wildcard include/config/serial/input/enable/help/msg.h) \
    $(wildcard include/config/nls/codepage/857.h) \
    $(wildcard include/config/sound/oss/core/preclaim.h) \
    $(wildcard include/config/netfilter/xt/target/dscp.h) \
    $(wildcard include/config/extra/firmware.h) \
    $(wildcard include/config/tcg/tpm.h) \
    $(wildcard include/config/cache/l2x0.h) \
    $(wildcard include/config/bootparam/hung/task/panic.h) \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/proc/events.h) \
    $(wildcard include/config/virt/to/bus.h) \
    $(wildcard include/config/vfat/fs.h) \
    $(wildcard include/config/pid/ns.h) \
    $(wildcard include/config/crypto/vmac.h) \
    $(wildcard include/config/kexec.h) \
    $(wildcard include/config/radio/tef6862.h) \
    $(wildcard include/config/fb/ddc.h) \
    $(wildcard include/config/mp/sysattr/show.h) \
    $(wildcard include/config/mp/debug/tool/codedump/debug.h) \
    $(wildcard include/config/video/hdpvr.h) \
    $(wildcard include/config/romfs/on/block.h) \
    $(wildcard include/config/plat/versatile.h) \
    $(wildcard include/config/blk/dev/sr.h) \
    $(wildcard include/config/cpu/rmap.h) \
    $(wildcard include/config/snd/hwdep.h) \
    $(wildcard include/config/blk/dev/loop.h) \
    $(wildcard include/config/ipmi/handler.h) \
    $(wildcard include/config/b44.h) \
    $(wildcard include/config/default/hung/task/timeout.h) \
    $(wildcard include/config/sysv/fs.h) \
    $(wildcard include/config/mp/emmc/partition.h) \
    $(wildcard include/config/wakelock.h) \
    $(wildcard include/config/nf/nat/irc.h) \
    $(wildcard include/config/media/tuner/customise.h) \
    $(wildcard include/config/rtc/drv/pl031.h) \
    $(wildcard include/config/media/tuner/xc2028.h) \
    $(wildcard include/config/input/misc.h) \
    $(wildcard include/config/auto/zreladdr.h) \
    $(wildcard include/config/dnet.h) \
    $(wildcard include/config/cpu/pabrt/v7.h) \
    $(wildcard include/config/soc/camera.h) \
    $(wildcard include/config/suspend.h) \
    $(wildcard include/config/crypto/cbc.h) \
    $(wildcard include/config/media/controller.h) \
    $(wildcard include/config/bcm/wimax.h) \
    $(wildcard include/config/cpu/bpredict/disable.h) \
    $(wildcard include/config/input/mma8450.h) \
    $(wildcard include/config/ip6/nf/match/rt.h) \
    $(wildcard include/config/serial/8250/runtime/uarts.h) \
    $(wildcard include/config/usb/otg/wakelock.h) \
    $(wildcard include/config/kprobes.h) \
    $(wildcard include/config/fs/mbcache.h) \
    $(wildcard include/config/ds1682.h) \
    $(wildcard include/config/gfs2/fs.h) \
    $(wildcard include/config/rtc/class.h) \
    $(wildcard include/config/crc7.h) \
    $(wildcard include/config/usb/emi26.h) \
    $(wildcard include/config/arch/shark.h) \
    $(wildcard include/config/tmpfs/xattr.h) \
    $(wildcard include/config/fb/uvesa.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/fpe/nwfpe/xp.h) \
    $(wildcard include/config/mp/remote/control/rc/register/retry/debug.h) \
    $(wildcard include/config/w1.h) \
    $(wildcard include/config/arm/amba.h) \
    $(wildcard include/config/logfs.h) \
    $(wildcard include/config/usb/gspca/vc032x.h) \
    $(wildcard include/config/emac/supply/rng.h) \
    $(wildcard include/config/mstar/semutex.h) \
    $(wildcard include/config/arch/s3c2410.h) \
    $(wildcard include/config/android/ram/console/early/init.h) \
    $(wildcard include/config/rtc/drv/ds1374.h) \
    $(wildcard include/config/rtc/drv/bq4802.h) \
    $(wildcard include/config/generic/tracer.h) \
    $(wildcard include/config/nls/iso8859/5.h) \
    $(wildcard include/config/have/function/tracer.h) \
    $(wildcard include/config/nf/nat/tftp.h) \
    $(wildcard include/config/netfilter/xt/match/multiport.h) \
    $(wildcard include/config/outer/cache.h) \
    $(wildcard include/config/usb/storage/realtek.h) \
    $(wildcard include/config/crypto/camellia.h) \
    $(wildcard include/config/snd/emu10k1/seq.h) \
    $(wildcard include/config/cpu/cache/v7.h) \
    $(wildcard include/config/rtc/drv/ds1307.h) \
    $(wildcard include/config/arch/select/memory/model.h) \
    $(wildcard include/config/nfsd/v4.h) \
    $(wildcard include/config/i2c/si4713.h) \
    $(wildcard include/config/mp/core/path/debug.h) \
    $(wildcard include/config/crypto/manager2.h) \
    $(wildcard include/config/mstar/str/crc.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/configfs/fs.h) \
    $(wildcard include/config/crypto/test.h) \
    $(wildcard include/config/mstar/msglog.h) \
    $(wildcard include/config/xz/dec/bcj.h) \
    $(wildcard include/config/slip.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/i2c.h) \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/ppp/multilink.h) \
    $(wildcard include/config/mp/mstar/str/base/debug.h) \
    $(wildcard include/config/arch/dove.h) \
    $(wildcard include/config/video/cx25840.h) \
    $(wildcard include/config/video/adv7175.h) \
    $(wildcard include/config/scsi/srp/attrs.h) \
    $(wildcard include/config/frame/pointer.h) \
    $(wildcard include/config/bt/hidp.h) \
    $(wildcard include/config/ks8851/mll.h) \
    $(wildcard include/config/cpu/abrt/ev7.h) \
    $(wildcard include/config/ufs/fs.h) \
    $(wildcard include/config/report/present/cpus.h) \
    $(wildcard include/config/vm/event/counters.h) \
    $(wildcard include/config/relay.h) \
    $(wildcard include/config/pohmelfs.h) \
    $(wildcard include/config/usb/gspca/finepix.h) \
    $(wildcard include/config/crypto/ecb.h) \
    $(wildcard include/config/usb/gspca/etoms.h) \
    $(wildcard include/config/mstp.h) \
    $(wildcard include/config/squashfs/lzo.h) \
    $(wildcard include/config/adj/miu01.h) \
    $(wildcard include/config/snd/dynamic/minors.h) \
    $(wildcard include/config/video/saa7191.h) \
    $(wildcard include/config/usb/libusual.h) \
    $(wildcard include/config/i2c/slave.h) \
    $(wildcard include/config/nf/conntrack/amanda.h) \
    $(wildcard include/config/debug/fs.h) \
    $(wildcard include/config/arm/thumbee.h) \
    $(wildcard include/config/arch/tcc/926.h) \
    $(wildcard include/config/mp/android/alarm/clock/boottime/patch/debug.h) \
    $(wildcard include/config/net/pktgen.h) \
    $(wildcard include/config/base/full.h) \
    $(wildcard include/config/fb/cfb/imageblit.h) \
    $(wildcard include/config/zlib/deflate.h) \
    $(wildcard include/config/solaris/x86/partition.h) \
    $(wildcard include/config/sunrpc.h) \
    $(wildcard include/config/pmic/da903x.h) \
    $(wildcard include/config/cifs/debug2.h) \
    $(wildcard include/config/input/ati/remote2.h) \
    $(wildcard include/config/osf/partition.h) \
    $(wildcard include/config/nls/codepage/864.h) \
    $(wildcard include/config/mp/mips/l2/catch/debug.h) \
    $(wildcard include/config/mstar/gpio.h) \
    $(wildcard include/config/plat/versatile/sched/clock.h) \
    $(wildcard include/config/usb/yurex.h) \
    $(wildcard include/config/fw/loader.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/usb/gspca/spca501.h) \
    $(wildcard include/config/sw/sync/user.h) \
    $(wildcard include/config/rtc/hctosys/device.h) \
    $(wildcard include/config/crypto/xts.h) \
    $(wildcard include/config/mstar/arm/bd/generic.h) \
    $(wildcard include/config/netfilter/xt/match/recent.h) \
    $(wildcard include/config/netfilter/xt/match/pkttype.h) \
    $(wildcard include/config/mii.h) \
    $(wildcard include/config/signalfd.h) \
    $(wildcard include/config/ip/nf/target/reject/skerr.h) \
    $(wildcard include/config/video/cpia2.h) \
    $(wildcard include/config/ext4/fs.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/kretprobes.h) \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/crypto/sha1.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
    $(wildcard include/config/suspend/time.h) \
    $(wildcard include/config/mp/debug/tool/kdebug/debug.h) \
    $(wildcard include/config/arch/s5p64x0.h) \
    $(wildcard include/config/mstar/pm/swir.h) \
    $(wildcard include/config/ipv6/privacy.h) \
    $(wildcard include/config/usb/adutux.h) \
    $(wildcard include/config/arch/h720x.h) \
    $(wildcard include/config/crc8.h) \
    $(wildcard include/config/nls/koi8/u.h) \
    $(wildcard include/config/xz/dec.h) \
    $(wildcard include/config/ath/common.h) \
    $(wildcard include/config/mstar/gmac.h) \
    $(wildcard include/config/lockd/v4.h) \
    $(wildcard include/config/dummy.h) \
    $(wildcard include/config/tz/support.h) \
    $(wildcard include/config/coda/fs.h) \
    $(wildcard include/config/mp/ntfs/ioctl.h) \
    $(wildcard include/config/battery/max17042.h) \
    $(wildcard include/config/watchdog.h) \
    $(wildcard include/config/has/iomem.h) \
    $(wildcard include/config/ring/buffer/allow/swap.h) \
    $(wildcard include/config/snd/rawmidi.h) \
    $(wildcard include/config/pppopns.h) \
    $(wildcard include/config/mstar/titania.h) \
    $(wildcard include/config/virt/drivers.h) \
    $(wildcard include/config/kernel/lzo.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/cpu/icache/disable.h) \
    $(wildcard include/config/usb/storage/sddr09.h) \
    $(wildcard include/config/ip/nf/match/ttl.h) \
    $(wildcard include/config/keyboard/mcs.h) \
    $(wildcard include/config/xvmalloc.h) \
    $(wildcard include/config/ibm/new/emac/no/flow/ctrl.h) \
    $(wildcard include/config/netfilter/xt/target/trace.h) \
    $(wildcard include/config/rtc/drv/x1205.h) \
    $(wildcard include/config/mstar/mbx.h) \
    $(wildcard include/config/ip6/nf/match/frag.h) \
    $(wildcard include/config/apm/emulation.h) \
    $(wildcard include/config/mp/nand/mtd/debug.h) \
    $(wildcard include/config/mstar/sdmmc1/passlvl.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/crypto/lzo.h) \
    $(wildcard include/config/snd/pcm.h) \
    $(wildcard include/config/pm/runtime.h) \
    $(wildcard include/config/tracelevel.h) \
    $(wildcard include/config/easycap.h) \
    $(wildcard include/config/sdio/uart.h) \
    $(wildcard include/config/partition/advanced.h) \
    $(wildcard include/config/netfilter/xt/match/dccp.h) \
    $(wildcard include/config/mp/nand/bbt/size.h) \
    $(wildcard include/config/debug/section/mismatch.h) \
    $(wildcard include/config/unixware/disklabel.h) \
    $(wildcard include/config/bt/bnep/mc/filter.h) \
    $(wildcard include/config/scsi/iscsi/attrs.h) \
    $(wildcard include/config/keyboard/max7359.h) \
    $(wildcard include/config/ir/sony/decoder.h) \
    $(wildcard include/config/nls/codepage/852.h) \
    $(wildcard include/config/video/pvrusb2.h) \
    $(wildcard include/config/serial/xilinx/ps/uart.h) \
    $(wildcard include/config/netfilter/xt/match/comment.h) \
    $(wildcard include/config/usb/isp116x/hcd.h) \
    $(wildcard include/config/mp/platform/pm/debug.h) \
    $(wildcard include/config/net.h) \
    $(wildcard include/config/atari/partition.h) \
    $(wildcard include/config/input/evdev.h) \
    $(wildcard include/config/netfilter/xt/target/tcpmss.h) \
    $(wildcard include/config/have/sparse/irq.h) \
    $(wildcard include/config/ext2/fs.h) \
    $(wildcard include/config/netfilter/xt/match/dscp.h) \
    $(wildcard include/config/crypto/wp512.h) \
    $(wildcard include/config/hid/dragonrise.h) \
    $(wildcard include/config/netfilter/xt/match/conntrack.h) \
    $(wildcard include/config/netfilter/xt/match/rateest.h) \
    $(wildcard include/config/mmc/paranoid/sd/init.h) \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/snd/aloop.h) \
    $(wildcard include/config/hpfs/fs.h) \
    $(wildcard include/config/usb/ld.h) \
    $(wildcard include/config/bootparam/hardlockup/panic.h) \
    $(wildcard include/config/mstar/onenand.h) \
    $(wildcard include/config/packet.h) \
    $(wildcard include/config/module/srcversion/all.h) \
    $(wildcard include/config/rtc/drv/m48t86.h) \
    $(wildcard include/config/nf/conntrack/timestamp.h) \
    $(wildcard include/config/arm/errata/753970.h) \
    $(wildcard include/config/netfilter/xt/match/iprange.h) \
    $(wildcard include/config/i2c/tiny/usb.h) \
    $(wildcard include/config/usb/gspca/ov534.h) \
    $(wildcard include/config/keyboard/tca6416.h) \
    $(wildcard include/config/nf/conntrack/tftp.h) \
    $(wildcard include/config/mp/platform/mstar/legancy/intr.h) \
    $(wildcard include/config/nfs/v3.h) \
    $(wildcard include/config/mfd/max8997.h) \
    $(wildcard include/config/arch/lpc32xx.h) \
    $(wildcard include/config/nls/codepage/949.h) \
    $(wildcard include/config/nop/tracer.h) \
    $(wildcard include/config/backlight/lcd/support.h) \
    $(wildcard include/config/isl29020.h) \
    $(wildcard include/config/inet.h) \
    $(wildcard include/config/crypto/pcrypt.h) \
    $(wildcard include/config/i2c/pxa/pci.h) \
    $(wildcard include/config/sensors/ak8975.h) \
    $(wildcard include/config/ip/pnp/bootp.h) \
    $(wildcard include/config/mmc/sdhci/pxav3.h) \
    $(wildcard include/config/netfilter/xt/match/addrtype.h) \
    $(wildcard include/config/prevent/firmware/build.h) \
    $(wildcard include/config/crypto/twofish.h) \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/usb/test.h) \
    $(wildcard include/config/bt.h) \
    $(wildcard include/config/deprecated/param/struct.h) \
    $(wildcard include/config/bt/hcivhci.h) \
    $(wildcard include/config/input/cm109.h) \
    $(wildcard include/config/have/mach/clkdev.h) \
    $(wildcard include/config/usb/switch/fsa9480.h) \
    $(wildcard include/config/mstar/msgio.h) \
    $(wildcard include/config/parport.h) \
    $(wildcard include/config/arm/errata/752520.h) \
    $(wildcard include/config/headers/check.h) \
    $(wildcard include/config/hid/wacom.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/rtc/lib.h) \
    $(wildcard include/config/netfilter/xt/match/policy.h) \
    $(wildcard include/config/kgdb/tests.h) \
    $(wildcard include/config/have/kprobes.h) \
    $(wildcard include/config/netfilter/xt/target/tee.h) \
    $(wildcard include/config/crypto/aes.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/gpiolib.h) \
    $(wildcard include/config/ssb.h) \
    $(wildcard include/config/mmc/dw.h) \
    $(wildcard include/config/i2c/stub.h) \
    $(wildcard include/config/radio/tea5764.h) \
    $(wildcard include/config/ft1000.h) \
    $(wildcard include/config/usb/gspca/spca1528.h) \
    $(wildcard include/config/fb/svgalib.h) \
    $(wildcard include/config/hid/pid.h) \
    $(wildcard include/config/arch/ep93xx.h) \
    $(wildcard include/config/gameport.h) \
    $(wildcard include/config/iso9660/fs.h) \
    $(wildcard include/config/rtc/drv/ds3232.h) \
    $(wildcard include/config/swp/emulate.h) \
    $(wildcard include/config/clksrc/mmio.h) \
    $(wildcard include/config/uio.h) \
    $(wildcard include/config/snd/arm.h) \
    $(wildcard include/config/hid/waltop.h) \
    $(wildcard include/config/video/adv7170.h) \
    $(wildcard include/config/nf/conntrack/mark.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/rc/loopback.h) \
    $(wildcard include/config/mstar/titania2.h) \
    $(wildcard include/config/netfilter/xt/match/hashlimit.h) \
    $(wildcard include/config/usb/r8a66597/hcd.h) \
    $(wildcard include/config/mp/platform/mips/debug.h) \
    $(wildcard include/config/netfilter/xt/match/cluster.h) \
    $(wildcard include/config/squashfs/embedded.h) \
    $(wildcard include/config/use/generic/smp/helpers.h) \
    $(wildcard include/config/usb/mdc800.h) \
    $(wildcard include/config/serio/serport.h) \
    $(wildcard include/config/fb/boot/vesa/support.h) \
    $(wildcard include/config/nls/codepage/950.h) \
    $(wildcard include/config/mstar/sc.h) \
    $(wildcard include/config/lirc.h) \
    $(wildcard include/config/profile/annotated/branches.h) \
    $(wildcard include/config/bt/bnep.h) \
    $(wildcard include/config/blk/dev/ram/count.h) \
    $(wildcard include/config/rd/xz.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/inet/xfrm/mode/tunnel.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/nf/nat/needed.h) \
    $(wildcard include/config/nf/conntrack/snmp.h) \
    $(wildcard include/config/snd/drivers.h) \
    $(wildcard include/config/media/tuner/mt2266.h) \
    $(wildcard include/config/xfs/fs.h) \
    $(wildcard include/config/input/keyspan/remote.h) \
    $(wildcard include/config/usb/gspca/pac7302.h) \
    $(wildcard include/config/lockdep/support.h) \
    $(wildcard include/config/nls/codepage/869.h) \
    $(wildcard include/config/line6/usb.h) \
    $(wildcard include/config/fb/foreign/endian.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/rtc/intf/alarm.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/binfmt/aout.h) \
    $(wildcard include/config/usb/arch/has/ehci.h) \
    $(wildcard include/config/arch/u8500.h) \
    $(wildcard include/config/mp/android/low/mem/killer/include/cachemem.h) \
    $(wildcard include/config/chr/dev/osst.h) \
    $(wildcard include/config/acorn/partition.h) \
    $(wildcard include/config/video/capture/drivers.h) \
    $(wildcard include/config/arch/s5pv210.h) \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/mp/platform/arm/errata/775420.h) \
    $(wildcard include/config/video/vpx3220.h) \
    $(wildcard include/config/apds9802als.h) \
    $(wildcard include/config/netfilter/xt/match/sctp.h) \
    $(wildcard include/config/serial/input/disable/string.h) \
    $(wildcard include/config/snd/pcm/oss/plugins.h) \
    $(wildcard include/config/cpu/notifier/error/inject.h) \
    $(wildcard include/config/nls/iso8859/15.h) \
    $(wildcard include/config/inet6/esp.h) \
    $(wildcard include/config/autofs4/fs.h) \
    $(wildcard include/config/serial/input/manipulation.h) \
    $(wildcard include/config/mstar/msystem.h) \
    $(wildcard include/config/sysctl/syscall.h) \
    $(wildcard include/config/pda/power.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/nls/codepage/932.h) \
    $(wildcard include/config/ip6/nf/filter.h) \
    $(wildcard include/config/input/mousedev/screen/x.h) \
    $(wildcard include/config/need/dma/map/state.h) \
    $(wildcard include/config/iio.h) \
    $(wildcard include/config/serio/libps2.h) \
    $(wildcard include/config/blk/dev/drbd.h) \
    $(wildcard include/config/iscsi/boot/sysfs.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/netfilter/xt/match/connbytes.h) \
    $(wildcard include/config/android/paranoid/network.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/ms/nand/miu1/patch.h) \
    $(wildcard include/config/usb/printer.h) \
    $(wildcard include/config/arm/save/debug/context.h) \
    $(wildcard include/config/usb/gspca/pac207.h) \
    $(wildcard include/config/smc91x.h) \
    $(wildcard include/config/cpu/v7.h) \
    $(wildcard include/config/mp/platform/mstar/legancy/intr/debug.h) \
    $(wildcard include/config/fb/armclcd.h) \
    $(wildcard include/config/hid/twinhan.h) \
    $(wildcard include/config/panic/timeout.h) \
    $(wildcard include/config/zboot/rom/bss.h) \
    $(wildcard include/config/input/joystick.h) \
    $(wildcard include/config/qnx4fs/fs.h) \
    $(wildcard include/config/usb/oxu210hp/hcd.h) \
    $(wildcard include/config/usb/gspca/stv0680.h) \
    $(wildcard include/config/usb/storage/sddr55.h) \
    $(wildcard include/config/mp/debug/tool/watchdog/debug.h) \
    $(wildcard include/config/ip/nf/target/ttl.h) \
    $(wildcard include/config/cfg80211/default/ps.h) \
    $(wildcard include/config/usb/led.h) \
    $(wildcard include/config/serial/amba/pl010.h) \
    $(wildcard include/config/mstar/iomap.h) \
    $(wildcard include/config/preempt/none.h) \
    $(wildcard include/config/sparsemem/manual.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/netfilter/xt/match/time.h) \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/ip/vs.h) \
    $(wildcard include/config/have/kernel/gzip.h) \
    $(wildcard include/config/dm/uevent.h) \
    $(wildcard include/config/rtc/drv/rp5c01.h) \
    $(wildcard include/config/msatr/new/flip/function/enable.h) \
    $(wildcard include/config/netfilter/xt/match/mac.h) \
    $(wildcard include/config/task/size/3g/less/16m.h) \
    $(wildcard include/config/address/filter.h) \
    $(wildcard include/config/mp/mm/mstar/3dalloc/miu0/1/debug.h) \
    $(wildcard include/config/netfilter/xt/target/nflog.h) \
    $(wildcard include/config/video/bt819.h) \
    $(wildcard include/config/l2tp.h) \
    $(wildcard include/config/kallsyms/all.h) \
    $(wildcard include/config/usb/gspca/spca561.h) \
    $(wildcard include/config/android/timed/output.h) \
    $(wildcard include/config/libcrc32c.h) \
    $(wildcard include/config/mp/webcam/init.h) \
    $(wildcard include/config/arch/nr/gpio.h) \
    $(wildcard include/config/mfd/pcf50633.h) \
    $(wildcard include/config/crypto/sha256.h) \
    $(wildcard include/config/mp/miu/mapping.h) \
    $(wildcard include/config/arch/ixp23xx.h) \
    $(wildcard include/config/have/ftrace/mcount/record.h) \
    $(wildcard include/config/tegra/crypto/dev.h) \
    $(wildcard include/config/hid/sony.h) \
    $(wildcard include/config/rtc/intf/dev/uie/emul.h) \
    $(wildcard include/config/hw/console.h) \
    $(wildcard include/config/dm/debug.h) \
    $(wildcard include/config/ipx.h) \
    $(wildcard include/config/devmem.h) \
    $(wildcard include/config/usb/net/raw/ip.h) \
    $(wildcard include/config/ftmac100.h) \
    $(wildcard include/config/omfs/fs.h) \
    $(wildcard include/config/usb/gspca/zc3xx.h) \
    $(wildcard include/config/hid/monterey.h) \
    $(wildcard include/config/fb/udl.h) \
    $(wildcard include/config/hid/ezkey.h) \
    $(wildcard include/config/cpu/dcache/disable.h) \
    $(wildcard include/config/cfg80211/allow/reconnect.h) \
    $(wildcard include/config/iosched/noop.h) \
    $(wildcard include/config/snd/opl4/lib/seq.h) \
    $(wildcard include/config/serial/nonstandard.h) \
    $(wildcard include/config/bt/mrvl.h) \
    $(wildcard include/config/quotactl.h) \
    $(wildcard include/config/rtc/drv/rx8581.h) \
    $(wildcard include/config/crypto/cryptd.h) \
    $(wildcard include/config/neon.h) \
    $(wildcard include/config/mstar/kernel/fault/do/wdt/reset.h) \
    $(wildcard include/config/trusted/foundations.h) \
    $(wildcard include/config/generic/acl.h) \
    $(wildcard include/config/debug/kernel.h) \
    $(wildcard include/config/write/to/android/misc/nand.h) \
    $(wildcard include/config/mstar/sdmmc1/gpio/padspi2di.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/mstar/fusion/8/7.h) \
    $(wildcard include/config/video/cx2341x.h) \
    $(wildcard include/config/localversion.h) \
    $(wildcard include/config/radio/adapters.h) \
    $(wildcard include/config/keyboard/xtkbd.h) \
    $(wildcard include/config/usb/gspca/mars.h) \
    $(wildcard include/config/crypto.h) \
    $(wildcard include/config/function/tracer.h) \
    $(wildcard include/config/usb/trancevibrator.h) \
    $(wildcard include/config/mstar/fusion.h) \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/usb/stkwebcam.h) \
    $(wildcard include/config/mstar/system.h) \
    $(wildcard include/config/btrfs/fs.h) \
    $(wildcard include/config/default/mmap/min/addr.h) \
    $(wildcard include/config/snd/usb/6fire.h) \
    $(wildcard include/config/media/tuner/tda18218.h) \
    $(wildcard include/config/ip/nf/iptables.h) \
    $(wildcard include/config/cmdline.h) \
    $(wildcard include/config/mstar/chip/type.h) \
    $(wildcard include/config/bt/bnep/proto/filter.h) \
    $(wildcard include/config/fb/virtual.h) \
    $(wildcard include/config/usb/gspca/cpia1.h) \
    $(wildcard include/config/usb/xhci/hcd.h) \
    $(wildcard include/config/have/dma/api/debug.h) \
    $(wildcard include/config/mfd/sm501.h) \
    $(wildcard include/config/hid/samsung.h) \
    $(wildcard include/config/ip/pimsm/v2.h) \
    $(wildcard include/config/separate/printk/from/user.h) \
    $(wildcard include/config/usb/otg/blacklist/hub.h) \
    $(wildcard include/config/mp/android/dummy/mstar/rtc.h) \
    $(wildcard include/config/hw/perf/events.h) \
    $(wildcard include/config/st/gps.h) \
    $(wildcard include/config/mp/checkpt/boot/debug.h) \
    $(wildcard include/config/xfrm/statistics.h) \
    $(wildcard include/config/usb/isightfw.h) \
    $(wildcard include/config/usb/arch/has/hcd.h) \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
    $(wildcard include/config/strict/devmem.h) \
    $(wildcard include/config/generic/irq/show.h) \
    $(wildcard include/config/mstar/bd/none.h) \
    $(wildcard include/config/mfd/max8925.h) \
    $(wildcard include/config/ipv6/optimistic/dad.h) \
    $(wildcard include/config/mstar/ir/fantasy/mode.h) \
    $(wildcard include/config/alignment/trap.h) \
    $(wildcard include/config/dm9000.h) \
    $(wildcard include/config/scsi/mod.h) \
    $(wildcard include/config/mp/ntfs/volume/label.h) \
    $(wildcard include/config/usb/gspca/kinect.h) \
    $(wildcard include/config/oprofile.h) \
    $(wildcard include/config/crypto/crc32c.h) \
    $(wildcard include/config/serial/core.h) \
    $(wildcard include/config/fuse/fs.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/ethoc.h) \
    $(wildcard include/config/tree/rcu/trace.h) \
    $(wildcard include/config/embedded.h) \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/hid/microsoft.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/gic/set/multiple/cpus.h) \
    $(wildcard include/config/memory/start/address.h) \
    $(wildcard include/config/rtc/drv/s35390a.h) \
    $(wildcard include/config/have/kretprobes.h) \
    $(wildcard include/config/nf/defrag/ipv6.h) \
    $(wildcard include/config/usb/gspca/conex.h) \
    $(wildcard include/config/video/dev.h) \
    $(wildcard include/config/mstar/arm/bd/fpga.h) \
    $(wildcard include/config/nfs/v4.h) \
    $(wildcard include/config/mstar/rfid.h) \
    $(wildcard include/config/pmic/adp5520.h) \
    $(wildcard include/config/ppp/filter.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/sound/prime.h) \
    $(wildcard include/config/usb/et61x251.h) \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/nf/ct/proto/sctp.h) \
    $(wildcard include/config/bt/l2cap.h) \
    $(wildcard include/config/mp/wdt/off/dbg.h) \
    $(wildcard include/config/scsi.h) \
    $(wildcard include/config/ir/supply/rng.h) \
    $(wildcard include/config/fb/cfb/fillrect.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/nf/nat/pptp.h) \
    $(wildcard include/config/arch/vt8500.h) \
    $(wildcard include/config/hid/chicony.h) \
    $(wildcard include/config/hid.h) \
    $(wildcard include/config/nls/iso8859/9.h) \
    $(wildcard include/config/firmware/edid.h) \
    $(wildcard include/config/snd/serial/u16550.h) \
    $(wildcard include/config/battery/max17048.h) \
    $(wildcard include/config/clkdev/lookup.h) \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
    $(wildcard include/config/libfc.h) \
    $(wildcard include/config/mp/mm/mali/mmu/notifier/debug.h) \
    $(wildcard include/config/rtc/drv/cmos.h) \
    $(wildcard include/config/adfs/fs.h) \
    $(wildcard include/config/usb/pwc.h) \
    $(wildcard include/config/squashfs/fragment/cache/size.h) \
    $(wildcard include/config/media/tuner/tda8290.h) \
    $(wildcard include/config/bonding.h) \
    $(wildcard include/config/media/tuner/tda18212.h) \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/rtc/drv/em3027.h) \
    $(wildcard include/config/jbd2.h) \
    $(wildcard include/config/usbip/core.h) \
    $(wildcard include/config/mmc/ushc.h) \
    $(wildcard include/config/inet6/ipcomp.h) \
    $(wildcard include/config/tps6507x.h) \
    $(wildcard include/config/android/ram/console.h) \
    $(wildcard include/config/phylib.h) \
    $(wildcard include/config/mstar/reserve/mem/for/aeon.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
    $(wildcard include/config/nls/codepage/874.h) \
    $(wildcard include/config/ultrix/partition.h) \
    $(wildcard include/config/net/dsa.h) \
    $(wildcard include/config/virtio/balloon.h) \
    $(wildcard include/config/localversion/auto.h) \
    $(wildcard include/config/media/tuner/mt20xx.h) \
    $(wildcard include/config/ipc/ns.h) \
    $(wildcard include/config/misc/filesystems.h) \
    $(wildcard include/config/ftrace.h) \
    $(wildcard include/config/netfilter/xt/match/connlimit.h) \
    $(wildcard include/config/ip/nf/raw.h) \
    $(wildcard include/config/i2c/debug/core.h) \
    $(wildcard include/config/netfilter/xt/match/socket.h) \
    $(wildcard include/config/keyboard/adp5588.h) \
    $(wildcard include/config/r8712u.h) \
    $(wildcard include/config/hid/topseed.h) \
    $(wildcard include/config/cordic.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/decnet.h) \
    $(wildcard include/config/nf/nat/h323.h) \
    $(wildcard include/config/arch/sparsemem/default.h) \
    $(wildcard include/config/input/cma3000.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/bootparam/softlockup/panic.h) \
    $(wildcard include/config/hid/a4tech.h) \
    $(wildcard include/config/media/tuner/mc44s803.h) \
    $(wildcard include/config/i2c/debug/algo.h) \
    $(wildcard include/config/mp/platform/sw/patch/l2/sram/rma.h) \
    $(wildcard include/config/nls/codepage/860.h) \
    $(wildcard include/config/netfilter/xt/match/esp.h) \
    $(wildcard include/config/ip/nf/target/netmap.h) \
    $(wildcard include/config/usb/gspca/spca505.h) \
    $(wildcard include/config/rcu/cpu/stall/timeout.h) \
    $(wildcard include/config/usb/gspca/sq905c.h) \
    $(wildcard include/config/usb/zr364xx.h) \
    $(wildcard include/config/rtc/drv/isl1208.h) \
    $(wildcard include/config/snd/verbose/procfs.h) \
    $(wildcard include/config/cfg80211/debugfs.h) \
    $(wildcard include/config/mmc/mstar/mmc/emmc.h) \
    $(wildcard include/config/lapb.h) \
    $(wildcard include/config/mstar/software/ir.h) \
    $(wildcard include/config/mp/android/low/mem/killer/include/cachemem/debug.h) \
    $(wildcard include/config/input/ff/memless.h) \
    $(wildcard include/config/chr/dev/sg.h) \
    $(wildcard include/config/mp/compiler/error/debug.h) \
    $(wildcard include/config/snd/mpu401.h) \
    $(wildcard include/config/crypto/xcbc.h) \
    $(wildcard include/config/nf/nat/amanda.h) \
    $(wildcard include/config/keyboard/qt2160.h) \
    $(wildcard include/config/ip6/nf/match/ipv6header.h) \
    $(wildcard include/config/arch/nuc93x.h) \
    $(wildcard include/config/macvlan.h) \
    $(wildcard include/config/tps6105x.h) \
    $(wildcard include/config/cache/pl310.h) \
    $(wildcard include/config/mstar/vpool.h) \
    $(wildcard include/config/profiling.h) \
    $(wildcard include/config/pl310/errata/769419.h) \
    $(wildcard include/config/sensors/nct1008.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/inet6/xfrm/mode/transport.h) \
    $(wildcard include/config/crypto/arc4.h) \
    $(wildcard include/config/video/tea6415c.h) \
    $(wildcard include/config/usb/enestorage.h) \
    $(wildcard include/config/mstar/chip.h) \
    $(wildcard include/config/usb/gl860.h) \
    $(wildcard include/config/slhc.h) \
    $(wildcard include/config/scsi/tgt.h) \
    $(wildcard include/config/crypto/manager.h) \
    $(wildcard include/config/task/size.h) \
    $(wildcard include/config/eeprom/at24.h) \
    $(wildcard include/config/ppp/bsdcomp.h) \
    $(wildcard include/config/cifs/stats.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/vectors/base.h) \
    $(wildcard include/config/rtc/drv/test.h) \
    $(wildcard include/config/hid/ortek.h) \
    $(wildcard include/config/netfilter/xt/target/mark.h) \
    $(wildcard include/config/pci/syscall.h) \
    $(wildcard include/config/media/tuner/mxl5007t.h) \
    $(wildcard include/config/vmsplit/2g.h) \
    $(wildcard include/config/ipv6/subtrees.h) \
    $(wildcard include/config/arm/errata/742231.h) \
    $(wildcard include/config/mmc/block.h) \
    $(wildcard include/config/arch/realview.h) \
    $(wildcard include/config/abx500/core.h) \
    $(wildcard include/config/video/saa7185.h) \
    $(wildcard include/config/expert.h) \
    $(wildcard include/config/mstar/iic.h) \
    $(wildcard include/config/video/mt9v011.h) \
    $(wildcard include/config/wireless.h) \
    $(wildcard include/config/wext/proc.h) \
    $(wildcard include/config/squashfs.h) \
    $(wildcard include/config/usb/gspca/konica.h) \
    $(wildcard include/config/bt/hcibtusb.h) \
    $(wildcard include/config/perf/use/vmalloc.h) \
    $(wildcard include/config/video/tvp7002.h) \
    $(wildcard include/config/kgdb.h) \
    $(wildcard include/config/serio/raw.h) \
    $(wildcard include/config/fat/default/iocharset.h) \
    $(wildcard include/config/dump/use/regset.h) \
    $(wildcard include/config/tipc.h) \
    $(wildcard include/config/frame/warn.h) \
    $(wildcard include/config/mp/sd/mstar.h) \
    $(wildcard include/config/cfg80211/reg/debug.h) \
    $(wildcard include/config/rcu/cpu/stall/verbose.h) \
    $(wildcard include/config/debug/info/reduced.h) \
    $(wildcard include/config/generic/hweight.h) \
    $(wildcard include/config/initramfs/source.h) \
    $(wildcard include/config/video/ths7303.h) \
    $(wildcard include/config/mp/debug/tool/changelist.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/lzo/compress.h) \
    $(wildcard include/config/mmc.h) \
    $(wildcard include/config/crypto/seqiv.h) \
    $(wildcard include/config/cpa.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/rtc/drv/pl030.h) \
    $(wildcard include/config/tty/printk.h) \
    $(wildcard include/config/hid/logitech.h) \
    $(wildcard include/config/mp/mips/highmem/cache/alias/patch/debug.h) \
    $(wildcard include/config/arch/netx.h) \
    $(wildcard include/config/mstar/ramdisk.h) \
    $(wildcard include/config/stacktrace.h) \
    $(wildcard include/config/arch/iop32x.h) \
    $(wildcard include/config/pppolac.h) \
    $(wildcard include/config/scsi/sas/libsas.h) \
    $(wildcard include/config/netfilter/xt/target/idletimer.h) \
    $(wildcard include/config/inline/spin/unlock.h) \
    $(wildcard include/config/usb/net/rndis/wlan.h) \
    $(wildcard include/config/usb/debug.h) \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/usb/sevseg.h) \
    $(wildcard include/config/rtc/drv/pcf8563.h) \
    $(wildcard include/config/mp/mips/l2/catch.h) \
    $(wildcard include/config/mp/bdi/mstar/bdi/patch/debug.h) \
    $(wildcard include/config/usb/dsbr.h) \
    $(wildcard include/config/snd/debug.h) \
    $(wildcard include/config/kgdb/kdb.h) \
    $(wildcard include/config/dcb.h) \
    $(wildcard include/config/cgroup/cpuacct.h) \
    $(wildcard include/config/video/adv/debug.h) \
    $(wildcard include/config/fb/earlysuspend.h) \
    $(wildcard include/config/hz.h) \
    $(wildcard include/config/echo.h) \
    $(wildcard include/config/i2c/helper/auto.h) \
    $(wildcard include/config/netfilter/xt/match/u32.h) \
    $(wildcard include/config/squashfs/zlib.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/nls/iso8859/4.h) \
    $(wildcard include/config/arch/ixp4xx.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/fb/backlight.h) \
    $(wildcard include/config/mstar/offset/for/sboot.h) \
    $(wildcard include/config/rtc/drv/rx8025.h) \
    $(wildcard include/config/serial/8250/nr/uarts.h) \
    $(wildcard include/config/mstar/onenand/ip.h) \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/crypto/rmd160.h) \
    $(wildcard include/config/ir/imon.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/mp/ntfs/volume/id/debug.h) \
    $(wildcard include/config/inet/ah.h) \
    $(wildcard include/config/default/iosched.h) \
    $(wildcard include/config/udf/fs.h) \
    $(wildcard include/config/ipv6/mip6.h) \
    $(wildcard include/config/netfilter/debug.h) \
    $(wildcard include/config/mp/core/path.h) \
    $(wildcard include/config/vxfs/fs.h) \
    $(wildcard include/config/cgroup/perf.h) \
    $(wildcard include/config/nlattr.h) \
    $(wildcard include/config/tcp/cong/cubic.h) \
    $(wildcard include/config/ceph/fs.h) \
    $(wildcard include/config/scsi/netlink.h) \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/suspend/freezer.h) \
    $(wildcard include/config/mmc/sdhci.h) \
    $(wildcard include/config/snd/support/old/api.h) \
    $(wildcard include/config/netfilter/xt/connmark.h) \
    $(wildcard include/config/hid/kye.h) \
    $(wildcard include/config/firmware/in/kernel.h) \
    $(wildcard include/config/ibm/new/emac/zmii.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/input/touchscreen.h) \
    $(wildcard include/config/serial/input/manipulation/portnum.h) \
    $(wildcard include/config/radio/si4713.h) \
    $(wildcard include/config/mmc/block/deferred/resume.h) \
    $(wildcard include/config/outer/cache/sync.h) \
    $(wildcard include/config/usb/gspca/nw80x.h) \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/fb/sm7xx.h) \
    $(wildcard include/config/ip/nf/match/ah.h) \
    $(wildcard include/config/arpd.h) \
    $(wildcard include/config/afs/fs.h) \
    $(wildcard include/config/fb/wmt/ge/rops.h) \
    $(wildcard include/config/mp/android/coredump/path/debug.h) \
    $(wildcard include/config/i2c/parport/light.h) \
    $(wildcard include/config/netfilter/xt/match/limit.h) \
    $(wildcard include/config/ip6/nf/queue.h) \
    $(wildcard include/config/net/key/migrate.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/i2c/simtec.h) \
    $(wildcard include/config/nls/codepage/775.h) \
    $(wildcard include/config/fb/sys/fops.h) \
    $(wildcard include/config/wimax.h) \
    $(wildcard include/config/arm/errata/754322.h) \
    $(wildcard include/config/video/sr030pc30.h) \
    $(wildcard include/config/media/tuner/xc4000.h) \
    $(wildcard include/config/netfilter/xt/match/devgroup.h) \
    $(wildcard include/config/memstick.h) \
    $(wildcard include/config/keyboard/mpr121.h) \
    $(wildcard include/config/mfd/max8907c.h) \
    $(wildcard include/config/fb.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/mp/wdt/off/dbg/debug.h) \
    $(wildcard include/config/mp/platform/arm/errata/775420/debug.h) \
    $(wildcard include/config/nls/codepage/865.h) \
    $(wildcard include/config/i2c/compat.h) \
    $(wildcard include/config/keyboard/opencores.h) \
    $(wildcard include/config/cpu/32v7.h) \
    $(wildcard include/config/nfsd/v3/acl.h) \
    $(wildcard include/config/usb/gspca/spca500.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/arch/sa1100.h) \
    $(wildcard include/config/battery/ds2780.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/msdos/partition.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/plat/spear.h) \
    $(wildcard include/config/bt/hciuart.h) \
    $(wildcard include/config/bootparam/hardlockup/panic/value.h) \
    $(wildcard include/config/usb/gspca/sn9c2028.h) \
    $(wildcard include/config/have/oprofile.h) \
    $(wildcard include/config/ir/streamzap.h) \
    $(wildcard include/config/slub/stats.h) \
    $(wildcard include/config/have/generic/dma/coherent.h) \
    $(wildcard include/config/ip/set.h) \
    $(wildcard include/config/thermal.h) \
    $(wildcard include/config/usb/ulpi.h) \
    $(wildcard include/config/input/powermate.h) \
    $(wildcard include/config/ip/pnp/rarp.h) \
    $(wildcard include/config/brcmutil.h) \
    $(wildcard include/config/veth.h) \
    $(wildcard include/config/rtc/drv/pcf8583.h) \
    $(wildcard include/config/fb/cfb/rev/pixels/in/byte.h) \
    $(wildcard include/config/rcu/fanout/exact.h) \
    $(wildcard include/config/hid/petalynx.h) \
    $(wildcard include/config/arch/gemini.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/have/arch/kgdb.h) \
    $(wildcard include/config/jbd2/debug.h) \
    $(wildcard include/config/usb/arch/has/ohci.h) \
    $(wildcard include/config/bcm4329.h) \
    $(wildcard include/config/nf/conntrack/ipv4.h) \
    $(wildcard include/config/smp/on/up.h) \
    $(wildcard include/config/zone/dma/flag.h) \
    $(wildcard include/config/arch/ixp2000.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/battery/bq27x00.h) \
    $(wildcard include/config/snd/dummy.h) \
    $(wildcard include/config/usb/wusb/cbaf.h) \
    $(wildcard include/config/rps.h) \
    $(wildcard include/config/inet6/xfrm/tunnel.h) \
    $(wildcard include/config/fb/metronome.h) \
    $(wildcard include/config/rt/mutex/tester.h) \
    $(wildcard include/config/video/saa6588.h) \
    $(wildcard include/config/video/tcm825x.h) \
    $(wildcard include/config/sensors/bh1770.h) \
    $(wildcard include/config/zisofs.h) \
    $(wildcard include/config/mp/platform/mips.h) \
    $(wildcard include/config/video/media.h) \
    $(wildcard include/config/fb/broadsheet.h) \
    $(wildcard include/config/htc/pasic3.h) \
    $(wildcard include/config/blk/dev/cryptoloop.h) \
    $(wildcard include/config/ip/multicast.h) \
    $(wildcard include/config/crypto/rmd256.h) \
    $(wildcard include/config/wakelock/stat.h) \
    $(wildcard include/config/input/keychord.h) \
    $(wildcard include/config/write/to/android/misc.h) \
    $(wildcard include/config/squashfs/xz.h) \
    $(wildcard include/config/has/earlysuspend.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/no/user/space/screen/access/control.h) \
    $(wildcard include/config/gcov/kernel.h) \
    $(wildcard include/config/eeprom/max6875.h) \
    $(wildcard include/config/default/security.h) \
    $(wildcard include/config/debug/kmemleak.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/tick/oneshot.h) \
    $(wildcard include/config/crypto/ctr.h) \
    $(wildcard include/config/cgroup/debug.h) \
    $(wildcard include/config/sw/sync.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/media/tuner/mt2060.h) \
    $(wildcard include/config/video/fixed/minor/ranges.h) \
    $(wildcard include/config/debug/locking/api/selftests.h) \
    $(wildcard include/config/redlion/debug.h) \
    $(wildcard include/config/android/logger.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/hw/random.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
    $(wildcard include/config/arch/vexpress.h) \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
    $(wildcard include/config/sys/hypervisor.h) \
    $(wildcard include/config/amiga/partition.h) \
    $(wildcard include/config/debug/driver.h) \
    $(wildcard include/config/default/noop.h) \
    $(wildcard include/config/nls/codepage/737.h) \
    $(wildcard include/config/have/function/graph/tracer.h) \
    $(wildcard include/config/ntfs/fs.h) \
    $(wildcard include/config/ip/sctp.h) \
    $(wildcard include/config/base/small.h) \
    $(wildcard include/config/show/fault/trace/info.h) \
    $(wildcard include/config/mp/debug/tool/codedump.h) \
    $(wildcard include/config/crypto/blkcipher2.h) \
    $(wildcard include/config/xip/kernel.h) \
    $(wildcard include/config/scsi/sas/attrs.h) \
    $(wildcard include/config/devpts/multiple/instances.h) \
    $(wildcard include/config/dm/raid.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/arm/errata/430973.h) \
    $(wildcard include/config/arch/nomadik.h) \
    $(wildcard include/config/video/tm6000.h) \
    $(wildcard include/config/video/tda7432.h) \
    $(wildcard include/config/test/power.h) \
    $(wildcard include/config/mp/usb/mstar/debug.h) \
    $(wildcard include/config/usb/gspca/sonixj.h) \
    $(wildcard include/config/mp/platform/arm/debug.h) \
    $(wildcard include/config/video/ov7670.h) \
    $(wildcard include/config/rtc/drv/ds1553.h) \
    $(wildcard include/config/bt/bluesleep.h) \
    $(wildcard include/config/crypto/khazad.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/rc/map.h) \
    $(wildcard include/config/wext/priv.h) \
    $(wildcard include/config/scsi/lowlevel.h) \
    $(wildcard include/config/arm/errata/458693.h) \
    $(wildcard include/config/mstar/hw/tx/checksum.h) \
    $(wildcard include/config/nilfs2/fs.h) \
    $(wildcard include/config/nls/iso8859/3.h) \
    $(wildcard include/config/ext2/fs/xattr.h) \
    $(wildcard include/config/asus/oled.h) \
    $(wildcard include/config/hid/pantherlord.h) \
    $(wildcard include/config/mp/android/coredump/path.h) \
    $(wildcard include/config/test/kstrtox.h) \
    $(wildcard include/config/snd.h) \
    $(wildcard include/config/mp/scsi/mstar/sd/card/hotplug/debug.h) \
    $(wildcard include/config/ir/rc6/decoder.h) \
    $(wildcard include/config/ide.h) \
    $(wildcard include/config/video/msp3400.h) \
    $(wildcard include/config/video/bt856.h) \
    $(wildcard include/config/usb/pegasus.h) \
    $(wildcard include/config/usb/tmc.h) \
    $(wildcard include/config/mmc/debug.h) \
    $(wildcard include/config/blk/dev/bsglib.h) \
    $(wildcard include/config/ikconfig.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/debug/list.h) \
    $(wildcard include/config/mstar/arm/mmap/256mb/256mb.h) \
    $(wildcard include/config/lirc/staging.h) \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/arch/msm.h) \
    $(wildcard include/config/video/tlv320aic23b.h) \
    $(wildcard include/config/bridge/igmp/snooping.h) \
    $(wildcard include/config/ip/nf/target/clusterip.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/local/timers.h) \
    $(wildcard include/config/have/c/recordmcount.h) \
    $(wildcard include/config/pl310/errata/727915.h) \
    $(wildcard include/config/media/tuner/mt2131.h) \
    $(wildcard include/config/mp/usb/str/patch/debug.h) \
    $(wildcard include/config/ir/mceusb.h) \
    $(wildcard include/config/ldm/partition.h) \
    $(wildcard include/config/cifs.h) \
    $(wildcard include/config/xfrm/user.h) \
    $(wildcard include/config/net/ns.h) \
    $(wildcard include/config/have/perf/events.h) \
    $(wildcard include/config/sensors/lis3/i2c.h) \
    $(wildcard include/config/crypto/lrw.h) \
    $(wildcard include/config/ext3/fs/security.h) \
    $(wildcard include/config/arch/s5pc100.h) \
    $(wildcard include/config/usb/gspca/t613.h) \
    $(wildcard include/config/snd/mtpav.h) \
    $(wildcard include/config/ppp/async.h) \
    $(wildcard include/config/video/noon010pc30.h) \
    $(wildcard include/config/nls/codepage/866.h) \
    $(wildcard include/config/cramfs.h) \
    $(wildcard include/config/radio/saa7706h.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/stmmac/eth.h) \
    $(wildcard include/config/arch/iop13xx.h) \
    $(wildcard include/config/uid/stat.h) \
    $(wildcard include/config/debug/memory/init.h) \
    $(wildcard include/config/arm/charlcd.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/chr/dev/st.h) \
    $(wildcard include/config/dm/multipath.h) \
    $(wildcard include/config/crypto/user/api/skcipher.h) \
    $(wildcard include/config/sys/supports/apm/emulation.h) \
    $(wildcard include/config/rtc/intf/alarm/dev.h) \
    $(wildcard include/config/mp/emmc/partition/debug.h) \
    $(wildcard include/config/hid/multitouch.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/netfilter/xt/match/quota2.h) \
    $(wildcard include/config/hid/elecom.h) \
    $(wildcard include/config/snd/timer.h) \
    $(wildcard include/config/mstar/master0/only.h) \
    $(wildcard include/config/usb/ehci/tt/newsched.h) \
    $(wildcard include/config/debug/credentials.h) \
    $(wildcard include/config/fat/default/codepage.h) \
    $(wildcard include/config/ipv6/mroute/multiple/tables.h) \
    $(wildcard include/config/blk/dev.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/hvc/dcc.h) \
    $(wildcard include/config/tracing/support.h) \
    $(wildcard include/config/unix98/ptys.h) \
    $(wildcard include/config/cdrom/pktcdvd.h) \
    $(wildcard include/config/minix/subpartition.h) \
    $(wildcard include/config/netfilter/xt/target/connmark.h) \
    $(wildcard include/config/radio/wl1273.h) \
    $(wildcard include/config/crypto/michael/mic.h) \
    $(wildcard include/config/have/sched/clock.h) \
    $(wildcard include/config/crypto/ansi/cprng.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/jbd.h) \
    $(wildcard include/config/printk/time.h) \
    $(wildcard include/config/input/mousedev/screen/y.h) \
    $(wildcard include/config/ppp.h) \
    $(wildcard include/config/fb/macmodes.h) \
    $(wildcard include/config/netfilter/xt/match/quota.h) \
    $(wildcard include/config/have/kernel/lzo.h) \
    $(wildcard include/config/ms/usb/miu1/patch.h) \
    $(wildcard include/config/inet/diag.h) \
    $(wildcard include/config/crypto/ghash.h) \
    $(wildcard include/config/pstore.h) \
    $(wildcard include/config/mstar/dump/reg/lr.h) \
    $(wildcard include/config/nf/nat/ftp.h) \
    $(wildcard include/config/crypto/rmd320.h) \
    $(wildcard include/config/nf/ct/proto/udplite.h) \
    $(wildcard include/config/ikconfig/proc.h) \
    $(wildcard include/config/proc.h) \
    $(wildcard include/config/speakup.h) \
    $(wildcard include/config/elf/core.h) \
    $(wildcard include/config/arch/w90x900.h) \
    $(wildcard include/config/mp/mstar/str/base.h) \
    $(wildcard include/config/mp/nand/spansion.h) \
    $(wildcard include/config/input/pcf8574.h) \
    $(wildcard include/config/usb/support.h) \
    $(wildcard include/config/arm/flush/console/on/restart.h) \
    $(wildcard include/config/input/polldev.h) \
    $(wildcard include/config/rtc/drv/bq32k.h) \
    $(wildcard include/config/usb/gspca/pac7311.h) \
    $(wildcard include/config/rtc/debug.h) \
    $(wildcard include/config/netfilter/xt/match/qtaguid.h) \
    $(wildcard include/config/twl4030/core.h) \
    $(wildcard include/config/usb/hwa/hcd.h) \
    $(wildcard include/config/staging.h) \
    $(wildcard include/config/sensors/lis3lv02d.h) \
    $(wildcard include/config/inet/lro.h) \
    $(wildcard include/config/atm.h) \
    $(wildcard include/config/scsi/fc/attrs.h) \
    $(wildcard include/config/vt/console.h) \
    $(wildcard include/config/hid/uclogic.h) \
    $(wildcard include/config/mstar/nand/ip.h) \
    $(wildcard include/config/usb/gspca/stk014.h) \
    $(wildcard include/config/cfg80211/wext.h) \
    $(wildcard include/config/netfilter/xt/match/physdev.h) \
    $(wildcard include/config/nop/usb/xceiv.h) \
    $(wildcard include/config/fpe/nwfpe.h) \
    $(wildcard include/config/blk/dev/ram.h) \
    $(wildcard include/config/arch/footbridge.h) \
    $(wildcard include/config/netfilter/xt/match/state.h) \
    $(wildcard include/config/input/evbug.h) \
    $(wildcard include/config/arm/errata/720791.h) \
    $(wildcard include/config/bsd/disklabel.h) \
    $(wildcard include/config/rtc/drv/m48t35.h) \
    $(wildcard include/config/i2c/taos/evm.h) \
    $(wildcard include/config/mmc/test.h) \
    $(wildcard include/config/usb/storage/freecom.h) \
    $(wildcard include/config/mp/camera/plug/out/debug.h) \
    $(wildcard include/config/ir/rc5/decoder.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
    $(wildcard include/config/arm/timer/sp804.h) \
    $(wildcard include/config/drm.h) \
    $(wildcard include/config/inet6/xfrm/mode/beet.h) \
    $(wildcard include/config/rtc/drv/max6900.h) \
    $(wildcard include/config/ics932s401.h) \
    $(wildcard include/config/mp/usb/str/patch.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/fb/cfb/copyarea.h) \
    $(wildcard include/config/usb/sisusbvga.h) \
    $(wildcard include/config/usb/cytherm.h) \
    $(wildcard include/config/ip6/nf/match/eui64.h) \
    $(wildcard include/config/binary/printf.h) \
    $(wildcard include/config/mwifiex.h) \
    $(wildcard include/config/usb/storage/jumpshot.h) \
    $(wildcard include/config/arm/errata/460075.h) \
    $(wildcard include/config/mmc/clkgate.h) \
    $(wildcard include/config/generic/clockevents/build.h) \
    $(wildcard include/config/usb/storage/debug.h) \
    $(wildcard include/config/tracer/max/trace.h) \
    $(wildcard include/config/sgi/partition.h) \
    $(wildcard include/config/input/sparsekmap.h) \
    $(wildcard include/config/highpte.h) \
    $(wildcard include/config/usb/xhci/hcd/debugging.h) \
    $(wildcard include/config/video/v4l2.h) \
    $(wildcard include/config/sysvipc/sysctl.h) \
    $(wildcard include/config/mp/android/dummy/mstar/rtc/debug.h) \
    $(wildcard include/config/hid/ntrig.h) \
    $(wildcard include/config/dma/shared/buffer.h) \
    $(wildcard include/config/decompress/gzip.h) \
    $(wildcard include/config/xfrm/sub/policy.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/i2c/chardev.h) \
    $(wildcard include/config/llc.h) \
    $(wildcard include/config/cross/compile.h) \
    $(wildcard include/config/media/tuner/tea5761.h) \
    $(wildcard include/config/network/secmark.h) \
    $(wildcard include/config/generic/clockevents/broadcast.h) \
    $(wildcard include/config/video/tvaudio.h) \
    $(wildcard include/config/hid/holtek.h) \
    $(wildcard include/config/mp/nand/ubi/debug.h) \
    $(wildcard include/config/lx3/support.h) \
    $(wildcard include/config/rtc/drv/ds1672.h) \
    $(wildcard include/config/mp/jiffies/print.h) \
    $(wildcard include/config/ath6k/legacy.h) \
    $(wildcard include/config/mp/mmap/bufferable.h) \
    $(wildcard include/config/atalk.h) \
    $(wildcard include/config/nls/utf8.h) \
    $(wildcard include/config/radio/si470x.h) \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/netfilter/tproxy.h) \
    $(wildcard include/config/input/yealink.h) \
    $(wildcard include/config/iwmc3200top.h) \
    $(wildcard include/config/rtc/drv/m41t80.h) \
    $(wildcard include/config/vt6656.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/usb/usbnet.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/scsi/multi/lun.h) \
    $(wildcard include/config/hamradio.h) \
    $(wildcard include/config/android/ram/console/error/correction.h) \
    $(wildcard include/config/fhandle.h) \
    $(wildcard include/config/serial/altera/uart.h) \
    $(wildcard include/config/usb/microtek.h) \
    $(wildcard include/config/ibm/new/emac/tah.h) \
    $(wildcard include/config/hid/smartjoyplus.h) \
    $(wildcard include/config/new/leds.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/rtc/drv/ds1511.h) \
    $(wildcard include/config/rtc/drv/rv3029c2.h) \
    $(wildcard include/config/mac80211.h) \
    $(wildcard include/config/plat/mstar.h) \
    $(wildcard include/config/crc/ccitt.h) \
    $(wildcard include/config/ext3/fs/posix/acl.h) \
    $(wildcard include/config/blk/dev/sd.h) \
    $(wildcard include/config/cmdline/from/bootloader.h) \
    $(wildcard include/config/netfilter/netlink.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/average.h) \
    $(wildcard include/config/debug/notifiers.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/mp/debug/tool/kdebug.h) \
    $(wildcard include/config/usb/gspca/benq.h) \
    $(wildcard include/config/ad525x/dpot.h) \
    $(wildcard include/config/oc/etm.h) \
    $(wildcard include/config/arch/cns3xxx.h) \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/bitreverse.h) \
    $(wildcard include/config/blk/dev/ram/size.h) \
    $(wildcard include/config/mac/partition.h) \
    $(wildcard include/config/console/poll.h) \
    $(wildcard include/config/mp/nand/bbt/debug.h) \
    $(wildcard include/config/mp/platform/pm.h) \
    $(wildcard include/config/video/v4l2/common.h) \
    $(wildcard include/config/fb/mode/helpers.h) \
    $(wildcard include/config/nls/iso8859/13.h) \
    $(wildcard include/config/crypto/blkcipher.h) \
    $(wildcard include/config/usb/storage/onetouch.h) \
    $(wildcard include/config/nls/iso8859/8.h) \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/usb/gspca/xirlink/cit.h) \
    $(wildcard include/config/rtc/drv/stk17ta8.h) \
    $(wildcard include/config/usb/gspca/sn9c20x.h) \
    $(wildcard include/config/mstar/mpif.h) \
    $(wildcard include/config/karma/partition.h) \
    $(wildcard include/config/netpoll.h) \
    $(wildcard include/config/ibm/new/emac/emac4.h) \
    $(wildcard include/config/blk/dev/ub.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/ext2/fs/xip.h) \
    $(wildcard include/config/mp/checkpt/boot.h) \
    $(wildcard include/config/kprobes/sanity/test.h) \
    $(wildcard include/config/ring/buffer/benchmark.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/ip/nf/target/reject.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/sysctl/syscall/check.h) \
    $(wildcard include/config/ip6/nf/match/hl.h) \
    $(wildcard include/config/video/saa7110.h) \
    $(wildcard include/config/mp/mips/highmem/cache/alias/patch.h) \
    $(wildcard include/config/ip6/nf/match/opts.h) \
    $(wildcard include/config/rtc/intf/dev.h) \
    $(wildcard include/config/hid/support.h) \
    $(wildcard include/config/mstar/emac.h) \
    $(wildcard include/config/default/deadline.h) \
    $(wildcard include/config/serial/timberdale.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/netfilter/xt/match/osf.h) \
    $(wildcard include/config/ext4/fs/xattr.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/net/activity/stats.h) \
    $(wildcard include/config/video/bt866.h) \
    $(wildcard include/config/nls/default.h) \
    $(wildcard include/config/uts/ns.h) \
    $(wildcard include/config/nf/ct/netlink.h) \
    $(wildcard include/config/efs/fs.h) \
    $(wildcard include/config/auxdisplay.h) \
    $(wildcard include/config/crypto/aead2.h) \
    $(wildcard include/config/debug/info.h) \
    $(wildcard include/config/crypto/fcrypt.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/mp/ntfs/volume/label/debug.h) \
    $(wildcard include/config/bootparam/softlockup/panic/value.h) \
    $(wildcard include/config/netfilter/xt/match/hl.h) \
    $(wildcard include/config/crypto/algapi2.h) \
    $(wildcard include/config/mstar/arm/mmap/128mb.h) \
    $(wildcard include/config/keyboard/adp5589.h) \
    $(wildcard include/config/zboot/rom/text.h) \
    $(wildcard include/config/have/memblock.h) \
    $(wildcard include/config/input.h) \
    $(wildcard include/config/proc/sysctl.h) \
    $(wildcard include/config/netfilter/xt/target/ct.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/have/irq/work.h) \
    $(wildcard include/config/user/wakelock.h) \
    $(wildcard include/config/usb/kaweth.h) \
    $(wildcard include/config/input/ati/remote.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/input/ad714x.h) \
  include/linux/module.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/debug/set/module/ronx.h) \
  include/linux/list.h \
  include/linux/types.h \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/posix_types.h \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/const.h \
  include/linux/stat.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/stat.h \
  include/linux/time.h \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/kernel.h \
    $(wildcard include/config/numa.h) \
  /opt/arm-2010.09/bin/../lib/gcc/arm-none-linux-gnueabi/4.5.1/include/stdarg.h \
  include/linux/linkage.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/linkage.h \
  include/linux/bitops.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/bitops.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/v6.h) \
  include/linux/irqflags.h \
  include/linux/typecheck.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/irqflags.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/hwcap.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/outercache.h \
  include/mstar/mpatch_macro.h \
    $(wildcard include/config/mp/platform/sw/patch/l2/sram/rma/debug.h) \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/lock.h \
  include/asm-generic/bitops/le.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/printk.h \
  include/linux/init.h \
  include/linux/dynamic_debug.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/bug.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/div64.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/cache.h \
  include/linux/seqlock.h \
  include/linux/spinlock.h \
  include/linux/preempt.h \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/thread_info.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/iwmmxt.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/spinlock.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/processor.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/hw_breakpoint.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_smp.h \
  include/linux/rwlock_api_smp.h \
  include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/atomic.h \
  include/asm-generic/atomic-long.h \
  include/linux/math64.h \
  include/linux/kmod.h \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/zone/dma.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
  include/linux/wait.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/current.h \
  include/linux/threads.h \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/nodemask.h \
  include/linux/bitmap.h \
  include/linux/string.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/string.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/generated/bounds.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v3.h) \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/xscale.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/glue.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/memory.h \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt/16bit.h) \
  arch/arm/arm-boards/edison//include/mach/memory.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/sizes.h \
  include/asm-generic/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/asm-generic/getorder.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
  include/linux/notifier.h \
  include/linux/errno.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/mutex.h \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  include/linux/rwsem.h \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/sparsemem.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/smp.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/smp.h \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/pfn.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
  include/linux/timer.h \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/linux/timex.h \
  include/linux/param.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/param.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/timex.h \
  arch/arm/arm-boards/edison//include/mach/timex.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/sysctl.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tiny/preempt/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/preempt/rt.h) \
  include/linux/completion.h \
  include/linux/rcutree.h \
  include/linux/elf.h \
  include/linux/elf-em.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/elf.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/user.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/kref.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  include/linux/tracepoint.h \
  include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  include/linux/export.h \
    $(wildcard include/config/symbol/prefix.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/module.h \
  include/trace/events/module.h \
  include/trace/define_trace.h \
  include/linux/sched.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/mm/owner.h) \
  include/linux/capability.h \
  include/linux/rbtree.h \
  include/linux/mm_types.h \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/cmpxchg/local.h) \
    $(wildcard include/config/transparent/hugepage.h) \
  include/linux/auxvec.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/auxvec.h \
  include/linux/prio_tree.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/mmu.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/sem.h \
  include/linux/ipc.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/ipcbuf.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/sembuf.h \
  include/linux/signal.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/signal.h \
  include/asm-generic/signal-defs.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/sigcontext.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/pid.h \
  include/linux/proportions.h \
  include/linux/percpu_counter.h \
  include/linux/seccomp.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/seccomp.h \
  include/linux/unistd.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/unistd.h \
  include/linux/rculist.h \
  include/linux/rtmutex.h \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/hrtimer.h \
  include/linux/timerqueue.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
  include/linux/key.h \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  include/linux/aio.h \
  include/linux/aio_abi.h \
  include/linux/uio.h \
  include/linux/fs.h \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/ioctl.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/blk_types.h \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/path.h \
  include/linux/radix-tree.h \
  include/linux/semaphore.h \
  include/linux/fiemap.h \
  include/linux/shrinker.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/inet.h \
  include/linux/fcntl.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/err.h \
  include/linux/delay.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/delay.h \
    $(wildcard include/config/arch/provides/udelay.h) \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/slub_def.h \
  include/linux/kmemleak.h \
  include/linux/mm.h \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
  include/linux/debug_locks.h \
  include/linux/range.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/pgtable.h \
  include/asm-generic/4level-fixup.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/proc-fns.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/glue-proc.h \
    $(wildcard include/config/cpu/arm610.h) \
    $(wildcard include/config/cpu/arm7tdmi.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/cpu/arm720t.h) \
    $(wildcard include/config/cpu/arm740t.h) \
    $(wildcard include/config/cpu/arm9tdmi.h) \
    $(wildcard include/config/cpu/arm920t.h) \
    $(wildcard include/config/cpu/arm922t.h) \
    $(wildcard include/config/cpu/arm925t.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/arm940t.h) \
    $(wildcard include/config/cpu/arm946e.h) \
    $(wildcard include/config/cpu/arm1020.h) \
    $(wildcard include/config/cpu/arm1020e.h) \
    $(wildcard include/config/cpu/arm1022.h) \
    $(wildcard include/config/cpu/arm1026.h) \
    $(wildcard include/config/cpu/mohawk.h) \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/cpu/v6k.h) \
  arch/arm/arm-boards/edison//include/mach/vmalloc.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/pgtable-hwdef.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/s390.h) \
  include/linux/huge_mm.h \
  include/linux/vmstat.h \
  include/linux/vm_event_item.h \
  include/linux/ioport.h \
  include/linux/interrupt.h \
    $(wildcard include/config/irq/forced/threading.h) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/hardirq.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/irq.h \
  arch/arm/arm-boards/edison//include/mach/irqs.h \
  include/linux/irq_cpustat.h \
  include/trace/events/irq.h \
  include/linux/poll.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/poll.h \
  include/asm-generic/poll.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/uaccess.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
  include/linux/cdev.h \
  include/linux/device.h \
  include/linux/klist.h \
  include/linux/pm.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/device.h \
    $(wildcard include/config/dmabounce.h) \
  include/linux/pm_wakeup.h \
  include/linux/version.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/io.h \
  arch/arm/arm-boards/edison//include/mach/io.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/cacheflush.h \
    $(wildcard include/config/arm/errata/411920.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/glue-cache.h \
    $(wildcard include/config/cpu/cache/v3.h) \
    $(wildcard include/config/cpu/cache/v4.h) \
    $(wildcard include/config/cpu/cache/v4wb.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/shmparam.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/cachetype.h \
    $(wildcard include/config/cpu/cache/vivt.h) \
  drivers/mstar/include/mst_devid.h \
  drivers/mstar/mpool/mdrv_mpool.h \
  drivers/mstar/include/mdrv_mpool_io.h \
  drivers/mstar/include/mdrv_mpool_st.h \
  drivers/mstar/include/mdrv_types.h \
  drivers/mstar/include/mst_platform.h \
  drivers/mstar/include/mdrv_system.h \
    $(wildcard include/config/mask.h) \
    $(wildcard include/config/mstar/kip.h) \
  drivers/mstar/include/mdrv_system_st.h \
  drivers/mstar/include/mst_platform.h \
  arch/arm/arm-boards/edison/chip_setup.h \

drivers/mstar/mpool/mdrv_mpool.o: $(deps_drivers/mstar/mpool/mdrv_mpool.o)

$(deps_drivers/mstar/mpool/mdrv_mpool.o):
