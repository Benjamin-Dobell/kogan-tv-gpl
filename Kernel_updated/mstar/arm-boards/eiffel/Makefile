
obj-y := eiffel_irq.o 
obj-y += eiffel_arch.o 
obj-y += eiffel_timer.o 
obj-y += eiffel_early_printk.o 
obj-$(CONFIG_SMP) += platsmp.o headsmp.o
obj-$(CONFIG_LOCAL_TIMERS) += localtimer.o
#obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o
obj-y += platform.o
obj-$(CONFIG_MSTAR_STR)+= sleep.o pm.o sleep_helper.o sleep_helper_asm.o
