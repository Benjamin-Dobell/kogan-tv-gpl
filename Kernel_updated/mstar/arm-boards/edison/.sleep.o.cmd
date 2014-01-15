cmd_arch/arm/arm-boards/edison/sleep.o := arm-none-linux-gnueabi-gcc -Wp,-MD,arch/arm/arm-boards/edison/.sleep.o.d  -nostdinc -isystem /opt/arm-2010.09/bin/../lib/gcc/arm-none-linux-gnueabi/4.5.1/include -I/home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/arm-boards/edison//include -Iarch/arm/arm-boards/plat-mstar//include -Iarch/arm/arm-boards/edison/ -Iarch/arm/arm-boards/plat-mstar/ -I drivers/mstar -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2        -c -o arch/arm/arm-boards/edison/sleep.o arch/arm/arm-boards/edison/sleep.S

source_arch/arm/arm-boards/edison/sleep.o := arch/arm/arm-boards/edison/sleep.S

deps_arch/arm/arm-boards/edison/sleep.o := \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/include/linux/kconfig.h \
    $(wildcard include/config/h.h) \
    $(wildcard include/config/.h) \
    $(wildcard include/config/foo.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/linkage.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/hwcap.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  arch/arm/arm-boards/edison//include/mach/pm.h \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/vfpmacros.h \
    $(wildcard include/config/vfpv3.h) \
  /home/supengfei/MSD6369/Android_abroad/system/Kernel/kernel/arch/arm/include/asm/vfp.h \

arch/arm/arm-boards/edison/sleep.o: $(deps_arch/arm/arm-boards/edison/sleep.o)

$(deps_arch/arm/arm-boards/edison/sleep.o):
