LDFLAG=-lusb-1.0 -pthread -ljsoncpp

ifndef CFLAGS
	ifeq ($(TARGET),Debug)
		CFLAGS=-Wall -Wextra -g
	else
		CFLAGS=-Wall -Wextra -O2
	endif
endif

.PHONY: all clean

all: usb-proxy test-parser

usb-proxy: usb-proxy.o host-raw-gadget.o device-libusb.o GCS-parser.o proxy.o misc.o
	g++ usb-proxy.o host-raw-gadget.o device-libusb.o GCS-parser.o proxy.o misc.o $(LDFLAG) -o usb-proxy

test-parser: test-parser.o GCS-parser.o
	g++ test-parser.o GCS-parser.o $(LDFLAG) -o test-parser

%.o: %.cpp %.h
	g++ $(CFLAGS) -c $<

%.o: %.cpp
	g++ $(CFLAGS) -c $<

clean:
	-rm *.o
	-rm usb-proxy
	-rm test-parser
