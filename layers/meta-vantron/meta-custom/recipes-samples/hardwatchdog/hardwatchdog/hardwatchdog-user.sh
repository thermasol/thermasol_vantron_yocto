#!/bin/sh

if [ "$1" == "start"  ];then
        systemctl enable hardwatchdog.service
        systemctl start hardwatchdog.service
        sleep 1
        echo "hardwatchdog start." 
elif  [ "$1" == "stop"  ];then
        echo V > /dev/watchdog
        systemctl disable hardwatchdog.service
        systemctl stop hardwatchdog.service
        sleep 1
        echo "hardwatchdog stop."
else
        echo Error, Unkown $1 
fi

