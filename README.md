# Information

#### Device: Applent AT4208

This program works with hidapi.h and can read from many AT4208 devices

## Manual
1. Install 'hidapi.h' and 'hidapi_libusb.h' (https://github.com/libusb)
```
sudo apt install libhidapi-dev
```
2. Check for libraries in a directory
```
ls /usr/local/include/hidapi/
```
3. Clone repository
```
git clone https://github.com/ipanshin91/hid-multi-applent.git
```
4. Compile the program
```
g++ hid.cpp -lhidapi-libusb -I/usr/local/include/hidapi/ -o hid
```

## Usage
#### Check availability of device VID: 0825 / PID: 0826
```
$ lsusb | grep APPLENT
Bus 004 Device 117: ID 0825:0826 GC Protronics APPLENT USB
```
#### Add rules
```
/etc/udev/rules.d/99-anbai.rules
SUBSYSTEM=="usb", ATTR{idVendor}=="0825", ATTR{idProduct}=="0826", MODE="0666"
udevadm control --reload-rules && sudo udevadm trigger
```

#### Query system info
```
$ ./hid "IDN?" ""
AT4208,REV E2.1,<Serial_Number>,Applent Instruments Inc.
AT4208,REV E1.0,<Serial_Number>,Applent Instruments Inc.
```

#### Query measure value
```
$ ./hid "FETC?" ""
  27.1,-1000.0,-1000.0,-1000.0,-1000.0,-1000.0,-1000.0,-1000.0
  43.1,  42.8,  30.3,  32.5,  31.7,  28.9,  26.8,  27.5
```
