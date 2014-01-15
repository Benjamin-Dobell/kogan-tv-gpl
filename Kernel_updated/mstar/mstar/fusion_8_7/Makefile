EXTRA_CFLAGS      +=  -Idrivers/mstar/include 
EXTRA_CFLAGS      +=  -Idrivers/mstar/fusion_8_7
EXTRA_CFLAGS      +=  -Idrivers/mstar/fusion_8_7/single

obj-$(CONFIG_MSTAR_FUSION_8_7) += fusion.o

fusion-y := single/fusioncore_impl.o call.o debug.o entries.o fifo.o fusiondev.o fusionee.o list.o property.o reactor.o ref.o skirmish.o shmpool.o hash.o
