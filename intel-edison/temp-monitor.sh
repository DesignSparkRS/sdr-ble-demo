#!/bin/bash

rfkill unblock bluetooth
hciconfig hci0 up

scriptdir=`dirname "$BASH_SOURCE"`

if [ $# -eq 0 ]
  then
    echo "Specify a service UUID, Ex: EA06"
    exit -1
fi

while :
do
    cpu0_temp=$(cat /sys/devices/virtual/thermal/thermal_zone3/temp)
    cpu1_temp=$(cat /sys/devices/virtual/thermal/thermal_zone4/temp)
    echo "$(date): cpu0_temp=${cpu0_temp}, cpu1_temp=${cpu1_temp}"
    ${scriptdir}/advertise.exe "" "$1" "${cpu0_temp},${cpu1_temp}"
    sleep 1
done
