#!/bin/sh
cp linux.config linux/arch/x86/configs/BLFS2_defconfig
cd linux
make distclean
make BLFS2_defconfig
rm arch/x86/configs/BLFS2_defconfig
make -j8
