#include "AMT223-b.h"
#include "delay.h"
#include "bit_operations.h"
using namespace PSR;

/**
 * @brief Executes the checkbit value from the AMT223 data
 *
 * @param value data to be checked
 * @return Status indicating if the data passed the formula test
 */
bool amt223Check(uint16_t value)
{
	int k1 = bitExtract(value, 15);
	int k0 = bitExtract(value, 14);

	int checkOdd  = !(bitExtract(value, 13) ^ bitExtract(value, 11) ^ bitExtract(value, 9) ^ bitExtract(value, 7) ^ bitExtract(value, 5) ^ bitExtract(value, 3) ^ bitExtract(value, 1));
	int checkEven = !(bitExtract(value, 12) ^ bitExtract(value, 10) ^ bitExtract(value, 8) ^ bitExtract(value, 6) ^ bitExtract(value, 4) ^ bitExtract(value, 2) ^ bitExtract(value, 0));

	return ((k1 == checkOdd) && (k0 == checkEven));
}

/**
 * @brief Send a byte of data to the AMT223b optical encoder and get the data back
 *
 * @param sendByte byte to be sent
 * @param pullLow indicating if the CS line should be pulled low
 * @param hspi pointer to the SPI setup
 * @param htim pointer to the timer
 * @param receiveByte data byte recieved from the optical encoder
 * 
 * @return HAL_Status of the byte that was sent
 */
HAL_StatusTypeDef sendByte(uint8_t* sendByte, bool pullLow, SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim, uint8_t* receiveByte)
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
/**
 * @brief Reset the AMT223b optical encoder
 *
 * @param hspi pointer to the SPI setup 
 * @param htim pointer to the timer
 * @return VOID
 */
void resetEncoder(SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim)
{
	bool checkValue = false;

	HAL_StatusTypeDef checkHALStatus;

	uint8_t spi_Tx[2];
	uint8_t spi_Rx[2];

	spi_Tx[0] = 0x00;
	spi_Tx[1] = 0x70;

	while (!checkValue)
	{
		checkHALStatus = sendByte(&spi_Tx[0], true, hspi, htim, &spi_Rx[0]);
		if (checkHALStatus == HAL_OK)
		{
			checkHALStatus = sendByte(&spi_Tx[1], false, hspi, htim, &spi_Rx[1]);
			if (checkHALStatus == HAL_OK)
			{
				int16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];
				checkValue            = amt223Check(finalPosition);
			}
		}
	}
}
