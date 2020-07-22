#include <stdint.h>
#include <string.h>
#include <stm32f4xx.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_tim.h>
#include <stm32f4xx_ll_spi.h>

#include "error.h"
#include "led.h"
#include "tim.h"
#include "spi.h"

static uint16_t LED_Mode;
static uint16_t LED_Update_Page;
static uint16_t LED_Update_Buffer[3][256];
static uint16_t LED_Update_Scratch_Readback[256];
static uint16_t LED_Status_Readback[17];
static uint16_t LED_Start_Buffer[16];


static const uint8_t LED_RGB_Map[9][16] = {
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
	{ 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1 },
	{ 2,2,2,1,0,0,0,0,0,0,0,0,2,2,1,1 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2 },
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
};


/**
* @brief This function is ran at the TIM10 update interrupt
*/
void LED_IRQHandler(void)
{
	if (LED_Mode == 0) {
		LED_Update_Buffer[0][0] = 0xa035;
		HAL_SPI_TransmitReceive_DMA(&SPI_HandleStruct_SPI2,
					    (uint8_t *)LED_Update_Buffer[LED_Update_Page],
					    (uint8_t *)LED_Update_Scratch_Readback, 256);
		if (++LED_Update_Page > 2)
			LED_Update_Page = 0;
	} else {
		int pulse_count, delay;
		uint16_t spi_word;

		LL_TIM_DisableCounter(TIM_HandleStruct_TIM10.Instance);
		LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3);
		LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3);
		LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_2);
		switch (LED_Mode) {
		case 1:
			for(pulse_count=19; pulse_count>=0; --pulse_count) {
				LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3);
				LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3);
			}
			LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_2);
			spi_word = 0x0000;
			HAL_SPI_Transmit(&SPI_HandleStruct_SPI2, (uint8_t *)&spi_word, 1, 0);
			spi_word = 0x003e;
			HAL_SPI_Transmit(&SPI_HandleStruct_SPI2, (uint8_t *)&spi_word, 1, 0);
			memset(LED_Status_Readback, 0, sizeof(LED_Status_Readback));
			for (delay = 0xffff; delay >= 0; --delay)
				if (!LL_SPI_IsActiveFlag_BSY(SPI_HandleStruct_SPI2.Instance))
					break;
			HAL_SPI_Receive(&SPI_HandleStruct_SPI2, (uint8_t *)LED_Status_Readback, 17, 1);
			break;
		case 2:
			for(pulse_count=19; pulse_count>=0; --pulse_count) {
				LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3);
				LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3);
			}
			LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_2);
			spi_word = 0x0000;
			HAL_SPI_Transmit(&SPI_HandleStruct_SPI2, (uint8_t *)&spi_word, 1, 0);
			spi_word = 0xfffe;
			HAL_SPI_Transmit(&SPI_HandleStruct_SPI2, (uint8_t *)&spi_word, 1, 0);
			for (delay = 0xffff; delay >= 0; --delay)
				if (!LL_SPI_IsActiveFlag_BSY(SPI_HandleStruct_SPI2.Instance))
					break;
			LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_2);
			for(pulse_count=7; pulse_count>=0; --pulse_count) {
				LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3);
				LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3);
			}
			LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_2);
			HAL_SPI_Transmit(&SPI_HandleStruct_SPI2, (uint8_t *)LED_Start_Buffer, 16, 2);
			for (delay = 0xffff; delay >= 0; --delay)
				if (!LL_SPI_IsActiveFlag_BSY(SPI_HandleStruct_SPI2.Instance))
					break;
		}
		LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3);
		LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3);
		LED_Mode = 0;
		LL_TIM_EnableCounter(TIM_HandleStruct_TIM10.Instance);
	}
}

static void LED_Start_TIM1(void)
{
	TIM_BreakDeadTimeConfigTypeDef TIM_BreakDeadTimeConfigStruct;

	uint16_t old_duty_ch1, old_duty_ch2, old_duty_ch3, old_duty_ch4;

	/* Re-initialize TIM1 */

	TIM_HandleStruct_TIM1.Instance               = TIM1;
	TIM_HandleStruct_TIM1.Init.Prescaler         = 0;
	TIM_HandleStruct_TIM1.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM1.Init.Period            = 99;
	TIM_HandleStruct_TIM1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	TIM_HandleStruct_TIM1.Init.RepetitionCounter = 0;
	CHECK_HAL_RESULT(HAL_TIM_Base_Init(&TIM_HandleStruct_TIM1));

	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1, 28);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2, 42);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3, 56);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4, 70);

	TIM_BreakDeadTimeConfigStruct.OffStateRunMode  = TIM_OSSR_DISABLE;
	TIM_BreakDeadTimeConfigStruct.OffStateIDLEMode = TIM_OSSI_DISABLE;
	TIM_BreakDeadTimeConfigStruct.LockLevel        = TIM_LOCKLEVEL_OFF;
	TIM_BreakDeadTimeConfigStruct.DeadTime         = 7;
	TIM_BreakDeadTimeConfigStruct.BreakState       = TIM_BREAK_DISABLE;
	TIM_BreakDeadTimeConfigStruct.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
	TIM_BreakDeadTimeConfigStruct.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
	CHECK_HAL_RESULT(HAL_TIMEx_ConfigBreakDeadTime(&TIM_HandleStruct_TIM1, &TIM_BreakDeadTimeConfigStruct));

	/* Start PWM TIM1 CH1-4 at a low duty cycle */

	old_duty_ch1 = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1);
	old_duty_ch2 = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2);
	old_duty_ch3 = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3);
	old_duty_ch4 = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1, TIM_BreakDeadTimeConfigStruct.DeadTime-1);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2, TIM_BreakDeadTimeConfigStruct.DeadTime-1);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3, TIM_BreakDeadTimeConfigStruct.DeadTime-1);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4, TIM_BreakDeadTimeConfigStruct.DeadTime-1);
	HAL_TIM_Base_Start(&TIM_HandleStruct_TIM1);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4);

	GPIOE->BSRR = GPIO_PIN_4;
	HAL_Delay(1);

	/* Ramp PWM TIM1 CH1-4 up to the desired duty values (28%, 42%, 56%, 70%) */

	while (__HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1) != old_duty_ch1 ||
	       __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2) != old_duty_ch2 ||
	       __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3) != old_duty_ch3 ||
	       __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4) != old_duty_ch4) {
		if (__HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1) != old_duty_ch1)
			__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1,
					      __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_1) + 1);
		if (__HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2) != old_duty_ch2)
			__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2,
					      __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_2) + 1);
		if (__HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3) != old_duty_ch3)
			__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3,
					      __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_3) + 1);
		if (__HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4) != old_duty_ch4)
			__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4,
					      __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM1, TIM_CHANNEL_4) + 1);
		HAL_Delay(1);
	}
}

static void LED_Set_Start_Packet(uint16_t value)
{
	uint16_t cword = (value << 4) | 0x0008;
	int i;
	for (i=0; i<16; i++)
		LED_Start_Buffer[i] = cword;
	LED_Mode = 2;
}

void LED_Start(void)
{
	uint16_t duty, old_duty, tim4_old_duty[2];

	tim4_old_duty[0] = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM4, TIM_CHANNEL_1);
	tim4_old_duty[1] = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM4, TIM_CHANNEL_2);

	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM4, TIM_CHANNEL_1, 5);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM4, TIM_CHANNEL_2, 5);


	/* Ramp up PWM TIM11 CH1 from 0 to the preset duty value (20%) */

	old_duty = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM11, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM11, TIM_CHANNEL_1, 0);
	HAL_TIM_Base_Start(&TIM_HandleStruct_TIM11);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM11, TIM_CHANNEL_1);

	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);

	HAL_Delay(500); /* Should wait for ADC1 CH15.2 > 2500 here (power good?),
			   but for now just sleep a bit instead... */

	for (duty = 0; duty <= old_duty; duty++) {
		HAL_Delay(10);
		__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM11, TIM_CHANNEL_1, duty);
	}

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);


	/* Ramp up PWM TIM2 CH3 from 0 to the preset duty value (100%) */

	old_duty = __HAL_TIM_GET_COMPARE(&TIM_HandleStruct_TIM2, TIM_CHANNEL_3);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM2, TIM_CHANNEL_3, 0);
	HAL_TIM_Base_Start(&TIM_HandleStruct_TIM2);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM2, TIM_CHANNEL_3);
	for (duty = 0; duty <= old_duty; duty++) {
		HAL_Delay(1);
		__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM2, TIM_CHANNEL_3, duty);
	}


	/* Start PWM TIM4 CH1 and CH2 */

	HAL_TIM_Base_Start(&TIM_HandleStruct_TIM4);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM4, TIM_CHANNEL_2);

	LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_5);


	LED_Start_TIM1();


	/* Restore PWM TIM4 CH1 & CH2 duty cycle (43.75% and 60.16%) */

	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM4, TIM_CHANNEL_1, tim4_old_duty[0]);
	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM4, TIM_CHANNEL_2, tim4_old_duty[1]);

	/* Start PWM TIM9 CH2 */

	HAL_TIM_Base_Start(&TIM_HandleStruct_TIM9);
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM9, TIM_CHANNEL_2);


	/* Start PWM TIM10 CH 1 */

	__HAL_TIM_SET_COMPARE(&TIM_HandleStruct_TIM10, TIM_CHANNEL_1,
			      __HAL_TIM_GET_AUTORELOAD(&TIM_HandleStruct_TIM10) - 299);
	HAL_Delay(6);
	/* First in one-shot mode */
	TIM10->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;
	HAL_TIM_PWM_Start(&TIM_HandleStruct_TIM10, TIM_CHANNEL_1);
	HAL_Delay(10);
	/* Switch to continuous PWM */
	TIM10->CR1 &= ~TIM_CR1_OPM;
	TIM10->CR1 |= TIM_CR1_CEN;

	HAL_Delay(300);

	LED_Set_Start_Packet(7);

	/* Set control words for update buffer */

	LED_Update_Buffer[0][0]   = 0xa005;
	LED_Update_Buffer[0][16]  = 0xa115;
	LED_Update_Buffer[0][32]  = 0xa225;
	LED_Update_Buffer[0][48]  = 0xa335;
	LED_Update_Buffer[0][64]  = 0xa445;
	LED_Update_Buffer[0][80]  = 0xa555;
	LED_Update_Buffer[0][96]  = 0xa665;
	LED_Update_Buffer[0][112] = 0xa775;
	LED_Update_Buffer[0][128] = 0xa885;
	LED_Update_Buffer[0][144] = 0xa995;
	LED_Update_Buffer[0][160] = 0xa0a5;
	LED_Update_Buffer[0][176] = 0xa0b5;
	LED_Update_Buffer[0][192] = 0xa0c5;
	LED_Update_Buffer[0][208] = 0xa0d5;
	LED_Update_Buffer[0][224] = 0x00e6;
	LED_Update_Buffer[0][240] = 0x0006;

	LED_Update_Buffer[1][224] = 0x00e5;
	LED_Update_Buffer[1][240] = 0x0005;

	LED_Update_Buffer[2][224] = 0x00e3;
	LED_Update_Buffer[2][240] = 0x0003;

	/* Enable interrupt */

	__HAL_TIM_ENABLE_IT(&TIM_HandleStruct_TIM10, TIM_IT_UPDATE);

	HAL_Delay(10);
}

void LED_Set_LED(unsigned row, unsigned col, uint16_t c0, uint16_t c1, uint16_t c2)
{
	if (row < 16 && col < 9) {
		LED_Update_Buffer[0][(row<<4)+col+7] = c0;
		LED_Update_Buffer[1][(row<<4)+col+7] = c1;
		LED_Update_Buffer[2][(row<<4)+col+7] = c2;
	}
}

void LED_Set_LED_RGB(unsigned row, unsigned col, uint16_t r, uint16_t g, uint16_t b)
{
	if (row < 16 && col < 9) {
		switch(LED_RGB_Map[col][row]) {
		case 0:
			LED_Update_Buffer[0][(row<<4)+col+7] = r;
			LED_Update_Buffer[1][(row<<4)+col+7] = g;
			LED_Update_Buffer[2][(row<<4)+col+7] = b;
			break;
		case 1:
			LED_Update_Buffer[0][(row<<4)+col+7] = b;
			LED_Update_Buffer[1][(row<<4)+col+7] = r;
			LED_Update_Buffer[2][(row<<4)+col+7] = g;
			break;
		case 2:
			LED_Update_Buffer[0][(row<<4)+col+7] = g;
			LED_Update_Buffer[1][(row<<4)+col+7] = b;
			LED_Update_Buffer[2][(row<<4)+col+7] = r;
			break;
		}
	}
}