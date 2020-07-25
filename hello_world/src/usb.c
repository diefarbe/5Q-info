#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stm32f4xx.h>

#include "error.h"
#include "usb.h"

enum {
	USB_STRING_DESCR_LANG_IDS = 0,
	USB_STRING_DESCR_MANUF,
	USB_STRING_DESCR_PROD,
	USB_STRING_DESCR_INTFC,
};

typedef struct {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} USB_SetupPacketTypeDef;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} __attribute__((packed)) USB_DeviceDescriptorTypeDef;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
} __attribute__((packed)) USB_ConfigurationDescriptorTypeDef;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} __attribute__((packed)) USB_InterfaceDescriptorTypeDef;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdHID;
	uint8_t bCountryCode;
	uint8_t bNumDescriptors;
	uint8_t bDescriptor1Type;
	uint16_t wDescriptor1Length;
} __attribute__((packed)) USB_HIDDescriptorTypeDef;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
} __attribute__((packed)) USB_EndpointDescriptorTypeDef;


static const USB_DeviceDescriptorTypeDef USB_DeviceDescriptorStruct = {
	sizeof(USB_DeviceDescriptorTypeDef),
	1,
	0x200,
	0, 0, 0, /* Interface specified class */
	64,
	0x24f0, 0x2020, /* VID, PID */
	0x001,
	1, 2, 0,
	1        /* One configuration */
};

static const uint8_t USB_ReportDescriptor0[] = {
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	0x09, 0x06,        // Usage (Keyboard)
	0xA1, 0x01,        // Collection (Application)
	0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
	0x19, 0xE0,        //   Usage Minimum (0xE0)
	0x29, 0xE7,        //   Usage Maximum (0xE7)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x08,        //   Report Count (8)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
	0x19, 0x00,        //   Usage Minimum (0x00)
	0x29, 0x65,        //   Usage Maximum (0x65)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x06,        //   Report Count (6)
	0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x08,        //   Usage Page (LEDs)
	0x19, 0x01,        //   Usage Minimum (Num Lock)
	0x29, 0x05,        //   Usage Maximum (Kana)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x05,        //   Report Count (5)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x75, 0x03,        //   Report Size (3)
	0x95, 0x01,        //   Report Count (1)
	0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0xC0,              // End Collection
};

static const uint8_t * const USB_ReportDescriptors[] = { USB_ReportDescriptor0 };
static const uint8_t USB_ReportDescriptorSizes[] = { sizeof(USB_ReportDescriptor0) };

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bString[];
} USB_StringDescriptorTypeDef;

#define USB_STRING_DESCRIPTOR(name, value) \
	static const USB_StringDescriptorTypeDef name = { sizeof(u##value), 3, u##value }

static const USB_StringDescriptorTypeDef USB_StringDescrLangIds = { 4, 3, { 0x0409 } };
USB_STRING_DESCRIPTOR(USB_StringDescrManuf,      "DieFarbe");
USB_STRING_DESCRIPTOR(USB_StringDescrProd,       "Hello World");
USB_STRING_DESCRIPTOR(USB_StringDescrInterface,  "HID Device");

static const void * const USB_StringDescriptors[] = {
	[USB_STRING_DESCR_LANG_IDS] = &USB_StringDescrLangIds,
	[USB_STRING_DESCR_MANUF]    = &USB_StringDescrManuf,
	[USB_STRING_DESCR_PROD]     = &USB_StringDescrProd,
	[USB_STRING_DESCR_INTFC]    = &USB_StringDescrInterface,
};
#define NUM_STRING_DESCRIPTORS (sizeof(USB_StringDescriptors)/sizeof(USB_StringDescriptors[0]))

typedef struct {
	USB_ConfigurationDescriptorTypeDef config;
	USB_InterfaceDescriptorTypeDef interface0;
	USB_HIDDescriptorTypeDef hid0;
	USB_EndpointDescriptorTypeDef ep1;
}  __attribute__((packed)) USB_CompositeDescriptorsTypeDef;

static const USB_CompositeDescriptorsTypeDef USB_ConfigurationDescriptorStruct = {
	{
		sizeof(USB_ConfigurationDescriptorTypeDef),
		2,
		sizeof(USB_CompositeDescriptorsTypeDef),
		1,  /* One interface */
		1,
		2,
		0xa0,
		250,
	},
	{
		/* Interface 0 */
		sizeof(USB_InterfaceDescriptorTypeDef),
		4,
		0,
		0,
		1,       /* One endpoint */
		3, 1, 1, /* HID Boot Keyboard */
		3,
	},
	{
		sizeof(USB_HIDDescriptorTypeDef),
		0x21,
		0x0111,
		0,
		1,
		0x22,
		sizeof(USB_ReportDescriptor0),
	},
	{
		/* EP1 */
		sizeof(USB_EndpointDescriptorTypeDef),
		5,
		0x81,
		0x03, /* Interrupt */
		8,
		8
	}
};

typedef struct {
	enum {
		MODE_NONE,
		MODE_CTLOUT,
		MODE_CTLIN,
		MODE_CTLIN_TRUNC,
	} EP0_Mode;
	uint16_t EP0_DataInLeft;
	uint8_t Config, IdleDuration, Protocol;
	uint8_t HIDReportIn[8];
	uint8_t HIDReportOut[8];
} USB_StateTypeDef;

static USB_StateTypeDef USB_StateStruct;
PCD_HandleTypeDef PCD_HandleStruct;

/**
  * @brief  This function handles USB-On-The-Go FS global interrupt request.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
	HAL_PCD_IRQHandler(&PCD_HandleStruct);
}

/* CTL out transfer, len < 64 */
static void USB_CtlOut(PCD_HandleTypeDef * hpcd, void *buf, size_t len)
{
	USB_StateTypeDef *state = hpcd->pData;
	const USB_SetupPacketTypeDef *req = (const USB_SetupPacketTypeDef *)hpcd->Setup;
	state->EP0_Mode = MODE_CTLOUT;
	if (len > req->wLength) {
		memset ((uint8_t *)buf + req->wLength, 0, len - req->wLength);
		len = req->wLength;
	}
	HAL_PCD_EP_Receive(hpcd, 0, (uint8_t *)buf, len);
}

/* CTL in transfer, len may be >= 64 */
static void USB_CtlIn(PCD_HandleTypeDef * hpcd, const void *buf, size_t len)
{
	USB_StateTypeDef *state = hpcd->pData;
	const USB_SetupPacketTypeDef *req = (const USB_SetupPacketTypeDef *)hpcd->Setup;
	if (len < req->wLength) {
		state->EP0_Mode = (len < 64? MODE_NONE : MODE_CTLIN_TRUNC);
	} else {
		len = req->wLength;
		state->EP0_Mode = (len <= 64? MODE_NONE : MODE_CTLIN);
	}
	if (len > 64) {
		state->EP0_DataInLeft = len - 64;
		len = 64;
	} else
		state->EP0_DataInLeft = 0;
	HAL_PCD_EP_Transmit(hpcd, 0, (uint8_t *)buf, len);
}

static bool USB_HandleStdDevSetup(PCD_HandleTypeDef * hpcd)
{
	USB_StateTypeDef *state = hpcd->pData;
	const USB_SetupPacketTypeDef *req = (const USB_SetupPacketTypeDef *)hpcd->Setup;
	switch (req->bRequest) {
	case 0: /* GET_STATUS */
		if (req->wLength == 2) {
			static uint16_t status = 0;
			USB_CtlIn(hpcd, &status, sizeof(status));
			return true;
		}
		break;
	case 5: /* SET_ADDRESS */
		if (req->wIndex == 0 && req->wLength == 0 && req->wValue < 0x80) {
			HAL_PCD_SetAddress(hpcd, req->wValue);
			USB_CtlIn(hpcd, NULL, 0);
			return true;
		}
		break;
	case 6: /* GET_DESCRIPTOR */
		if (req->wValue == 0x100) {
			USB_CtlIn(hpcd, &USB_DeviceDescriptorStruct, sizeof(USB_DeviceDescriptorStruct));
			return true;
		} else if (req->wValue == 0x200) {
			USB_CtlIn(hpcd, &USB_ConfigurationDescriptorStruct, sizeof(USB_ConfigurationDescriptorStruct));
			return true;
		} else if (req->wValue >= 0x300 && req->wValue < 0x300+NUM_STRING_DESCRIPTORS &&
			   USB_StringDescriptors[req->wValue&0xff] != NULL) {
			USB_CtlIn(hpcd, USB_StringDescriptors[req->wValue&0xff],
				  *(uint8_t*)USB_StringDescriptors[req->wValue&0xff]);
			return true;
		}
		break;
	case 9: /* SET_CONFIGURATION */
		if (req->wValue < 2) {
			if (req->wValue != state->Config) {
				state->Config = req->wValue;
				if (state->Config) {
					HAL_PCD_EP_Open(hpcd, 0x81, sizeof(state->HIDReportIn), EP_TYPE_INTR);
				} else {
					HAL_PCD_EP_Close(hpcd, 0x81);
				}
			}
			USB_CtlIn(hpcd, NULL, 0);
			return true;
		}
		break;
	}
	return false;
}

static bool USB_HandleClsIfcSetup(PCD_HandleTypeDef * hpcd)
{
	USB_StateTypeDef *state = hpcd->pData;
	const USB_SetupPacketTypeDef *req = (const USB_SetupPacketTypeDef *)hpcd->Setup;
	switch (req->bRequest) {
	case 1: /* GET_REPORT */
		if (req->wValue == 0x0100 && req->wLength <= sizeof(state->HIDReportIn)) {
			USB_CtlIn(hpcd, state->HIDReportIn, sizeof(state->HIDReportIn));
			return true;
		}
		break;
	case 2: /* GET_IDLE */
		if (req->wLength == 1 && req->wValue == 0) {
			USB_CtlIn(hpcd, &state->IdleDuration, sizeof(state->IdleDuration));
			return true;
		}
		break;
	case 3: /* GET_PROTOCOL */
		if (req->wLength == 1 && req->wValue == 0 && req->wIndex == 0) {
			USB_CtlIn(hpcd, &state->Protocol, sizeof(state->Protocol));
			return true;
		}
		break;
	case 9: /* SET_REPORT */
		if (req->wValue == 0x0200 && req->wLength <= sizeof(state->HIDReportOut)) {
			USB_CtlOut(hpcd, state->HIDReportOut, sizeof(state->HIDReportOut));
			return true;
		}
		break;
	case 10: /* SET_IDLE */
		if (req->wLength == 0 && !(req->wValue & 0xff)) {
			state->IdleDuration = req->wValue >> 8;
			USB_CtlIn(hpcd, NULL, 0);
			return true;
		}
		break;
	case 11: /* SET_PROTOCOL */
		if (req->wLength == 0 && req->wValue < 2 && req->wIndex == 0) {
			state->Protocol = req->wValue;
			USB_CtlIn(hpcd, NULL, 0);
			return true;
		}
		break;
	}
	return false;
}

static bool USB_HandleStdIfcSetup(PCD_HandleTypeDef * hpcd)
{
	const USB_SetupPacketTypeDef *req = (const USB_SetupPacketTypeDef *)hpcd->Setup;
	switch (req->bRequest) {
	case 6: /* GET_DESCRIPTOR */
		if (req->wValue == 0x2200) {
			USB_CtlIn(hpcd, USB_ReportDescriptors[req->wIndex],
				  USB_ReportDescriptorSizes[req->wIndex]);
			return true;
		}
		break;
	}
	return false;
}

/**
  * @brief  SetupStage callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef * hpcd)
{
	USB_StateTypeDef *state = hpcd->pData;
	const USB_SetupPacketTypeDef *req = (const USB_SetupPacketTypeDef *)hpcd->Setup;
	switch (req->bmRequestType & ~0x80) {
	case (0<<5)|0:
		if (USB_HandleStdDevSetup(hpcd))
			return;
		break;
	case (0<<5)|1:
		if (state->Config && req->wIndex == 0 && USB_HandleStdIfcSetup(hpcd))
			return;
		break;
	case (1<<5)|1:
		if (state->Config && req->wIndex == 0 && USB_HandleClsIfcSetup(hpcd))
			return;
		break;
	}
	HAL_PCD_EP_SetStall(hpcd, req->bmRequestType & 0x80);
}

/**
  * @brief  DataOut Stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	if (epnum == 0) {
		USB_StateTypeDef *state = hpcd->pData;

		if (state->EP0_Mode == MODE_CTLOUT) {
			state->EP0_Mode = MODE_NONE;
			HAL_PCD_EP_Transmit(hpcd, 0, NULL, 0);
		}
	}
}

/**
  * @brief  DataIn Stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	USB_StateTypeDef *state = hpcd->pData;

	if (epnum == 0) {
		if (state->EP0_Mode >= MODE_CTLIN) {
			if (state->EP0_DataInLeft >= 64) {
				if (!(state->EP0_DataInLeft -= 64) && state->EP0_Mode != MODE_CTLIN_TRUNC)
					state->EP0_Mode = MODE_NONE;
				HAL_PCD_EP_Transmit(hpcd, 0, hpcd->IN_ep[0].xfer_buff, 64);
			} else {
				state->EP0_Mode = MODE_NONE;
				HAL_PCD_EP_Transmit(hpcd, 0, hpcd->IN_ep[0].xfer_buff, state->EP0_DataInLeft);
			}
		} else {
			HAL_PCD_EP_SetStall(hpcd, 0x80);
			HAL_PCD_EP_Receive(hpcd, 0, NULL, 0);
		}
	}
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef * hpcd)
{
	USB_StateTypeDef *state = hpcd->pData;

	state->Config = 0;
	state->IdleDuration = 2;
	state->Protocol = 1;
	state->EP0_Mode = MODE_NONE;
	HAL_PCD_EP_Open(hpcd, 0x00, 64, EP_TYPE_CTRL);
	HAL_PCD_EP_Open(hpcd, 0x80, 64, EP_TYPE_CTRL);
}

/**
  * @brief  Suspend callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef * hpcd)
{
	__HAL_PCD_GATE_PHYCLOCK(hpcd);
}

/**
  * @brief  Resume callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef * hpcd)
{
	__HAL_PCD_UNGATE_PHYCLOCK(hpcd);
}

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef * hpcd)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	if (hpcd->Instance == USB_OTG_FS) {
		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		__HAL_RCC_USB_OTG_FS_CLK_ENABLE();
		HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
	}
}

void USB_Setup_USB(void)
{
	PCD_HandleStruct.Instance = USB_OTG_FS;
	PCD_HandleStruct.Init.dev_endpoints = 4;
	PCD_HandleStruct.Init.speed = PCD_SPEED_FULL;
	PCD_HandleStruct.Init.dma_enable = DISABLE;
	PCD_HandleStruct.Init.phy_itface = PCD_PHY_EMBEDDED;
	PCD_HandleStruct.Init.Sof_enable = DISABLE;
	PCD_HandleStruct.Init.low_power_enable = DISABLE;
	PCD_HandleStruct.Init.lpm_enable = DISABLE;
	PCD_HandleStruct.Init.battery_charging_enable = DISABLE;
	PCD_HandleStruct.Init.vbus_sensing_enable = DISABLE;
	PCD_HandleStruct.Init.use_dedicated_ep1 = DISABLE;
	PCD_HandleStruct.Init.use_external_vbus = ENABLE;
	PCD_HandleStruct.pData = &USB_StateStruct;
	CHECK_HAL_RESULT(HAL_PCD_Init(&PCD_HandleStruct));
	
	/* configure EPs FIFOs */
	HAL_PCDEx_SetRxFiFo(&PCD_HandleStruct, 0x80);
	HAL_PCDEx_SetTxFiFo(&PCD_HandleStruct, 0, 0x40);
	HAL_PCDEx_SetTxFiFo(&PCD_HandleStruct, 1, 0x80);

	CHECK_HAL_RESULT(HAL_PCD_Start(&PCD_HandleStruct));
}
