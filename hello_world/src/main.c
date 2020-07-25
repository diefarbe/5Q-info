#include <stdint.h>
#include <stdbool.h>
#include <stm32f4xx.h>

#include "error.h"
#include "dma.h"
#include "adc.h"
#include "tim.h"
#include "spi.h"
#include "led.h"
#include "usb.h"


static const uint8_t KeyCodes[9][14] = {
	/* Row 0 omitted */
	{ 0x3a, 0x29, 0x1f, 0x1e, 0x35, 0x14, 0x2b, 0x04, 0x39, 0x64, 0x1d, 0xe1, 0xe3, 0xe0 },
	{ 0x3c, 0x3b, 0x20, 0x21, 0x08, 0x1a, 0x15, 0x16, 0x09, 0x07, 0x1b, 0x06, 0x8b, 0xe2 },
	{ 0x3e, 0x3d, 0x24, 0x22, 0x23, 0x1c, 0x17, 0x0b, 0x0a, 0x05, 0x11, 0x19, 0x2c, 0x88 },
	{ 0x3f, 0x40, 0x25, 0x26, 0x0c, 0x18, 0x12, 0x0d, 0x0f, 0x0e, 0x10, 0x37, 0x36, 0x8a },
	{ 0x42, 0x41, 0x2e, 0x27, 0x2d, 0x2f, 0x13, 0x30, 0x33, 0x34, 0x32, 0x38, 0xe7, 0xe6 },
	{ 0x43, 0x45, 0x89, 0x44, 0x2a, 0x49, 0x4c, 0x4d, 0x31, 0x28, 0xe5, 0x87, 0x65, 0xe4 },
	{ 0x48, 0x46, 0x53, 0x47, 0x4b, 0x4a, 0x5f, 0x4e, 0x5c, 0x59, 0x52, 0x51, 0x4f, 0x50 },
	{ 0x00, 0x00, 0xf1, 0xf0, 0x54, 0x61, 0x60, 0x5e, 0x5d, 0x5a, 0x5b, 0x62, 0x85, 0x63 },
	{ 0xf3, 0x00, 0xf2, 0x00, 0x55, 0x56, 0x57, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58 },
};

static uint8_t HIDReport[8];
static bool HIDReportOverflow;

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}


/**
  * @brief  Initialize the MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Configure the main internal regulator output voltage 
    */
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = (HSE_VALUE / 2000000u);

	RCC_OscInitStruct.PLL.PLLN = 168;

	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7; /* To make USB work. */
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;

	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	/**Configure clock outputs */	
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInit.PLLI2S.PLLI2SN = 100;
	PeriphClkInit.PLLI2S.PLLI2SR = 4;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_2);
	HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_PLLI2SCLK, RCC_MCODIV_5);

	/**Enable Clock Security System */
	HAL_RCC_EnableCSS();

	/**Configure the Systick interrupt time 
    */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick 
    */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** GPIO Setup
*/
void GPIO_Setup(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/** Enable GPIO clocks */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	/** Set default level for output pins */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_All, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);

	/** Setup GPIO modes */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* PE2 */

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* PE3 */

	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* PE4-PE5 */

	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); /* PC11-PC15 */

	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); /* PB2 & PB12 */

	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* PE15 */

	GPIO_InitStruct.Pin = GPIO_PIN_All;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); /* PD0-PD15 */

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); /* PC8 */

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); /* PC9: MCO_2 */

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); /* PA8: MCO_1 */

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); /* PA9 */

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); /* PA10 */

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); /* PC10 */

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); /* PE0-PE1 */
}

static bool AddKeyToReport(uint8_t kc)
{
	unsigned i;
	for (i=2; i<8; i++) {
		if (HIDReport[i] == kc)
			return true;
		else if(HIDReport[i] == 0) {
			HIDReport[i] = kc;
			return true;
		}
	}
	return false;
}

static bool RemoveKeyFromReport(uint8_t kc)
{
	unsigned i;
	for (i=2; i<8; i++) {
		if (HIDReport[i] == kc) {
			while (i < 7) {
				if (!(HIDReport[i] = HIDReport[i+1]))
					break;
				i++;
			}
			HIDReport[i] = 0;
			return true;
		}
		else if(HIDReport[i] == 0)
			break;
	}
	return false;
}

static void KeyDown(unsigned row, unsigned column)
{
	LED_Set_Key_RGB(row, column, 0x1800, 0x1800, 0x1800);
	if (row != 0 && row < 10 && column < 14) {
		uint8_t kc = KeyCodes[row-1][column];
		if (!kc)
			;
		else if (kc < 0xe0) {
			if (!AddKeyToReport(kc))
				HIDReportOverflow = true;
		} else if (kc < 0xe8) {
			HIDReport[0] |= 1 << (kc & 7);
		}
	}
}

static void KeyUp(unsigned row, unsigned column)
{
	LED_Set_Key_RGB(row, column, 0, 0, 0);
	if (row != 0 && row < 10 && column < 14) {
		uint8_t kc = KeyCodes[row-1][column];
		if (!kc)
			;
		else if (kc < 0xe0) {
			if (RemoveKeyFromReport(kc))
				HIDReportOverflow = false;
		} else if (kc < 0xe8) {
			HIDReport[0] &= ~(1 << (kc & 7));
		}
	}
}

void ADC_MaskCallback(uint8_t column, uint16_t mask)
{
	static uint16_t LastKeyMask[14];

	mask ^= LastKeyMask[column];

	if (mask) {
		int row;
		uint16_t new_mask = LastKeyMask[column] ^ mask;
		LastKeyMask[column] = new_mask;
		for (row = 0; mask; row++) {
			if ((mask & 1)) {
				if ((new_mask & 1))
					KeyDown(row, column);
				else
					KeyUp(row, column);
			}
			mask >>= 1;
			new_mask >>= 1;
		}
	}
	if (column == 13) {
		if (HIDReportOverflow) {
			static uint8_t overflow_report[8] = "\0\0\1\1\1\1\1\1";
			overflow_report[0] = HIDReport[0];
			USB_HIDInReportSubmit(overflow_report);
		} else
			USB_HIDInReportSubmit(HIDReport);
	}
}

void TIM_EncoderCallback(uint8_t value)
{
	unsigned hue = value * 6;
	unsigned r = 0, g = 0, b = 0;
	unsigned x = hue & 0x100? ~hue & 0xff : hue & 0xff;
	switch (hue >> 8) {
	case 0:
		r = 0xff;
		g = x;
		break;
	case 1:
		r = x;
		g = 0xff;
		break;
	case 2:
		g = 0xff;
		b = x;
		break;
	case 3:
		g = x;
		b = 0xff;
		break;
	case 4:
		r = x;
		b = 0xff;
		break;
	case 5:
		r = 0xff;
		b = x;
		break;
	}	
	LED_Set_Key_RGB(9, 0, r<<5, g<<5, b<<5);
}

void USB_HIDOutReportCallback(const uint8_t *report)
{
	static const uint8_t LED_row[4] = { 12, 10, 8, 13 };
	unsigned i, mask = *report;
	for (i=0; i<4; i++) {
		if (mask & 1)
			LED_Set_LED_RGB(LED_row[i], 6, 0x1800, 0x1800, 0x1800);
		else
			LED_Set_LED_RGB(LED_row[i], 6, 0, 0, 0);
		mask >>= 1;
	}
}

int main()
{
	HAL_Init();
	SystemClock_Config();
	GPIO_Setup();
	DMA_Setup();
	ADC_Setup_ADC();
	TIM_Setup_TIM1();
	TIM_Setup_TIM2();
	TIM_Setup_TIM3();
	TIM_Setup_TIM4();
	TIM_Setup_TIM10();
	TIM_Setup_TIM11();
	USB_Setup_USB();
	SPI_Setup_SPI2();
	TIM_Setup_TIM9();

	ADC_Start(0);
	LED_Start();
	TIM_Start_Encoder();

	/* Blink the LED on the brighness adjustment key */
	while (1)
	{
		HAL_Delay(200);
		LED_Set_LED_RGB(13, 8, 0x2800, 0x2800, 0x2800);
		HAL_Delay(200);
		LED_Set_LED_RGB(13, 8, 0, 0, 0);
	}

	return 0;
}
