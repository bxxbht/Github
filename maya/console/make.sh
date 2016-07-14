#! /bin/bash

set -o nounset                              # Treat unset variables as an error

export PATH=/home/development/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin:$PATH

QT_INSTALL_ROOT=/home/development/ltib/rootfs/usr
$QT_INSTALL_ROOT/bin/qmake

make
