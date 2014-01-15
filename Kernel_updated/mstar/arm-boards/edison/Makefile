#
# Makefile for the linux kernel.
#

obj-y := chip_irq.o chip_arch.o chip_timer.o chip_early_printk.o 
obj-$(CONFIG_SMP)			+= platsmp.o
obj-y += platform.o

obj-$(CONFIG_OC_ETM)			+= emu.o
obj-$(CONFIG_MP_MSTAR_STR_BASE)+= sleep.o pm.o sleep_helper.o sleep_helper_asm.o
