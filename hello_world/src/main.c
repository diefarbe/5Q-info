#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* ADC handler declaration */
ADC_HandleTypeDef AdcHandle;

/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 0;

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 144000000
  *            HCLK(Hz)                       = 144000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 288
  *            PLL_P                          = 2
  *            PLL_Q                          = 6
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  AdcHandle : AdcHandle handle
  * @note   This example shows a simple way to report end of conversion, and 
  *         you can add your own implementation.    
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
	while (1)
	{
		HAL_Delay(40);
	}
}

int main()
{

	ADC_MultiModeTypeDef sConfig;

	HAL_Init();

	SystemClock_Config();

	/*##-1- Configure the ADC peripheral #######################################*/
	AdcHandle.Instance = ADC1;

	AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
	AdcHandle.Init.ScanConvMode = DISABLE;
	AdcHandle.Init.ContinuousConvMode = ENABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.NbrOfDiscConversion = 0;
	AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion = 1;
	AdcHandle.Init.DMAContinuousRequests = ENABLE;
	AdcHandle.Init.EOCSelection = DISABLE;

	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		while (1)
		{
			HAL_Delay(40);
		}
	}

	/*##-2- Configure ADC regular channel ######################################*/
	/* Note: Considering IT occurring after each number of size of              */
	/*       "uhADCxConvertedValue"  ADC conversions (IT by DMA end             */
	/*       of transfer), select sampling time and ADC clock with sufficient   */
	/*       duration to not create an overhead situation in IRQHandler.        */
	sConfig.Mode = ADC_MODE_INDEPENDENT;
	sConfig.DMAAccessMode = ADC_DMAACCESSMODE_1;
	sConfig.TwoSamplingDelay = 0;

	HAL_StatusTypeDef data = HAL_ADCEx_MultiModeConfigChannel(&AdcHandle, &sConfig);
	if (data != HAL_OK)
	{
		while (1)
		{
			HAL_Delay(40);
		}
	}

	/*##-3- Start the conversion process and enable interrupt ##################*/
	/* Note: Considering IT occurring after each number of ADC conversions      */
	/*       (IT by DMA end of transfer), select sampling time and ADC clock    */
	/*       with sufficient duration to not create an overhead situation in    */
	/*        IRQHandler. */
	HAL_StatusTypeDef ret = HAL_ADCEx_MultiModeStart_DMA(&AdcHandle, (uint32_t *)&uhADCxConvertedValue, 1);

	if (ret != HAL_OK)
	{
		while (1)
		{
			HAL_Delay(40);
		}
	}

	int i = 0;
	while (1)
	{
		i++;
	}

	return 0;
}
