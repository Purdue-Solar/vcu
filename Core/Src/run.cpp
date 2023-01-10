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
		// Initialize timer
		HAL_TIM_Base_Start(htim);

		// Initializes the can communications
		CANBus::Config config = { .AutoRetransmit = true, .FilterMask = 0x7FF };

		CANBus can = PSR::CANBus(*hcan, config);
		can.Init();
		vesc = new VescCAN(can, 113);

		HAL_Delay(250);

		// SPI Setup
		uint8_t spi_Tx[2] = {0x00, 0x00};
		uint8_t spi_Rx[2];

		HAL_StatusTypeDef checkStatus;

		int16_t initialPosition;
		int16_t relativePosition;
		// get initial position
		for (int i = 0; i < 100; i++)
		{
			if (amt223SendReceive(hspi, htim, spi_Tx, spi_Rx))
			{
				// If both positions are valid and correct, then we can use the checkbit formula
				uint16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

				if (amt223Check(finalPosition))
				{
					initialPosition = finalPosition & 0x3FFF;
				}
			}
		}

		// Infinite Loop
		// Continually gets the position from the optical encoder
		while (true)
		{
			if (amt223SendReceive(hspi, htim, spi_Tx, spi_Rx))
			{
				uint16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

				if (amt223Check(finalPosition))
				{
					finalPosition &= 0x3FFF;
					relativePosition = finalPosition - initialPosition;

					float duty       = (float)relativePosition / MAX_AMT223B_VALUE;
					vesc->SetDutyCycle(duty * 12);
				}
			}
		}
	}
}
