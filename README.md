# A Simple Android Accessory using libusb

Simple Android Acessory that I wrote. It shows you how to use the AOA 2.0 protocol to make enable USB Audio and HID Events. In its current form it just opens the default music app of the phone.

## Compiling.

You must have libusb-1.0.0 installed.

```shell
gcc -std=gnu99 -o driv android_device.c driver.c `pkg-config --libs --cflags libusb-1.0`
```

## TODO

I should probably add better error checking and possibly turn this into a real library.

## License

MIT.