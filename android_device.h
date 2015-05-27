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

#ifndef ANDROID_DEV_H
#define ANDROID_DEV_H

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <libusb.h>


#define GOOGLE_VID						0x18D1
#define ACCESSORY_PID_ADB				0x2D02
#define ACCESSORY_PID_ADB				0x2D03

#define FIRST_REQUEST_TYPE				0xC0
#define ANDROID_REQUEST_TYPE			0x40

#define ACCESSORY_GET_PROTOCOL			51
#define ACCESSORY_SET_AUDIO_MODE		58
#define ACCESSORY_START					53
#define ACCESSORY_UNREGISTER_HID		54
#define ACCESSORY_UNREGISTER_HID		55
#define ACCESSORY_SET_HID_REPORT_DESC	56
#define ACCESSORY_SEND_HID_EVENT		57

#define HID_EVENT_LEN					2
#define CONSUMER_HID_DESC_LEN			23

// Various keycodes that I have used.
// More can be found here:https://source.android.com/devices/input/keyboard-devices.html
static const unsigned char KEYCODE_MEDIA_PLAY_PAUSE[]	= {0xCD, 0x00};
static const unsigned char KEYCODE_VOLUME_MUTE[]		= {0xE2, 0x00};
static const unsigned char KEYCODE_VOLUME_UP[]			= {0xE9, 0x00};
static const unsigned char KEYCODE_VOLUME_DOWN[]		= {0xEA, 0x00};
static const unsigned char KEYCODE_MEDIA_NEXT[]			= {0xB5, 0x00};
static const unsigned char KEYCODE_MEDIA_PREVIOUS[]		= {0xB6, 0x00};
static const unsigned char KEYCODE_MEDIA_STOP[]			= {0xB7, 0x00};
static const unsigned char KEYCODE_MUSIC[]				= {0x83, 0x01};
static const unsigned char HID_EVENT_RELEASE[]			= {0x00, 0x00};

static unsigned char consumer_hid_descriptor[CONSUMER_HID_DESC_LEN] = {
    0x05, 0x0c,                     // Usage Page (Consumer Devices)
    0x09, 0x01,                     // Usage (Consumer Control)
    0xa1, 0x01,                     // Collection (Application)
    0x19, 0x00,                     // Usage Minimum (0),
    0x2A, 0x3C, 0x02,               // Usage Maximum (0x23C),
    0x15, 0x00,                     // Logical Minimum (0)
    0x26, 0x3C, 0x02,               // Logical Maximum (0x23C)
    0x75, 0x10,                     // Report Size (10)
    0x95, 0x01,                     // Report Count (1)
    0x81, 0x00,                     // Input (Data,Array,Absolute)
    0xC0                            // End Collection
};

typedef struct {
    uint16_t   vendor_id;
    uint16_t   product_id;
    struct libusb_device_handle* handle;
} android_device;

bool android_device_find_device(android_device *and_dev);
bool android_device_set_accesory_mode(android_device *and_dev);
bool android_device_send_hid_event(android_device *and_dev, const unsigned char *event);

#endif