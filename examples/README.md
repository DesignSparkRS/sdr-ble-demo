## Various examples

## btle_printer.pth

A very simple topology to decode and print BTLE advertisement packets.
SDR Source is configuted for complex int16 mode, 2.426 GHz, and 2 Msps.
The contents of decoded BTLE packets will be printed to the console.

The topology is as follows:

SDR Source -> Freq Demod -> BTLE Decoder -> Message Printer
