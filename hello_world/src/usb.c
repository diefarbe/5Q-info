#include <stdint.h>
#include <stm32f4xx.h>
#include <usbd_core.h>
#include <usbd_hid.h>

#include "error.h"
#include "usb.h"
#include "usbd_desc.h"

USBD_HandleTypeDef USBD_DeviceStruct;

void USB_Setup_USB(void)
{
	USBD_Init(&USBD_DeviceStruct, &HID_Desc, 0);
	USBD_RegisterClass(&USBD_DeviceStruct, USBD_HID_CLASS);
	USBD_Start(&USBD_DeviceStruct);
}
