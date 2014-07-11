#!/bin/bash

# cmd 1:增加命名空间 2：删除命名空间
cmd=1
filelist=`ls *.h *.cpp`


#增加命名空间
if [ $cmd=1 ]; 
then
echo "增加命名空间"
echo "========================="
echo "========================="
for file in $filelist;
do
echo $file
sed '1 i\namespace CMS_NAMSPACE{' $file > $file.tmp
sed '$ a\}//end for  CMS_NAMESPACE' $file.tmp > $file.tmp2
rm -rf $file $file.tmp
mv $file.tmp2 $file
done

else

#删除命名空间
echo "删除命名空间"
echo "========================="
echo "========================="
for file in $filelist;
do
echo $file
sed  '1'd $file > $file.tmp
sed '$'d  $file.tmp > $file.tmp2
rm -rf $file $file.tmp
mv $file.tmp2 $file
done

fi

echo ""
echo "========================="
echo "finished !" 



