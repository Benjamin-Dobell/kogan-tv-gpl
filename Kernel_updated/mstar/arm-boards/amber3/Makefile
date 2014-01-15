
#obj-y := amber3_irq.o amber3_timer.o amber3_arch.o amber3_dma.o
obj-y := amber3_irq.o amber3_arch.o amber3_timer.o amber3_early_printk.o 
obj-$(CONFIG_SMP)           += platsmp.o headsmp.o
obj-$(CONFIG_LOCAL_TIMERS)      += localtimer.o
#obj-$(CONFIG_HOTPLUG_CPU)       += hotplug.o
obj-y += platform.o
obj-$(CONFIG_MSTAR_STR)+= sleep.o pm.o sleep_helper.o sleep_helper_asm.o
