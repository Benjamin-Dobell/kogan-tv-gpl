obj-$(CONFIG_MTD_UBI) += ubi.o

ubi-y += vtbl.o vmt.o upd.o build.o cdev.o kapi.o eba.o io.o wl.o scan.o
ubi-y += misc.o

ubi-$(CONFIG_MTD_UBI_DEBUG) += debug.o
obj-$(CONFIG_MTD_UBI_GLUEBI) += gluebi.o
ifeq ($(CONFIG_MP_NAND_UBI),y)
ubi-$(CONFIG_MTD_UBI_ENHANCE_INIT) += mstar/ubi_en_init.o
ubi-$(CONFIG_MTD_UBI_WRITE_CALLBACK) += mstar/wcb.o
endif
