
obj-y := eagle_irq.o 
obj-y += eagle_arch.o 
obj-y += eagle_timer.o 
obj-y += eagle_early_printk.o 
obj-$(CONFIG_SMP) += platsmp.o headsmp.o
obj-$(CONFIG_LOCAL_TIMERS) += localtimer.o
#obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o
obj-y += platform.o
obj-$(CONFIG_MSTAR_STR)+= sleep.o pm.o sleep_helper.o sleep_helper_asm.o
