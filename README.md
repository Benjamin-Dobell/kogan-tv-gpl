A Sane README
===

**Warning:** Some of the other READMEs in this repo were provided by Kogan/Mstar and are non-sensical and/or generally wrong.

Prerequisites
===

If running OS X, install homebrew then:

    brew install binutils coreutils gar libelf xz

You will also need an ARM cross-compilation toolchain.

OS X:

    git clone https://android.googlesource.com/platform/prebuilts/gcc/darwin-x86/arm/arm-eabi-4.6

Linux:

    git clone https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6

You will want to add the cloned bin/ directory to your PATH.

Compiling Android/Linux Kernel
===

Copy the pregenerated kernel config to the kernel sub-directory.

    cp .config_edison_SMP_android_emmc Kernel_updated/kernel/.config

Then build the kernel as follows:

    cd Kernel_updated/kernel/
    
    export ARCH=arm
    export CROSS_COMPILE=arm-eabi-

OS X:

    make -j4 HOSTCFLAGS="-I$(brew --prefix libelf) -I$(pwd)/include/linux -I$(pwd)/arch/arm/include/"

Linux:

    make -j4

Where you replace *&lt;PATH\_TO\_ELFUTILS&gt;* with the path to elfutils. If you have downloaded AOSP, elfutils is included in the external/ sub-directory.

Compiling U-Boot
===

There is no major need to replace U-Boot on your device. All TVs I've encountered will happily flash and custom kernels and Android builds using the preinstalled bootloaders. As such you may wish to skip building MBoot.

Compiling U-Boot
---


    cd MBoot/u-boot-2011.06
    make edison -j4

Building sboot
---

The source code for sboot is not included. Precompiled binaries are included and it's possible to build MBoot using these (only tested on Linux using the precompiled 32-bit Linux binaries in this repo).

Notes
===

 * Kernel_updated/kernel/Makefile has been modified as provided from Kogan/Mstar to skip running mpatch (Mstar's patching tool). This was done because the included binary is Linux only, but the necessary files to patch the kernel seem to be have been pregenerated and included anyway.

 * The source, as provided by Kogan, referred to a .config_edison_SMP_android_emmc, however no such file was provided. The .config_edison_SMP_android_emmc included in this repo was extracted from http://media.kogan.com/files/support_files/KALED473DSMTZA%20FIRMWARE%2020131107.zip

 * Several minor tweaks were made to U-Boot (and Mstar) Makefiles to ensure U-Boot builds on Mac.
