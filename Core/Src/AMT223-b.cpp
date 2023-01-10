#include "AMT223-b.h"
#include "delay.h"
#include "bit_operations.h"
using namespace PSR;

bool amt223Check(uint16_t value)
{
	int k1 = bitExtract(value, 15);
	int k0 = bitExtract(value, 14);

	int checkOdd  = !(bitExtract(value, 13) ^ bitExtract(value, 11) ^ bitExtract(value, 9) ^ bitExtract(value, 7) ^ bitExtract(value, 5) ^ bitExtract(value, 3) ^ bitExtract(value, 1));
	int checkEven = !(bitExtract(value, 12) ^ bitExtract(value, 10) ^ bitExtract(value, 8) ^ bitExtract(value, 6) ^ bitExtract(value, 4) ^ bitExtract(value, 2) ^ bitExtract(value, 0));

	return ((k1 == checkOdd) && (k0 == checkEven));
}

HAL_StatusTypeDef amt223SendByte(SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim, uint8_t* sendByte, uint8_t* receiveByte, bool pullLow)
{
	HAL_StatusTypeDef checkStatus;

	if (pullLow)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		checkStatus = HAL_SPI_TransmitReceive(hspi, sendByte, receiveByte, 1, 50);
		PSR::delayMicroseconds(htim, 3);
	}
	else
	{
		checkStatus = HAL_SPI_TransmitReceive(hspi, sendByte, receiveByte, 1, 50);
		PSR::delayMicroseconds(htim, 3);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		PSR::delayMicroseconds(htim, 40);
	}

	return checkStatus;
}

bool amt223SendReceive(SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim, uint8_t* sendBytes, uint8_t* receiveBytes)
{
	bool checkStatus1 = amt223SendByte(hspi, htim, &sendBytes[0], &receiveBytes[0], true) == HAL_OK;
	bool checkStatus2 = amt223SendByte(hspi, htim, &sendBytes[1], &receiveBytes[1], false) == HAL_OK;

	return checkStatus1 && checkStatus2;
}

void amt223Reset(SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim)
{
	uint8_t spi_Tx[2] = { 0x00, 0x70 };
	uint8_t spi_Rx[2];

	while (true)
	{
		if (amt223SendReceive(hspi, htim, spi_Tx, spi_Rx))
		{
			uint16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];
			if (amt223Check(finalPosition))
				break;
		}
	}
}
