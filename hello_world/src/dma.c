#include <stdint.h>
#include <stm32f4xx.h>

#include "error.h"
#include "dma.h"

DMA_HandleTypeDef DMA_HandleStruct_SPI2RX;
DMA_HandleTypeDef DMA_HandleStruct_SPI2TX;
DMA_HandleTypeDef DMA_HandleStruct_ADC;

/**
* @brief This function handles DMA1 Stream 3 interrupts (SPI 2 RX)
*/
void DMA1_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&DMA_HandleStruct_SPI2RX);
}

/**
* @brief This function handles DMA1 Stream 4 interrupts (SPI 2 TX)
*/
void DMA1_Stream4_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&DMA_HandleStruct_SPI2TX);
}

/**
* @brief This function handles DMA2 Stream 0 interrupts (ADC)
*/
void DMA2_Stream0_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&DMA_HandleStruct_ADC);
}


/** DMA Setup
*/
void DMA_Setup(void)
{
	/* DMA 1 */

	__HAL_RCC_DMA1_CLK_ENABLE();

	/* Stream 3 */

	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

	/* Stream 4 */

	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	/* DMA 2 */

	__HAL_RCC_DMA2_CLK_ENABLE ();

	/* Stream 0 */

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}
