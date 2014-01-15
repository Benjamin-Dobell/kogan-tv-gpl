
#obj-y := agate_irq.o agate_timer.o agate_arch.o agate_dma.o
obj-y := agate_irq.o agate_arch.o agate_timer.o agate_early_printk.o 
obj-$(CONFIG_SMP)           += platsmp.o headsmp.o
obj-$(CONFIG_LOCAL_TIMERS)      += localtimer.o
#obj-$(CONFIG_HOTPLUG_CPU)       += hotplug.o
obj-y += platform.o
#obj-$(CONFIG_PM)+= pm.o sleep.o
