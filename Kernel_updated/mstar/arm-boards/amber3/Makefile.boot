ifeq ($(EXTRAVERSION),.13)
zreladdr-y   := $(shell /bin/bash -c 'printf "0x%08x" $$[$(CONFIG_MEMORY_START_ADDRESS) + 0x8000]')
params_phys-y   := 0x40000100
initrd_phys-y   := 0x40800000
else
#if((CONFIG_ARCH_COLUMBUS),y)
#ZTEXTADDR:=0x00008000
#endif
#if((CONFIG_ARCH_PIONEER),y)
#ZTEXTADDR:=0x00008000
#endif
zreladdr-y   := 0x0a008000
#params_phys-y   := 0x09000000
#initrd_phys-y   := 0x20410000

endif
