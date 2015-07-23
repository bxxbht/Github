
操作说明：

1.所有的文件必须包含在压缩目录中；比如需要打包的文件，和启动调用的脚本；
2.启动调用的脚本：如果涉及到拷贝文件，本地路径就是解压后的文件目录；
3.最后生成的run文件就是一个可执行的压缩包，它运行机制：首先解压本包到当前目录，然后启动执行脚本；




[root@localhost makeself]# makeself filepath filecopy.run "start file copy" ./cpfile.sh 
Header is 504 lines long

WARNING: Overwriting existing file: filecopy.run
About to compress 8 KB of data...
Adding files to archive named "filecopy.run"...
./
./b.txt
./a.txt
./cpfile.sh
./c.txt
CRC: 2518730158
MD5: 4fcab29b393081a3d313c5fa7b27ec9a

Self-extractable archive "filecopy.run" successfully created.
[root@localhost makeself]# ls
filecopy.run  filepath
[root@localhost makeself]# ./filecopy.run 
Verifying archive integrity...  100%   All good.
Uncompressing start file copy  100%  
cp: 无法获取"./filepath/*.txt" 的文件状态(stat): 没有那个文件或目录
copy success!
[root@localhost makeself]# cat ./filepath/c
cpfile.sh  c.txt      
[root@localhost makeself]# cat ./filepath/cpfile.sh 
#!/bin/bash

cp ./filepath/*.txt /tmp
echo "copy success!"
[root@localhost makeself]# vi ./filepath/cpfile.sh 
[root@localhost makeself]# ls
filecopy.run  filepath
[root@localhost makeself]# rm -rf filecopy.run 
[root@localhost makeself]# ls
filepath
[root@localhost makeself]# makeself filepath filecopy.run "start file copy" ./cpfile.sh 
Header is 504 lines long

About to compress 8 KB of data...
Adding files to archive named "filecopy.run"...
./
./b.txt
./a.txt
./cpfile.sh
./c.txt
CRC: 3695064092
MD5: 284a35c08031172d1d09a99e3a15231b

Self-extractable archive "filecopy.run" successfully created.
[root@localhost makeself]# ls
filecopy.run  filepath
[root@localhost makeself]# ./filecopy.run 
Verifying archive integrity...  100%   All good.
Uncompressing start file copy  100%  
copy success!
[root@localhost makeself]# cat ./filepath/cpfile.sh 
#!/bin/bash

cp ./*.txt /tmp
echo "copy success!"
[root@localhost makeself]# ls /tmp/*.txt
/tmp/100.txt           /tmp/panic-1.txt  /tmp/卡波.txt
/tmp/201502040855.txt  /tmp/panic.txt    /tmp/死机131225-1.txt
/tmp/a.txt             /tmp/TOP.txt      /tmp/死机131225.txt
/tmp/b.txt             /tmp/挂住-1.txt   /tmp/运行死机.txt
/tmp/c.txt             /tmp/挂住.txt
[root@localhost makeself]# 

