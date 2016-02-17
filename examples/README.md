## Various examples

## btle_printer_float32.pth

A very simple topology to decode and print BTLE advertisement packets.
SDR Source is configuted for complex float32 mode, 2.426 GHz, and 2 Msps.
The contents of decoded BTLE packets will be printed to the console.

The topology is as follows:

SDR Source -> Freq Demod -> BTLE Decoder -> Message Printer

## btle_printer_int16.pth

Same as btle_printer_float32 but using only fixed point blocks.

## brennenstuhl_3600_control.pth

Control the Brennenstuhl 3600 with a simple GUI and SDR sink.
