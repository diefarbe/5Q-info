#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f4xx_ll_gpio.h>

#include "error.h"
#include "adc.h"
#include "dma.h"

static uint8_t ADC_Column;
static int16_t ADC_Readback_Buffer[12];

ADC_HandleTypeDef ADC_HandleStruct;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  int i;
  uint16_t mask = 0;
  int16_t cutoff = ADC_Readback_Buffer[0];
  for (i=2; i<11; i++) {
    mask >>= 1;
    if (ADC_Readback_Buffer[i] < cutoff)
      mask |= (1<<9);
  }
  unsigned next_col = ADC_Column + 1;
  if (next_col >= 14)
    next_col = 0;
  ADC_MaskCallback(ADC_Column, mask);
  ADC_Start(next_col);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  if (hadc->Instance == ADC1) {
    __HAL_RCC_ADC1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    DMA_HandleStruct_ADC.Instance = DMA2_Stream0;
    DMA_HandleStruct_ADC.Init.Channel = 0;
    DMA_HandleStruct_ADC.Init.Direction = DMA_PERIPH_TO_MEMORY;
    DMA_HandleStruct_ADC.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA_HandleStruct_ADC.Init.MemInc = DMA_MINC_ENABLE;
    DMA_HandleStruct_ADC.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    DMA_HandleStruct_ADC.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    DMA_HandleStruct_ADC.Init.Mode = DMA_CIRCULAR;
    DMA_HandleStruct_ADC.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    CHECK_HAL_RESULT(HAL_DMA_Init(&DMA_HandleStruct_ADC));
    hadc->DMA_Handle = &DMA_HandleStruct_ADC;
    DMA_HandleStruct_ADC.Parent = hadc;
  }
}

extern void ADC_Setup_ADC(void)
{
  ADC_ChannelConfTypeDef ADC_ChannelConfigStruct;

  ADC_HandleStruct.Instance = ADC1;
  ADC_HandleStruct.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  ADC_HandleStruct.Init.Resolution = ADC_RESOLUTION_12B;
  ADC_HandleStruct.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  ADC_HandleStruct.Init.ScanConvMode = ENABLE;
  ADC_HandleStruct.Init.ContinuousConvMode = DISABLE;
  ADC_HandleStruct.Init.NbrOfConversion = 12;
  ADC_HandleStruct.Init.DMAContinuousRequests = ENABLE;
  CHECK_HAL_RESULT(HAL_ADC_Init(&ADC_HandleStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_0;
  ADC_ChannelConfigStruct.Rank = 1;
  ADC_ChannelConfigStruct.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  ADC_ChannelConfigStruct.Offset = 0;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_1;
  ADC_ChannelConfigStruct.Rank = 2;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_2;
  ADC_ChannelConfigStruct.Rank = 3;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_3;
  ADC_ChannelConfigStruct.Rank = 4;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_4;
  ADC_ChannelConfigStruct.Rank = 5;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_5;
  ADC_ChannelConfigStruct.Rank = 6;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_6;
  ADC_ChannelConfigStruct.Rank = 7;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_7;
  ADC_ChannelConfigStruct.Rank = 8;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_8;
  ADC_ChannelConfigStruct.Rank = 9;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_9;
  ADC_ChannelConfigStruct.Rank = 10;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_10;
  ADC_ChannelConfigStruct.Rank = 11;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));

  ADC_ChannelConfigStruct.Channel = ADC_CHANNEL_11;
  ADC_ChannelConfigStruct.Rank = 12;
  ADC_ChannelConfigStruct.SamplingTime = ADC_SAMPLETIME_28CYCLES;
  CHECK_HAL_RESULT(HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelConfigStruct));
}

extern void ADC_Start(uint8_t column)
{
  ADC_Column = column;
  GPIOD->ODR = ~(GPIO_PIN_2 << column);
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);
  HAL_ADC_Start_DMA(&ADC_HandleStruct, (uint32_t *)ADC_Readback_Buffer, 12);
}
