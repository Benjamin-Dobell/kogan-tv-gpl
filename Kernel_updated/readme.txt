/Kernel/kernel$ 下执行./genlink.sh
 
cp -rf .config_edison_SMP_android_emmc .config
 
make menuconfig
 
you missed the above required configs, y to continue, n to exit:
(or email wenjing.tien@mstarsemi.com to ask about android config)
(or email bob.fu@mstarsemi.com to ask about the rest)
输入 “y”
 
再退出
 
make clean;make -j4

最后编译完后会生成Kernel\kernel\arch\arm\boot\zImage

把zImage拷贝到JB\device\konka\konka_901下，替换kernel即可