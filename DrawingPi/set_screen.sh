#!/bin/bash

if [ $1 == 'on' ]; then
    sudo sh -c 'echo "1" > /sys/class/backlight/soc\:backlight/brightness'
else
    sudo sh -c 'echo "0" > /sys/class/backlight/soc\:backlight/brightness'
fi
