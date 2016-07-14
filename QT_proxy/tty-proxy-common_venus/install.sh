#! /bin/sh

make -C driver clean
make -C driver

which qmake-qt4

if [ "$?" == "0" ]; then
	qmake-qt4
	make
	
	mkdir -p /opt/tty-proxy
	chmod 777 /opt/tty-proxy
	
	cp driver/ttyp.ko /opt/tty-proxy/
	cp client /opt/tty-proxy/
	cp src/kapp /opt/tty-proxy/
	cp src/mkln /opt/tty-proxy/
	cp src/rmln /opt/tty-proxy/
	cp proxy.png /opt/tty-proxy/
	cp proxy.desktop ~/Desktop/
else
	echo ""
	echo "*****************************"
	echo "  未安装qt4，不支持图形客户端"
	echo "  请先修改 ip.conf 文件"
	echo "  然后执行 start 脚本，启用代理"
	echo "*****************************"
	echo ""
fi
