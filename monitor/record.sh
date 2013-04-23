#! /bin/sh

recordmydesktop --windowid=`xwininfo -name "monitor" | grep "monitor" | awk '{print $4}'` --no-sound -o out.ogv
mencoder out.ogv -o out.flv -of lavf -oac pcm -ovc lavc -lavcopts vcodec=flv
