########################################################################
## Simple makefile for intel edison cross compiler
########################################################################
EDISON_SDK_ROOT := /opt/poky-edison/1.7.2

SYSROOT := $(EDISON_SDK_ROOT)/sysroots/core2-32-poky-linux
CC := $(EDISON_SDK_ROOT)/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux/i586-poky-linux-gcc

CFLAGS := -m32 -march=i586 --sysroot=$(SYSROOT)
LDFLAGS := -lbluetooth

advertise.exe: advertise.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

all: advertise.exe

clean:
	rm -rf *.exe
