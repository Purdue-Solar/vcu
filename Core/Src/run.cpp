#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)

#include "AMT223-b.h"
#include "can_lib.h"
#include "run.h"
#include "vesc.h"

using namespace PSR;

extern "C"
{
	VescCAN* vesc;

	void run(TIM_HandleTypeDef* htim, CAN_HandleTypeDef* hcan, SPI_HandleTypeDef* hspi)
	{
		HAL_TIM_Base_Start(htim);

		// initializes the can communications
		CANBus::Config config = { .AutoRetransmit = true, .FilterMask = 0x7FF };

		CANBus can = PSR::CANBus(*hcan, config);
		can.Init();

		vesc = new VescCAN(can, 113);

		HAL_Delay(250);

		// SPI Setup
		uint8_t spi_Tx[2];
		uint8_t spi_Rx[2];

		HAL_StatusTypeDef checkStatus;

		int16_t initialPosition;
		int16_t finalPosition;
		int16_t relativePosition;
		// get position
		spi_Tx[0] = 0x00;
		spi_Tx[1] = 0x00;

		for (int i = 0; i < 100; i++)
		{
			checkStatus = sendByte(spi_Tx, true, hspi, htim, spi_Rx);
			if (checkStatus == HAL_OK)
			{
				checkStatus = sendByte(spi_Tx + 1, false, hspi, htim, spi_Rx + 1);
				if (checkStatus == HAL_OK)
				{
					// If both positions are valid and correct, then we can use the checkbit formula
					finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

					if (amt223Check(finalPosition))
					{
						finalPosition &= 0b0011111111111111;
						initialPosition = finalPosition;
					}
				}
			}
		}

		// Infinite Loop
		// Continually gets the position from the optical encoder
		while (true)
		{
			// TODO: investigate if breaking the byte sending corrupts subsequent data
			checkStatus = sendByte(spi_Tx, true, hspi, htim, spi_Rx);
			if (checkStatus == HAL_OK)
			{
				checkStatus = sendByte(spi_Tx + 1, false, hspi, htim, spi_Rx + 1);
				if (checkStatus == HAL_OK)
				{
					// If both positions are valid and correct, then we can use the checkbit formula
					int16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

					if (amt223Check(finalPosition))
					{
						finalPosition &= 0b0011111111111111;
						relativePosition = finalPosition - initialPosition;
						float duty       = (float)relativePosition / MAX_AMT223B_VALUE;
						vesc->SetDutyCycle(duty * 12);
					}
				}
			}
		}
	}
}
