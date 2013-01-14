
export PATH=/usr/local/arm/4.2.2-eabi/usr/bin:$PATH


arm-linux-g++ -I.. -Wall -Wstrict-prototypes -Wno-trigraphs -g -fno-strict-aliasing -fno-common -fno-common -fno-unsigned-char -pipe  -march=armv4 -mtune=arm7tdmi  -I/root/ak_base -I/root/workdir/ak_base_ut_new  -c l_mpm.cpp



ar clq  akbase_4.4.2.a  l_mpm.o rkthread.o l_jn.o


