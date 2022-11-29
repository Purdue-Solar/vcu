#include "AMT223-b.h"
//TODO Add Function headers

/**
* @brief Extracts a bit from the data
*
* @param value: value to be extracted from, bit: bit location to be extracted
*
* @return extracted bit
*/
int bitExtract(int value, int bit)
{
	return((value>>bit) & 1);
}
#define bx bitExtract
/**
* @brief Executes the checkbit value from the AMT223 data
*
* @param value: data to be checked
* @return Status indicating if the data passed the formula test
*/
bool amt223Check(uint16_t value)
{

	int k1 = bitExtract(value, 15);
	int k0 = bitExtract(value, 14);

	int checkOdd = !(bx(value,13)^bx(value,11)^bx(value,9)^bx(value,7)^bx(value,5)^bx(value,3)^bx(value,1));
	int checkEven = !(bx(value,12)^bx(value,10)^bx(value,8)^bx(value,6)^bx(value,4)^bx(value,2)^bx(value,0));

	return((k1 == checkOdd) && (k0 == checkEven));
}

/**
* @brief Delays in microseconds
*
* @param us: amount of microseconds to delay, htim: pointer to the timer
* @return VOID
*/
void delayMicro(uint16_t us, TIM_HandleTypeDef * htim)
{
	//TODO Implement common function on Github
	__HAL_TIM_SET_COUNTER(htim,0); // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(htim) < us); // wait for the counter to reach the us input in the parameter
}


/**
* @brief Send a byte of data to the AMT223b optical encoder and get the data back
*
* @param sendByte: byte to be sent, pullLow: indicating if the CS line should be pulled low, hspi: pointer to the SPI setup, htim: pointer to the timer, receiveByte: data byte recieved from the optical encoder
* @return HAL_Status of the byte that was sent
*/
HAL_StatusTypeDef sendByte(uint8_t * sendByte, bool pullLow, SPI_HandleTypeDef * hspi, TIM_HandleTypeDef * htim, uint8_t * receiveByte)
{
	HAL_StatusTypeDef checkStatus;

	if(pullLow)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		checkStatus = HAL_SPI_TransmitReceive(hspi, sendByte, receiveByte,1,50);
		delayMicro(3,htim);
	}
	else
	{
		checkStatus = HAL_SPI_TransmitReceive(hspi,sendByte,receiveByte,1,50);
		delayMicro(3,htim);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
		delayMicro(40,htim);
	}
	return checkStatus;
}
/**
* @brief Reset the AMT223b optical encoder
*
* @param hspi: pointer to the SPI setup, htim: pointer to the timer
* @return VOID
*/
void resetEncoder(SPI_HandleTypeDef * hspi, TIM_HandleTypeDef * htim)
{
	bool checkValue = false;

	HAL_StatusTypeDef checkHALStatus;

	uint8_t spi_Tx[2];
	uint8_t spi_Rx[2];

	spi_Tx[0] = 0x00;
	spi_Tx[1] = 0x70;

	while(!checkValue)
	{
		checkHALStatus = sendByte(&spi_Tx[0],true,hspi,htim,&spi_Rx[0]);
		if(checkHALStatus == HAL_OK)
		{
			checkHALStatus = sendByte(&spi_Tx[1],false,hspi,htim,&spi_Rx[1]);
			if(checkHALStatus == HAL_OK)
			{
				int16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];
				checkValue = amt223Check(finalPosition);
			}
		}
	}
}
