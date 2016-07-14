#配置XP：打开远程桌面：在“控制面版”的“系统”，“远程”，勾上“允许用户远程连接到此计算机”；

#! /bin/sh
 
echo "Start rdesktop ..."
echo ""
#ipAddr=192.168.7.123
ipAddr=130.147.179.178
user=haitao.b.bao@philips.com
passwd=Daohaokechi1#
size=1800X960
size1=1200X800
echo $ipAddr
#rdesktop -g $size1 -a32 -f -r clipboard:PRIMARYCLIPBOARD -r sound:local -r disk:h=/root -c clipboard -u $user -p $passwd $ipAddr &
rdesktop -g $size1 -a32 -f -r clipboard:PRIMARYCLIPBOARD -r sound:local -r disk:h=/ -c clipboard -u $user -p $passwd $ipAddr &
echo "Succeeded!"
 
sleep 1
