#include "AMT223-b.h"
//TODO Add Function headers

/**
* @brief Transmit a CAN frame
*
* @param frame The frame data to send
* @return A status representing whether the frame was successfully sent
*/
int bitExtract(int value, int bit)
{
	return((value>>bit) & 1);
}
#define bx bitExtract
/**
* @brief Transmit a CAN frame
*
* @param frame The frame data to send
* @return A status representing whether the frame was successfully sent
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
* @brief Transmit a CAN frame
*
* @param frame The frame data to send
* @return A status representing whether the frame was successfully sent
*/
void delayMicro(uint16_t us, TIM_HandleTypeDef * htim)
{
	//TODO Implement common function on Github
	__HAL_TIM_SET_COUNTER(htim,0); // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(htim) < us); // wait for the counter to reach the us input in the parameter
}


/**
* @brief Transmit a CAN frame
*
* @param frame The frame data to send
* @return A status representing whether the frame was successfully sent
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
