# Bluetooth Low Energy SDR project

This project will make use of SDR hardware to receive and decode
Bluetooth Low Energy advertisement packets containing sensor data.
The sensor data will be used to control a wireless power switch.

## Generating advertisement packets

For this project we will make use of the Intel Edison bluez stack
to generate Bluetooth LE advertisement packets with sensor data.
Sensor data will be read from a 1-wire temperature sensor
and encoded into a PDU of the advertisement packet.

* TODO: generate advertisement packets
* TODO: poll temp sensors
* TODO: server daemon to poll + advertise

## Receiving + decoding Bluetooth LE

* TODO: Blocks to receive and decode BTLE
* TODO: Blocks to display the packet data
* TODO: Example project showing BTLE decoded

## Controlling the wireless switch

* TODO: Blocks to generate transmit packets for wireless switch
* TODO: Example project to trigger power switch with GUI radio box
* TODO: Example project tying it all together
