/Kernel/kernel$ ��ִ��./genlink.sh
 
cp -rf .config_edison_SMP_android_emmc .config
 
make menuconfig
 
you missed the above required configs, y to continue, n to exit:
(or email wenjing.tien@mstarsemi.com to ask about android config)
(or email bob.fu@mstarsemi.com to ask about the rest)
���� ��y��
 
���˳�
 
make clean;make -j4

��������������Kernel\kernel\arch\arm\boot\zImage

��zImage������JB\device\konka\konka_901�£��滻kernel����