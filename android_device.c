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

#include "android_device.h"
//TODO: Create better error handling

/*
 * This function tries to see if an android device is connected. 
 * If a valid device exists it grabs a handle to the device and populates the passed struct
 * If no valid device is found the function returns false  
 */
bool android_device_find_device(android_device * and_dev)
{
	// Various variables that we need to declare
	struct libusb_device **list;
	struct libusb_device *dev;
	struct libusb_device_descriptor desc = {0};
	// Gets every USB device that is connected
	ssize_t size = libusb_get_device_list (NULL,&list);
	// We are assuming that there is only one Android device connected.
	bool found_device = 0;
	for(int i = 0; i < size; i++)
	{
		dev = list[i];
        int rc = libusb_get_device_descriptor(dev, &desc);
		if(desc.idVendor == GOOGLE_VID)
		{
			found_device = true;
			break;
		}
	}
	// Need to make sure that we can get a handle to the device
	int err = libusb_open(dev, &(and_dev->handle));
	if(err || !found_device)
		return false;
	and_dev->vendor_id = desc.idVendor;
	and_dev->product_id = desc.idProduct;
	libusb_free_device_list(list, 1);
	return true;
}

/*
 * Attempts to put the device in accesory mode.  
 * This allows the device to output usb audio and accept HID Commands
 * Returns true if the device was put in accesory mode 
 * 	and we were able to get a handle to it.
 * Returns false if something went wrong.
 */
bool android_device_set_accesory_mode(android_device *and_dev)
{
	unsigned char buf[2];
	int response;
	// First thing that needs to be done is to check and see if the phone supports AOA
	response = libusb_control_transfer(and_dev->handle, FIRST_REQUEST_TYPE, 
										ACCESSORY_GET_PROTOCOL, 0, 0, buf, 2, 0);
	if(response < 0 || (buf[1] << 8 | buf[0] < 2))
		return false;
	// We need to add a short sleep or the next request might fail on some devices.
	usleep(1000);
	// Now we are requesting the ability to use usb audio
	response = libusb_control_transfer(and_dev->handle, ANDROID_REQUEST_TYPE, 
										ACCESSORY_SET_AUDIO_MODE, 1, 0, NULL, 0, 0);
	if (response < 0) 
		return false;
	// Request to put the device in accesory mode. 
	// The PID will now change.
	response = libusb_control_transfer(and_dev->handle, ANDROID_REQUEST_TYPE, 
												ACCESSORY_START, 0, 0, NULL, 0, 0);
	if (response < 0) 
		return false;
	//Since the PID changed, we need to release our current handle
	// and try to find the new one
	if (and_dev->handle != NULL)
		libusb_release_interface (and_dev->handle, 0);
	// This request fails sometimes, so we attempt it multiple times.
	for(int i = 0; i < 5; i++)
	{
		response = android_device_find_device(and_dev);
		if(response && 
			(and_dev->product_id == ACCESSORY_PID || and_dev->product_id == ACCESSORY_PID_ADB))
			break;
		if(i == 4)
			return false;
		// sleep for a second.
		usleep(1000000);
	}
	libusb_claim_interface(and_dev->handle, 0);
	// Now that the accesory part is set up we need to set up HID support
	response = libusb_control_transfer(and_dev->handle, ANDROID_REQUEST_TYPE, 
							ACCESSORY_REGISTER_HID, 1, CONSUMER_HID_DESC_LEN, NULL, 0, 0);
	if (response < 0)
		return false;

	response = libusb_control_transfer(and_dev->handle, ANDROID_REQUEST_TYPE, 
					ACCESSORY_SET_HID_REPORT_DESC, 1, 0, consumer_hid_descriptor, CONSUMER_HID_DESC_LEN, 0);
	if (response < 0)
		return false;
	// Now everything is set up properly, we can send HID events and play usb audio.
	return true;
}

/*
 * Sends an HID event to the device. 
 * Returns true on the event being sent succesfully
 * False when it is not sent sucessfully.
 */
bool android_device_send_hid_event(android_device *and_dev, const unsigned char *event)
{
	int response = libusb_control_transfer(and_dev->handle, ANDROID_REQUEST_TYPE, 
						ACCESSORY_SEND_HID_EVENT, 1, 0, (unsigned char*)event, HID_EVENT_LEN, 0);
	if(response < 0)
		return false;
	response = libusb_control_transfer(and_dev->handle, ANDROID_REQUEST_TYPE,
						ACCESSORY_SEND_HID_EVENT, 1, 0, (unsigned char *)HID_EVENT_RELEASE, HID_EVENT_LEN, 0);
	return !(response < 0);
}