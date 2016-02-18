# Bluetooth Low Energy SDR project

This project will make use of SDR hardware to receive and decode
Bluetooth Low Energy advertisement packets containing sensor data.
The sensor data will be used to control a wireless power switch.

## Building dependencies

* First install pothos: https://github.com/pothosware/pothos/wiki
* Next build the blocks in this repository:

```
cd sdr-ble-demo
mkdir build
cd build
cmake ../
make -j4
sudo make install
```

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

The temp-monitor.sh script will run forever and update the advertisement data.
When the user CTRL+C's the script, the last reading will continue to broadcast.

## Receiving + decoding Bluetooth LE

Next, we will use a SDR and some procesing blocks to receive and decode BTLE.
This project comes with a BTLE Decoder block that is capable of
searching a frequency-demodulated input stream and extracting packets.
The decoded advertisement packets will be printed to the console.

Run the PothosGui, and open examples/btle_printer_float32.pth.
Click execute, and see the packets from the intel edison.

![Screenshot](https://raw.githubusercontent.com/DesignSparkrs/sdr-ble-demo/master/references/screenshots/btle_printer.png)

## Controlling the wireless switch

* TODO: Blocks to generate transmit packets for wireless switch
* TODO: Example project to trigger power switch with GUI radio box
* TODO: Example project tying it all together
