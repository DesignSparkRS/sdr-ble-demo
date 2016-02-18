# Bluetooth Low Energy SDR project

This project will make use of SDR hardware to receive and decode
Bluetooth Low Energy advertisement packets containing sensor data.
The sensor data will be used to control a wireless power switch.

## Generating advertisement packets

For this project we will make use of the Intel Edison bluez stack
to generate Bluetooth LE advertisement packets with sensor data.
Sensor data will be read from the on-die temperature sensor
and encoded into the service data of the advertisement packet.

To use, scp the contents of the intel-edison/ directory to the edison.

```
scp -rp intel-edison/* root@intel-edison-ip:
```

Then login to the edison and run the temperature monitor script.
This script will read the CPU die temperatures and encode them
into the advertisement data. The second argument is a 16-bit UUID.
Use the same UUID in the temperature monitoring and control block.

```
./temp-monitor.sh EA06
```

## Receiving + decoding Bluetooth LE

* TODO: Blocks to receive and decode BTLE
* TODO: Blocks to display the packet data
* TODO: Example project showing BTLE decoded

## Controlling the wireless switch

* TODO: Blocks to generate transmit packets for wireless switch
* TODO: Example project to trigger power switch with GUI radio box
* TODO: Example project tying it all together
