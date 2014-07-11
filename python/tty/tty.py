

#! /usr/bin/env python  
 
#coding=utf-8  
 
import pty  
import os  
import select  
 
def mkpty():  
    #  
    master1, slave = pty.openpty()  
    slaveName1 = os.ttyname(slave)  
    print '\nslave device names: ', slaveName1 
    return master1

 
if __name__ == "__main__":  
 
    master1 = mkpty()  


