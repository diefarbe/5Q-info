#include <stdint.h>
#include <stm32f4xx.h>

#include "error.h"
#include "spi.h"
#include "dma.h"

SPI_HandleTypeDef SPI_HandleStruct_SPI2;

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	if (hspi->Instance == SPI2) {
		__HAL_RCC_SPI2_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_14;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* TX DMA */
		DMA_HandleStruct_SPI2TX.Instance                 = DMA1_Stream4;
		DMA_HandleStruct_SPI2TX.Init.Channel             = 0;
		DMA_HandleStruct_SPI2TX.Init.Direction           = DMA_MEMORY_TO_PERIPH;
		DMA_HandleStruct_SPI2TX.Init.PeriphInc           = DMA_PINC_DISABLE;
		DMA_HandleStruct_SPI2TX.Init.MemInc              = DMA_MINC_ENABLE;
		DMA_HandleStruct_SPI2TX.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		DMA_HandleStruct_SPI2TX.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
		DMA_HandleStruct_SPI2TX.Init.Mode                = DMA_NORMAL;
		DMA_HandleStruct_SPI2TX.Init.Priority            = DMA_PRIORITY_LOW;
		DMA_HandleStruct_SPI2TX.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
		CHECK_HAL_RESULT(HAL_DMA_Init(&DMA_HandleStruct_SPI2TX));
		hspi->hdmatx = &DMA_HandleStruct_SPI2TX;
		DMA_HandleStruct_SPI2TX.Parent = hspi;

		/* RX DMA */
		DMA_HandleStruct_SPI2RX.Instance                 = DMA1_Stream3;
		DMA_HandleStruct_SPI2RX.Init.Channel             = 0;
		DMA_HandleStruct_SPI2RX.Init.Direction           = DMA_PERIPH_TO_MEMORY;
		DMA_HandleStruct_SPI2RX.Init.PeriphInc           = DMA_PINC_DISABLE;
		DMA_HandleStruct_SPI2RX.Init.MemInc              = DMA_MINC_ENABLE;
		DMA_HandleStruct_SPI2RX.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		DMA_HandleStruct_SPI2RX.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
		DMA_HandleStruct_SPI2RX.Init.Mode                = DMA_NORMAL;
		DMA_HandleStruct_SPI2RX.Init.Priority            = DMA_PRIORITY_LOW;
		DMA_HandleStruct_SPI2RX.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
		CHECK_HAL_RESULT(HAL_DMA_Init(&DMA_HandleStruct_SPI2RX));
		hspi->hdmarx = &DMA_HandleStruct_SPI2RX;
		DMA_HandleStruct_SPI2RX.Parent = hspi;
	}
}


/** SPI SPI2 Setup
*/
void SPI_Setup_SPI2(void)
{
	SPI_HandleStruct_SPI2.Instance               = SPI2;
	SPI_HandleStruct_SPI2.Init.Mode              = SPI_MODE_MASTER;
	SPI_HandleStruct_SPI2.Init.Direction         = SPI_DIRECTION_2LINES;
	SPI_HandleStruct_SPI2.Init.DataSize          = SPI_DATASIZE_16BIT;
	SPI_HandleStruct_SPI2.Init.CLKPolarity       = SPI_POLARITY_LOW;
	SPI_HandleStruct_SPI2.Init.CLKPhase          = SPI_PHASE_1EDGE;
	SPI_HandleStruct_SPI2.Init.NSS               = SPI_NSS_SOFT;
	SPI_HandleStruct_SPI2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	SPI_HandleStruct_SPI2.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SPI_HandleStruct_SPI2.Init.TIMode            = SPI_TIMODE_DISABLE;
	SPI_HandleStruct_SPI2.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SPI_HandleStruct_SPI2.Init.CRCPolynomial     = 10;
	CHECK_HAL_RESULT(HAL_SPI_Init(&SPI_HandleStruct_SPI2));
}
