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
	void run(TIM_HandleTypeDef* htim, CAN_HandleTypeDef* hcan, SPI_HandleTypeDef* hspi, UART_HandleTypeDef * huart)
	{
		char MSG[65] = {'\0'};
		// Initialize timer
		HAL_TIM_Base_Start(htim);

		// Initializes the can communications
		CANBus::Config config = { .AutoRetransmit = true, .Mode = CANBus::ReceiveMode::Polling };

		CANBus can = PSR::CANBus(*hcan, config);
		can.Init();
		VescCAN vesc = VescCAN(can, 113);

		HAL_Delay(250);

		// SPI Setup
		uint8_t spi_Tx[2];
		uint8_t spi_Rx[2];

		int16_t initialPosition;
		int16_t relativePosition;

		uint16_t position = 1;

		spi_Tx[0] = 0x00;
		spi_Tx[1] = 0x70;

		while(position != 0)
		{
			if (amt223SendReceive(hspi, htim, spi_Tx, spi_Rx))
				{
					position = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

					if (amt223Check(position))
					{
						position &= 0x3FFF;
					}
				}
			HAL_Delay(500);

		}
		// get initial position
		/*
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
		*/
		// Infinite Loop
		// Continually gets the position from the optical encoder
		while (true)
		{
			/*
			if (amt223SendReceive(hspi, htim, spi_Tx, spi_Rx))
			{
				uint16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

				if (amt223Check(finalPosition))
				{
					finalPosition &= 0x3FFF;
					relativePosition = finalPosition - initialPosition;

					//Preventing back movement from optical encoder
					if(relativePosition < 0)
					{
						relativePosition = 0;
					}

					float duty       = (float)relativePosition / MAX_AMT223B_VALUE;

					//Clamp output
					if(duty > 1)
					{
						duty = (1/12);
					}

					//Dead zone
					if(duty < .005)
					{
						duty = 0;
					}
					x++;
					vesc.SetDutyCycle(duty * 12);
					sprintf(MSG, "Initial:%d Final:%d Relative:%d X:%d\r\n",initialPosition,finalPosition,relativePosition, x);
					HAL_UART_Transmit(huart,(uint8_t*) MSG, sizeof(MSG), 100);
					HAL_Delay(500);
				}
			}*/
			spi_Tx[0] = 0x00;
			spi_Tx[1] = 0x00;
			if (amt223SendReceive(hspi, htim, spi_Tx, spi_Rx))
			{
				position = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

				if (amt223Check(position))
				{
					position &= 0x3FFF;

					//clamp position at 0
					if(position > (MAX_POS * 2))
					{
						position = 0;
					}

					float duty       = (float)position / MAX_POS;
					//Clamp output
					if(duty > 1)
					{
						duty = 1;
					}
					//Dead zone
					if(duty < .05)
					{
						duty = 0;
					}
					vesc.SetDutyCycle(duty);

					sprintf(MSG, "Position:%d Duty: %d\r\n",position,(int)(duty*100));
					HAL_UART_Transmit(huart,(uint8_t*) MSG, sizeof(MSG), 100);
				}
			}
		}
	}
}
