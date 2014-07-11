#!/bin/bash

data="name,sex,rollno,location"
#To read each of the item in a variable, we can use IFS.
oldIFS=$IFS
IFS=,
# now,
for item in $data;
do
echo Item: $item
done
IFS=$oldIFS

