#!/bin/bash

aa=(1 3 e23 44)
echo ${aa[*]}
for((i=0;i<${#aa[*]};i++)){
	echo ${aa[i]};
}

echo ""
echo ""

bb="da fadf fa d fa a fd"
echo ${bb}
for l in $bb;
do
echo $l
done


echo "end for.sh"
