#!/bin/sh /etc/rc.common
# Example script
# Copyright (C) 2007 OpenWrt.org

START=99

path1='/mnt/sdb1'
path2='/mnt/sda1'


dogtest() {	      
	    if [ -e $1/lininofiles/sketch_SPI.cpp.hex ] && [ -e $1/lininofiles/autotest.py ]
		    
		then
		
		      python $1/lininofiles/autotest.py
		      
		else
		
		      echo "Test FILES not found ! ! !" >> $1/errortest.log
		      echo "timer" > /sys/class/leds/ds:green:wlan/trigger
		      echo "timer" > /sys/class/leds/ds:green:usb/trigger
		      
	    fi
}

boot() {
	    # echo boot
	    # commands to run on boot
	    sleep 120
	    if [ -d $path1 ] && `grep -qs $path1 /proc/mounts` && [ -d $path1/lininofiles ] && [ -d $path1/lininologs ]
	                  
                then echo "launching.....from sdb1..." && dogtest $path1          
                                                                              
            elif [ -d $path2 ] && `grep -qs $path2 /proc/mounts` && [ -d $path2/lininofiles ] && [ -d $path2/lininologs ]
	                 
                then  echo "launching.....from sda1..." && dogtest $path2
                                                                     
            else                                                     
                echo "Both removable devices are not mounted TEST FAILED ! ! !" >> /tmp/errormount.log
                echo "timer" > /sys/class/leds/ds:green:wlan/trigger
                echo "timer" > /sys/class/leds/ds:green:usb/trigger                                                                                       
            fi
}