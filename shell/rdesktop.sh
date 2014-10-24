#配置XP：打开远程桌面：在“控制面版”的“系统”，“远程”，勾上“允许用户远程连接到此计算机”；

#! /bin/sh
 
echo "Start rdesktop ..."
echo ""
ipAddr=192.168.7.151
user=haitao.b.bao@philips.com
passwd=Buyaodaohao1#
echo $ipAddr
rdesktop -g 1800x960 -a32 -f -r clipboard:PRIMARYCLIPBOARD -r sound:local -rdisk:C=/root -c clipboard -u $user -p $passwd $ipAddr &
echo "Succeeded!"
 
sleep 1
