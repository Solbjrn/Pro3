#!/bin/bash
###############################################################################
# Description:		script for startup on Beagle to configure pins and start 
#                	relevant programs and PRU'S           
#                                                               
# Author:         	Kasper Jensen & Christoffer Wesselhoff          
# Date:           	26-03-2020                                      
#                                                               
###############################################################################

#check for what machine we're on
machine=$(awk '{print $NF}' /proc/device-tree/model)

#Write in console what machine has been found
echo "$machine"

#Set environmental variable for machine
export MACHINE=$machine

#run main program
./pinchecktest