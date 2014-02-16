A Sane README
===

**Warning: Some of the other READMEs in this repo were provided by Kogan/Mstar and are non-sensical and/or generally wrong.**

Setup
===

The source, as provided by Kogan, referred to a .config_edison_SMP_android_emmc, however no such file was provided. The .config_edison_SMP_android_emmc included in this repo was extracted from http://media.kogan.com/files/support_files/KALED473DSMTZA%20FIRMWARE%2020131107.zip

    cp .config_edison_SMP_android_emmc Kernel_updated/kernel/.config

Compiling
===

Make sure a suitable ARM cross-compiling toolchain is in your PATH.

*Suitable toolchains are included in the AOSP project.* 


    cd Kernel_updated/kernel/
    export ARCH=arm
    export SUBARCH=arm
    export CROSS_COMPILE=arm-eabi-
    make menuconfig

