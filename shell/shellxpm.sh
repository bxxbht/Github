#!/bin/sh

Direc="/root/workdir/ut6000f_Mainthread/src"
#Direc="/root/workdir/ut6000f_max12/src"
#picPath="pic pic_1024 pic_7c pic_BCF pic_Fpro pic_G80"
picPath="pic"
for path in $picPath
do
echo $Direc/$path
cd $Direc/$path
listxpm=`ls *xpm`
for file in $listxpm
do
echo $file
sed 's/static char /static const char/' $file > $file.tmp
rm -f $file
mv -f  $file.tmp $file
done
chmod 777 *xpm
done


#for cm_hz14*
#cd $Direc
#listcm=`ls cm_hz14_*`
#for file in $listcm
#do
#echo $file
#sed 's/static char /static const char/' $file > $file.tmp
#rm -f $file
#mv -f  $file.tmp $file
#done
