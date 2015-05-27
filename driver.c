/*
The MIT License (MIT)

Copyright (c) 2015 Thomas Corley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>

#include "android_device.h"

int main(int argc, char const *argv[])
{
    libusb_init(NULL);
    android_device dev;

    printf("Attempting to find device...");
    if (!android_device_find_device(&dev))
    {
        printf("Failure!...Exiting!\n");
        return -1;
    }
    else
        printf("Success!\n");
    if (dev.product_id != ACCESSORY_PID && dev.product_id != ACCESSORY_PID_ADB)
    {
        printf("Attempting to put device in accessory mode...");
        if (!android_device_set_accesory_mode(&dev))
        {
            printf("Failure...Exiting!\n");
            return -1;
        }
        printf("Success!\n");
        usleep(100000);
    }
    else
        printf("Device already in Accesory Mode!\n");
    printf("Attempting to open music app...\n");
    if (android_device_send_hid_event(&dev, KEYCODE_MUSIC))
        printf("Success!\n");
    else
        printf("Failure!\n");
    return 0;
}