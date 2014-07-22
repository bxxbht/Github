<?php
for ($i =1;$i<10;$i++)
{
	echo "hellow $i <br/>";
}
$far="nihao";
$_far= &$far;
$_far="my hi is $_far";
echo $_far;
?>
