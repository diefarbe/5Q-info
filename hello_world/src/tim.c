#include <stdint.h>
#include <stm32f4xx.h>

#include "error.h"
#include "tim.h"

TIM_HandleTypeDef TIM_HandleStruct_TIM1;
TIM_HandleTypeDef TIM_HandleStruct_TIM2;
TIM_HandleTypeDef TIM_HandleStruct_TIM3;
TIM_HandleTypeDef TIM_HandleStruct_TIM4;
TIM_HandleTypeDef TIM_HandleStruct_TIM9;
TIM_HandleTypeDef TIM_HandleStruct_TIM10;
TIM_HandleTypeDef TIM_HandleStruct_TIM11;


/**
* @brief This function handles TIM1 update and TIM10 interrupts
*/
void TIM1_UP_TIM10_IRQHandler(void)
{
	if (__HAL_TIM_GET_FLAG(&TIM_HandleStruct_TIM10, TIM_FLAG_UPDATE) != RESET) {
		__HAL_TIM_CLEAR_IT(&TIM_HandleStruct_TIM10, TIM_IT_UPDATE);
	} else {
		HAL_TIM_IRQHandler(&TIM_HandleStruct_TIM1);
		HAL_TIM_IRQHandler(&TIM_HandleStruct_TIM10);
	}
}

/**
* @brief This function handles TIM3 interrupts
*/
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM_HandleStruct_TIM3);
}


static void TIM_Config_PWM_GPIO(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	if (htim->Instance == TIM1) {
		GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	} else if (htim->Instance == TIM2) {
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	} else if (htim->Instance == TIM4) {
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	} else if (htim->Instance == TIM9) {
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	} else if (htim->Instance == TIM10) {
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM10;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	} else if (htim->Instance == TIM11) {
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM11;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	if (htim->Instance == TIM3) {
		__HAL_RCC_TIM3_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	/* Note:

	   This function is non run for any PWM timers
	   other than TIM9 because they call HAL_TIM_Base_Init()
	   before the call to HAL_TIM_PWM_Init().  Therefore
	   HAL_TIM_Base_MspInit() is ran for those instead.

	   GPIO setup is not done in either of these two functions,
	   but rather in the TIM_Config_PWM_GPIO() function which
	   is called manually.
	*/

	if (htim->Instance == TIM9) {
		__HAL_RCC_TIM9_CLK_ENABLE();
	}
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1) {
		__HAL_RCC_TIM1_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	} else if (htim->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_ENABLE();
	} else if (htim->Instance == TIM4) {
		__HAL_RCC_TIM4_CLK_ENABLE();
	} else if (htim->Instance == TIM10) {
		__HAL_RCC_TIM10_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	} else if (htim->Instance == TIM11) {
		__HAL_RCC_TIM11_CLK_ENABLE();
	}
}


/** Timer TIM1 Setup
*/
void TIM_Setup_TIM1(void)
{
	TIM_ClockConfigTypeDef TIM_ClockConfigStruct;
	TIM_MasterConfigTypeDef TIM_MasterConfigStruct;
	TIM_OC_InitTypeDef TIM_OC_InitStruct;
	TIM_BreakDeadTimeConfigTypeDef TIM_BreakDeadTimeConfigStruct;

	TIM_HandleStruct_TIM1.Instance               = TIM1;
	TIM_HandleStruct_TIM1.Init.Prescaler         = 0;
	TIM_HandleStruct_TIM1.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM1.Init.Period            = 99;
	TIM_HandleStruct_TIM1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	TIM_HandleStruct_TIM1.Init.RepetitionCounter = 0;
	CHECK_HAL_RESULT(HAL_TIM_Base_Init(&TIM_HandleStruct_TIM1));

	TIM_ClockConfigStruct.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	CHECK_HAL_RESULT(HAL_TIM_ConfigClockSource(&TIM_HandleStruct_TIM1, &TIM_ClockConfigStruct));

	CHECK_HAL_RESULT(HAL_TIM_PWM_Init(&TIM_HandleStruct_TIM1));

	TIM_MasterConfigStruct.MasterOutputTrigger = TIM_TRGO_RESET;
	TIM_MasterConfigStruct.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	CHECK_HAL_RESULT(HAL_TIMEx_MasterConfigSynchronization(&TIM_HandleStruct_TIM1, &TIM_MasterConfigStruct));

	/* PWM CH1 */
	TIM_OC_InitStruct.OCMode       = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.Pulse        = 70;
	TIM_OC_InitStruct.OCPolarity   = TIM_OCPOLARITY_HIGH;
	TIM_OC_InitStruct.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	TIM_OC_InitStruct.OCFastMode   = TIM_OCFAST_DISABLE;
	TIM_OC_InitStruct.OCIdleState  = TIM_OCIDLESTATE_RESET;
	TIM_OC_InitStruct.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM1, &TIM_OC_InitStruct, TIM_CHANNEL_1));

	/* PWM CH2 */
	TIM_OC_InitStruct.Pulse        = 78;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM1, &TIM_OC_InitStruct, TIM_CHANNEL_2));

	/* PWM CH3 */
	TIM_OC_InitStruct.Pulse        = 86;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM1, &TIM_OC_InitStruct, TIM_CHANNEL_3));

	/* PWM CH4 */
	TIM_OC_InitStruct.Pulse        = 24;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM1, &TIM_OC_InitStruct, TIM_CHANNEL_4));

	TIM_BreakDeadTimeConfigStruct.OffStateRunMode  = TIM_OSSR_DISABLE;
	TIM_BreakDeadTimeConfigStruct.OffStateIDLEMode = TIM_OSSI_DISABLE;
	TIM_BreakDeadTimeConfigStruct.LockLevel        = TIM_LOCKLEVEL_OFF;
	TIM_BreakDeadTimeConfigStruct.DeadTime         = 4;
	TIM_BreakDeadTimeConfigStruct.BreakState       = TIM_BREAK_DISABLE;
	TIM_BreakDeadTimeConfigStruct.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
	TIM_BreakDeadTimeConfigStruct.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
	CHECK_HAL_RESULT(HAL_TIMEx_ConfigBreakDeadTime(&TIM_HandleStruct_TIM1, &TIM_BreakDeadTimeConfigStruct));

	TIM_Config_PWM_GPIO(&TIM_HandleStruct_TIM1);
}

/** Timer TIM2 Setup
*/
void TIM_Setup_TIM2(void)
{
	TIM_ClockConfigTypeDef TIM_ClockConfigStruct;
	TIM_MasterConfigTypeDef TIM_MasterConfigStruct;
	TIM_OC_InitTypeDef TIM_OC_InitStruct;

	TIM_HandleStruct_TIM2.Instance           = TIM2;
	TIM_HandleStruct_TIM2.Init.Prescaler     = 0;
	TIM_HandleStruct_TIM2.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM2.Init.Period        = 99;
	TIM_HandleStruct_TIM2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	CHECK_HAL_RESULT(HAL_TIM_Base_Init(&TIM_HandleStruct_TIM2));

	TIM_ClockConfigStruct.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	CHECK_HAL_RESULT(HAL_TIM_ConfigClockSource(&TIM_HandleStruct_TIM2, &TIM_ClockConfigStruct));

	CHECK_HAL_RESULT(HAL_TIM_PWM_Init(&TIM_HandleStruct_TIM2));

	TIM_MasterConfigStruct.MasterOutputTrigger = TIM_TRGO_RESET;
	TIM_MasterConfigStruct.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	CHECK_HAL_RESULT(HAL_TIMEx_MasterConfigSynchronization(&TIM_HandleStruct_TIM2, &TIM_MasterConfigStruct));

	/* PWM CH3 */
	TIM_OC_InitStruct.OCMode     = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.Pulse      = 100;
	TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_LOW;
	TIM_OC_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM2, &TIM_OC_InitStruct, TIM_CHANNEL_3));

	TIM_Config_PWM_GPIO(&TIM_HandleStruct_TIM2);
}

/** Timer TIM3 Setup
*/
void TIM_Setup_TIM3(void)
{
	TIM_Encoder_InitTypeDef TIM_Encoder_InitStruct;
	TIM_MasterConfigTypeDef TIM_MasterConfigStruct;

	TIM_HandleStruct_TIM3.Instance           = TIM3;
	TIM_HandleStruct_TIM3.Init.Prescaler     = 0;
	TIM_HandleStruct_TIM3.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM3.Init.Period        = 255;
	TIM_HandleStruct_TIM3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM_Encoder_InitStruct.EncoderMode  = TIM_ENCODERMODE_TI12;
	TIM_Encoder_InitStruct.IC1Polarity  = TIM_ICPOLARITY_RISING;
	TIM_Encoder_InitStruct.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	TIM_Encoder_InitStruct.IC1Prescaler = TIM_ICPSC_DIV1;
	TIM_Encoder_InitStruct.IC1Filter    = 0x0;
	TIM_Encoder_InitStruct.IC2Polarity  = TIM_ICPOLARITY_RISING;
	TIM_Encoder_InitStruct.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	TIM_Encoder_InitStruct.IC2Prescaler = TIM_ICPSC_DIV1;
	TIM_Encoder_InitStruct.IC2Filter    = 0x0;
	CHECK_HAL_RESULT(HAL_TIM_Encoder_Init(&TIM_HandleStruct_TIM3, &TIM_Encoder_InitStruct));

	TIM_MasterConfigStruct.MasterOutputTrigger = TIM_TRGO_RESET;
	TIM_MasterConfigStruct.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	CHECK_HAL_RESULT(HAL_TIMEx_MasterConfigSynchronization(&TIM_HandleStruct_TIM3, &TIM_MasterConfigStruct));
}

/** Timer TIM4 Setup
*/
void TIM_Setup_TIM4(void)
{
	TIM_ClockConfigTypeDef TIM_ClockConfigStruct;
	TIM_MasterConfigTypeDef TIM_MasterConfigStruct;
	TIM_OC_InitTypeDef TIM_OC_InitStruct;

	TIM_HandleStruct_TIM4.Instance           = TIM4;
	TIM_HandleStruct_TIM4.Init.Prescaler     = 0;
	TIM_HandleStruct_TIM4.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM4.Init.Period        = 255;
	TIM_HandleStruct_TIM4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	CHECK_HAL_RESULT(HAL_TIM_Base_Init(&TIM_HandleStruct_TIM4));

	TIM_ClockConfigStruct.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	CHECK_HAL_RESULT(HAL_TIM_ConfigClockSource(&TIM_HandleStruct_TIM4, &TIM_ClockConfigStruct));

	CHECK_HAL_RESULT(HAL_TIM_PWM_Init(&TIM_HandleStruct_TIM4));

	TIM_MasterConfigStruct.MasterOutputTrigger = TIM_TRGO_RESET;
	TIM_MasterConfigStruct.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	CHECK_HAL_RESULT(HAL_TIMEx_MasterConfigSynchronization(&TIM_HandleStruct_TIM4, &TIM_MasterConfigStruct));

	/* PWM CH1 */
	TIM_OC_InitStruct.OCMode     = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.Pulse      = 112;
	TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OC_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM4, &TIM_OC_InitStruct, TIM_CHANNEL_1));

	/* PWM CH2 */
	TIM_OC_InitStruct.Pulse      = 154;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM4, &TIM_OC_InitStruct, TIM_CHANNEL_2));

	TIM_Config_PWM_GPIO(&TIM_HandleStruct_TIM4);
}

/** Timer TIM9 Setup
*/
void TIM_Setup_TIM9(void)
{
	TIM_OC_InitTypeDef TIM_OC_InitStruct;

	TIM_HandleStruct_TIM9.Instance           = TIM9;
	TIM_HandleStruct_TIM9.Init.Prescaler     = 0;
	TIM_HandleStruct_TIM9.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM9.Init.Period        = 1024;
	TIM_HandleStruct_TIM9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	CHECK_HAL_RESULT(HAL_TIM_PWM_Init(&TIM_HandleStruct_TIM9));

	/* PWM CH2 */
	TIM_OC_InitStruct.OCMode     = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.Pulse      = 80;
	TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OC_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM9, &TIM_OC_InitStruct, TIM_CHANNEL_2));

	TIM_Config_PWM_GPIO(&TIM_HandleStruct_TIM9);
}

/** Timer TIM10 Setup
*/
void TIM_Setup_TIM10(void)
{
	TIM_OC_InitTypeDef TIM_OC_InitStruct;

	TIM_HandleStruct_TIM10.Instance           = TIM10;
	TIM_HandleStruct_TIM10.Init.Prescaler     = 4;
	TIM_HandleStruct_TIM10.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM10.Init.Period        = 55999;
	TIM_HandleStruct_TIM10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	CHECK_HAL_RESULT(HAL_TIM_Base_Init(&TIM_HandleStruct_TIM10));

	CHECK_HAL_RESULT(HAL_TIM_PWM_Init(&TIM_HandleStruct_TIM10));

	/* PWM CH1 */
	TIM_OC_InitStruct.OCMode     = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.Pulse      = 55140;
	TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_LOW;
	TIM_OC_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM10, &TIM_OC_InitStruct, TIM_CHANNEL_1));

	TIM_Config_PWM_GPIO(&TIM_HandleStruct_TIM10);
}

/** Timer TIM11 Setup
*/
void TIM_Setup_TIM11(void)
{
	TIM_OC_InitTypeDef TIM_OC_InitStruct;

	TIM_HandleStruct_TIM11.Instance           = TIM11;
	TIM_HandleStruct_TIM11.Init.Prescaler     = 0;
	TIM_HandleStruct_TIM11.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM_HandleStruct_TIM11.Init.Period        = 49;
	TIM_HandleStruct_TIM11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	CHECK_HAL_RESULT(HAL_TIM_Base_Init(&TIM_HandleStruct_TIM11));

	CHECK_HAL_RESULT(HAL_TIM_PWM_Init(&TIM_HandleStruct_TIM11));

	/* PWM CH1 */
	TIM_OC_InitStruct.OCMode     = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.Pulse      = 10;
	TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OC_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	CHECK_HAL_RESULT(HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct_TIM11, &TIM_OC_InitStruct, TIM_CHANNEL_1));

	TIM_Config_PWM_GPIO(&TIM_HandleStruct_TIM11);
}
