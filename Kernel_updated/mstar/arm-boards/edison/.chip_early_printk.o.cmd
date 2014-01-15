cmd_arch/arm/arm-boards/edison/chip_early_printk.o := arm-none-linux-gnueabi-gcc -Wp,-MD,arch/arm/arm-boards/edison/.chip_early_printk.o.d  -nostdinc -isystem /opt/arm-2010.09/bin/../lib/gcc/arm-none-linux-gnueabi/4.5.1/include -I/home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/arm-boards/edison//include -Iarch/arm/arm-boards/plat-mstar//include -Iarch/arm/arm-boards/edison/ -Iarch/arm/arm-boards/plat-mstar/ -I drivers/mstar -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -pg -fno-inline-functions-called-once -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(chip_early_printk)"  -D"KBUILD_MODNAME=KBUILD_STR(chip_early_printk)" -c -o arch/arm/arm-boards/edison/chip_early_printk.o arch/arm/arm-boards/edison/chip_early_printk.c

source_arch/arm/arm-boards/edison/chip_early_printk.o := arch/arm/arm-boards/edison/chip_early_printk.c

deps_arch/arm/arm-boards/edison/chip_early_printk.o := \
    $(wildcard include/config/mp/compiler/error.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/include/linux/kconfig.h \
    $(wildcard include/config/h.h) \
    $(wildcard include/config/.h) \
    $(wildcard include/config/foo.h) \
  include/linux/console.h \
    $(wildcard include/config/hw/console.h) \
    $(wildcard include/config/vga/console.h) \
  include/linux/types.h \
    $(wildcard include/config/mp/debug/tool/changelist.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/posix_types.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/ratelimit.h \
    $(wildcard include/config/printk.h) \
  include/linux/param.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/typecheck.h \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  include/linux/bitops.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/bitops.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/system.h \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/v6.h) \
  include/linux/linkage.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/linkage.h \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/irqflags.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/hwcap.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/outercache.h \
    $(wildcard include/config/mp/platform/arm.h) \
    $(wildcard include/config/outer/cache/sync.h) \
    $(wildcard include/config/outer/cache.h) \
  include/mstar/mpatch_macro.h \
    $(wildcard include/config/mp/platform/arch/general.h) \
    $(wildcard include/config/mp/platform/arch/general/debug.h) \
    $(wildcard include/config/mp/platform/arm/debug.h) \
    $(wildcard include/config/mp/platform/mips.h) \
    $(wildcard include/config/mp/platform/mips/debug.h) \
    $(wildcard include/config/mp/platform/fixme.h) \
    $(wildcard include/config/mp/platform/fixme/debug.h) \
    $(wildcard include/config/mp/platform/arm/pmu.h) \
    $(wildcard include/config/mp/platform/arm/pmu/debug.h) \
    $(wildcard include/config/mp/platform/pm.h) \
    $(wildcard include/config/mp/platform/pm/debug.h) \
    $(wildcard include/config/mp/platform/arm/errata/775420.h) \
    $(wildcard include/config/mp/platform/arm/errata/775420/debug.h) \
    $(wildcard include/config/mp/platform/mstar/legancy/intr.h) \
    $(wildcard include/config/mp/platform/mstar/legancy/intr/debug.h) \
    $(wildcard include/config/mp/platform/sw/patch/l2/sram/rma.h) \
    $(wildcard include/config/mp/platform/sw/patch/l2/sram/rma/debug.h) \
    $(wildcard include/config/mp/nand/ubi.h) \
    $(wildcard include/config/mp/nand/ubi/debug.h) \
    $(wildcard include/config/mp/nand/mtd.h) \
    $(wildcard include/config/mp/nand/mtd/debug.h) \
    $(wildcard include/config/mp/nand/ubifs.h) \
    $(wildcard include/config/mp/nand/ubifs/debug.h) \
    $(wildcard include/config/mp/nand/spansion.h) \
    $(wildcard include/config/mp/nand/spansion/debug.h) \
    $(wildcard include/config/mp/nand/bbt.h) \
    $(wildcard include/config/mp/nand/bbt/debug.h) \
    $(wildcard include/config/mp/nand/bbt/size.h) \
    $(wildcard include/config/mp/nand/bbt/size/debug.h) \
    $(wildcard include/config/mp/usb/mstar.h) \
    $(wildcard include/config/mp/usb/mstar/debug.h) \
    $(wildcard include/config/mp/sd/mstar.h) \
    $(wildcard include/config/mp/sd/mstar/debug.h) \
    $(wildcard include/config/mp/sd/plug.h) \
    $(wildcard include/config/mp/sd/plug/debug.h) \
    $(wildcard include/config/mp/emmc/partition.h) \
    $(wildcard include/config/mp/emmc/partition/debug.h) \
    $(wildcard include/config/mp/mstar/str/base.h) \
    $(wildcard include/config/mp/mstar/str/base/debug.h) \
    $(wildcard include/config/mp/android/low/mem/killer/include/cachemem.h) \
    $(wildcard include/config/mp/android/low/mem/killer/include/cachemem/debug.h) \
    $(wildcard include/config/mp/android/dummy/mstar/rtc.h) \
    $(wildcard include/config/mp/android/dummy/mstar/rtc/debug.h) \
    $(wildcard include/config/mp/android/alarm/clock/boottime/patch.h) \
    $(wildcard include/config/mp/android/alarm/clock/boottime/patch/debug.h) \
    $(wildcard include/config/mp/android/mstar/rc/map/define.h) \
    $(wildcard include/config/mp/android/mstar/rc/map/define/debug.h) \
    $(wildcard include/config/mp/android/coredump/path.h) \
    $(wildcard include/config/mp/android/coredump/path/debug.h) \
    $(wildcard include/config/mp/compiler/error/debug.h) \
    $(wildcard include/config/mp/debug/tool/watchdog.h) \
    $(wildcard include/config/mp/debug/tool/watchdog/debug.h) \
    $(wildcard include/config/mp/debug/tool/codedump.h) \
    $(wildcard include/config/mp/debug/tool/codedump/debug.h) \
    $(wildcard include/config/mp/debug/tool/kdebug.h) \
    $(wildcard include/config/mp/debug/tool/kdebug/debug.h) \
    $(wildcard include/config/mp/debug/tool/changelist/debug.h) \
    $(wildcard include/config/mp/remote/control/rc/register/retry.h) \
    $(wildcard include/config/mp/remote/control/rc/register/retry/debug.h) \
    $(wildcard include/config/mp/scsi/mstar/sd/card/hotplug.h) \
    $(wildcard include/config/mp/scsi/mstar/sd/card/hotplug/debug.h) \
    $(wildcard include/config/mp/mm/mstar/3dalloc/miu0/1.h) \
    $(wildcard include/config/mp/mm/mstar/3dalloc/miu0/1/debug.h) \
    $(wildcard include/config/mp/mm/mali/mmu/notifier.h) \
    $(wildcard include/config/mp/mm/mali/mmu/notifier/debug.h) \
    $(wildcard include/config/mp/mmap/bufferable.h) \
    $(wildcard include/config/mp/mmap/bufferable/debug.h) \
    $(wildcard include/config/mp/jiffies/print.h) \
    $(wildcard include/config/mp/jiffies/print/debug.h) \
    $(wildcard include/config/mp/miu/mapping.h) \
    $(wildcard include/config/mp/miu/mapping/debug.h) \
    $(wildcard include/config/mp/mips/l2/catch.h) \
    $(wildcard include/config/mp/mips/l2/catch/debug.h) \
    $(wildcard include/config/mp/wdt/off/dbg.h) \
    $(wildcard include/config/mp/wdt/off/dbg/debug.h) \
    $(wildcard include/config/mp/camera/plug/out.h) \
    $(wildcard include/config/mp/camera/plug/out/debug.h) \
    $(wildcard include/config/mp/sysattr/show.h) \
    $(wildcard include/config/mp/sysattr/show/debug.h) \
    $(wildcard include/config/mp/mips/highmem/cache/alias/patch.h) \
    $(wildcard include/config/mp/mips/highmem/cache/alias/patch/debug.h) \
    $(wildcard include/config/mp/checkpt/boot.h) \
    $(wildcard include/config/mp/checkpt/boot/debug.h) \
    $(wildcard include/config/mp/core/path.h) \
    $(wildcard include/config/mp/core/path/debug.h) \
    $(wildcard include/config/mp/webcam/init.h) \
    $(wildcard include/config/mp/webcam/init/debug.h) \
    $(wildcard include/config/mp/bdi/mstar/bdi/patch.h) \
    $(wildcard include/config/mp/bdi/mstar/bdi/patch/debug.h) \
    $(wildcard include/config/mp/ntfs/ioctl.h) \
    $(wildcard include/config/mp/ntfs/ioctl/debug.h) \
    $(wildcard include/config/mp/ntfs/volume/label.h) \
    $(wildcard include/config/mp/ntfs/volume/label/debug.h) \
    $(wildcard include/config/mp/ntfs/volume/id.h) \
    $(wildcard include/config/mp/ntfs/volume/id/debug.h) \
    $(wildcard include/config/mp/usb/str/patch.h) \
    $(wildcard include/config/mp/usb/str/patch/debug.h) \
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
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/const.h \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /opt/arm-2010.09/bin/../lib/gcc/arm-none-linux-gnueabi/4.5.1/include/stdarg.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/printk.h \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/dynamic_debug.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/div64.h \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/spinlock.h \
    $(wildcard include/config/thumb2/kernel.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/processor.h \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/arm/errata/754327.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/hw_breakpoint.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/inline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/atomic.h \
  include/asm-generic/atomic-long.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/uaccess.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/string.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/task/size.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/arm/patch/phys/virt/16bit.h) \
  arch/arm/arm-boards/edison//include/mach/memory.h \
    $(wildcard include/config/memory/start/address.h) \
    $(wildcard include/config/sparsemem.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/sizes.h \
  include/asm-generic/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \

arch/arm/arm-boards/edison/chip_early_printk.o: $(deps_arch/arm/arm-boards/edison/chip_early_printk.o)

$(deps_arch/arm/arm-boards/edison/chip_early_printk.o):
