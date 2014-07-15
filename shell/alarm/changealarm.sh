

#!/bin/bash

file="alarm.txt"
sed  's/ALARM_MODULE_*//g' $file > file_1
sed  's/ALARM_LEVEL_M/中级/g' file_1 > file_2
sed  's/ALARM_LEVEL_H/高级/g' file_2 > file_3
sed  's/ALARM_LEVEL_L/低级/g' file_3 > file_4
sed  's/ALARM_LEVEL_T/提示/g' file_4 > file_5
sed  's/ALARM_TYPE_P/生理/g' file_5 > file_6
sed  's/ALARM_TYPE_T/技术/g' file_6 > result.txt
#sed  's/\t/ /g' file_7 > result.txt

rm -rf file_*
