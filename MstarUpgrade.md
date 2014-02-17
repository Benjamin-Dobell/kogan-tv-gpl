Mstar Upgrade Binary Format
===

A Mstar upgrade binary consists of a firmware installation script and a payload.

The firmware installation script is executed by the TV and is used to setup partitions and extract various portions of payload, optionally decompressing said portions, and then flash them to partitions on the TV's eMMC flash memory.

The firmware installation script can also be used to customise the environment (i.e. environment variables) on the device.

## Example Memory Map from a [Kogan Upgrade Image](http://media.kogan.com/files/support_files/KALED473DSMTZA%20FIRMWARE%2020131107.zip)

    0x0000 - 0x4000: Firmware Installation Script
    0x4000 - 0x635D1A: U-Boot Multi-Part uImage [(gzip) Linux Recovery Kernel, (gzip) Recovery initramfs] -> recovery
    0xA04000 - 0xDA1B75: U-Boot Multi-Part uImage [(gzip) Android/Linux Kernel, (gzip) Android initramfs] -> boot
    0xE04000 - 0x19E04000: (Uncompressed) Android System Partition -> system
    0x19E04000 - 0x1A874BE7: (lzo) Android Userdata Partition -> userdata
    0x1A884000 - 0x1A9B867B: (lzo) Android Cache Partition -> cache
    0x1a9C4000 - 0x1F9C4000: (Uncompressed) Mstar TVService Partition -> tvservice
    0x1F9C4000 - 0x209C4000: (Uncompressed) Mstar TVCustomer Partition -> tvcustomer
    0x209C4000 - 0x211C4000: (Uncompressed) Mstar TVDatabase Partition -> tvdatabase
    0x211C4000 - 0x241C4000: (Uncompressed) Mstar Customer Config Partition -> customercfg
