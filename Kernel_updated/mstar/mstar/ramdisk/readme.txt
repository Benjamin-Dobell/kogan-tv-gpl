1. after system bootup, you can run below cmd to mount mstar ramdisk;
   module="msramdisk"
   major=`cat /proc/devices | awk "\\$2==\"$module\" {print \\$1}"`
   rm -rf /dev/msram0
   mknod /dev/msram0 b $major 0
   /opt/sbin/mkfs.ext3 /dev/msram0
   rm /mnt/myramdisk
   mkdir /mnt/myramdisk
   mount /dev/msram0 /mnt/myramdisk

   you can write above cmd into auto execute script file /etc/init.d/rcS 

2. you can get disk size by£º 
   df -h /mnt/myramdisk

3. you can change disk config(memory space config) by run mdrv_app.c
   that is a demo for how to change config.(now, mstar ramdisk only support up to 3 not consecutive memory chunks)
   but before you changing disk config, you must umount disk like below£º

   umount /mnt/myramdisk
   ./mdrv_app

   then use below cmd to format it again and remount it: 
   /opt/sbin/mkfs.ext3 /dev/msram0
   rm /mnt/myramdisk
   mkdir /mnt/myramdisk
   mount /dev/msram0 /mnt/myramdisk

   ok, now you can use it again.


