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
    avg_tmp_c=$(python -c "print((${cpu0_temp}+${cpu1_temp})/2000.0)")
    echo "$(date): cpu0_temp=${cpu0_temp} mC, cpu1_temp=${cpu1_temp} mC, avg=${avg_tmp_c} C"
    ${scriptdir}/advertise.exe "" "$1" "${avg_tmp_c}"
    sleep 1
done
