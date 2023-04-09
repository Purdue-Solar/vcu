#include "AMT223-b.h"
#include "bit_operations.h"
#include "can_lib.h"
#include "delay.h"
#include "stm32_includer.h"
#include "vesc.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)
#include STM32_INCLUDE(BOARD_STM32, hal_tim.h)
#include STM32_INCLUDE(BOARD_STM32, hal_gpio.h)

#include <cstdio>
#include <cstring>

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

void amt223Setup(TIM_HandleTypeDef* timer1MHz, SPI_HandleTypeDef* encoderSPI)
{
	timer1MHz->Instance->CR1 |= TIM_CR1_CEN;

	// SPI Setup
	uint8_t spi_Tx[2];
	uint8_t spi_Rx[2];

	uint16_t position = 0xFFFF;

	spi_Tx[0] = 0x00;
	spi_Tx[1] = 0x70;
	do {
		if (amt223SendReceive(encoderSPI, timer1MHz, spi_Tx, spi_Rx))
		{
			position = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

			if (amt223Check(position))
			{
				position &= 0x3FFF;
			}
		}
		HAL_Delay(100);
	} while (position != 0);
}

static float _acceleratorCurve(float duty)
{
	return duty * duty;
}

static float currentDuty;

void amt223bPoll(SPI_HandleTypeDef* encoderSPI, TIM_HandleTypeDef* timer1MHz, PSR::VescCAN& vesc, UART_HandleTypeDef* debugUART)
{
	constexpr uint16_t MAX_POS = 900;

	char msg[65] = { '\0' };

	// SPI Setup
	uint8_t spi_Tx[2];
	uint8_t spi_Rx[2];

	spi_Tx[0] = 0x00;
	spi_Tx[1] = 0x00;

	int val    = (~(GPIOC->IDR >> 2)) & 0b11; // 2 bits active low forward/reverse
	int cruise = (~(GPIOC->IDR >> 1)) & 0b01; // 1 bit active low cruise control enable

	uint16_t position = 0;

	if (cruise && (val == 0b01 || val == 0b10))
	{
		// Cruise control set and not in neutral.
		HAL_UART_Transmit(debugUART, (uint8_t*)"Cruise Set: ", sizeof("Cruise Set: "), 100);
		vesc.SetDutyCycle(_acceleratorCurve(currentDuty));
	}
	else
	{
		HAL_UART_Transmit(debugUART, (uint8_t*)"Cruise Not Set: ", sizeof("Cruise Not Set: "), 100);

		if (amt223SendReceive(encoderSPI, timer1MHz, spi_Tx, spi_Rx))
		{
			position = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];
			if (amt223Check(position))
			{
				position &= 0x3FFF;

				// clamp position at 0
				if (position > (MAX_POS * 2))
				{
					position = 0;
				}

				currentDuty = (float)position / MAX_POS;
				// Clamp output
				if (currentDuty > 1)
				{
					currentDuty = 1;
				}
				// Dead zone
				if (currentDuty < 0.0125)
				{
					currentDuty = 0;
				}
			}
			// forward
			if (val == 0b01)
			{
				HAL_UART_Transmit(debugUART, (uint8_t*)"Forward,", sizeof("Forward,"), 100);
				vesc.SetDutyCycle(_acceleratorCurve(currentDuty));
			}

			// reverse
			else if (val == 0b10)
			{
				HAL_UART_Transmit(debugUART, (uint8_t*)"Reverse,", sizeof("Reverse,"), 100);
				vesc.SetDutyCycle(-_acceleratorCurve(currentDuty));
			}
			else
			{
				// neutral - does not transmit
				HAL_UART_Transmit(debugUART, (uint8_t*)"Neutral,", sizeof("Neutral,"), 100);
			}
			sprintf(msg, "Position:%d Duty: %d\r\n", position, (int)(_acceleratorCurve(currentDuty) * 100));
		}

		HAL_UART_Transmit(debugUART, (uint8_t*)msg, strlen(msg), 100);
	}
}
