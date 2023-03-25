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
#include "telemetry.h"

using namespace PSR;

extern "C"
{
	void run(
			TIM_HandleTypeDef* htim,
			CAN_HandleTypeDef* hcan,
			SPI_HandleTypeDef* hspi,
			UART_HandleTypeDef * huart,
			UART_HandleTypeDef* uart3)
	{
		char MSG[65] = {'\0'};
		// Initialize timer
		HAL_TIM_Base_Start(htim);

		// Initializes the can communications
		CANBus::Config config = { .AutoRetransmit = true, .Mode = CANBus::ReceiveMode::Polling };

		CANBus can = PSR::CANBus(*hcan, config);
		can.Init();
		VescCAN vesc = VescCAN(can, 113);

		Telemetry telem = Telemetry(can, uart3);
		while (true) telem.SendCAN();

		HAL_Delay(250);

		// SPI Setup
		uint8_t spi_Tx[2];
		uint8_t spi_Rx[2];

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
		// Infinite Loop
		// Continually gets the position from the optical encoder
		while (true)
		{
			telem.SendCAN();

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

					int val = (GPIOC->IDR >> 2) & 3; //high enable?
					int cruise = (GPIOC->IDR >> 1) & 1; //low enable?
					if(cruise)
					{
						HAL_UART_Transmit(huart,(uint8_t*) "Cruise Not Set: ", sizeof("Cruise Not Set: "), 100);
					}
					else
					{
						if(val == 1 || val == 2)
						{
							HAL_UART_Transmit(huart,(uint8_t*) "Cruise Set", sizeof("Cruise Set"), 100);
							continue;
						}
						else
						{
							HAL_UART_Transmit(huart,(uint8_t*) "Cruise Not Set: ", sizeof("Cruise Not Set: "), 100);
						}
					}
					//forward
					if(val == 1)
					{
						HAL_UART_Transmit(huart,(uint8_t*) "Forward,", sizeof("Forward,"), 100);
						vesc.SetDutyCycle(duty);
					}

					//reverse
					else if(val == 2)
					{
						HAL_UART_Transmit(huart,(uint8_t*) "Reverse,", sizeof("Reverse,"), 100);
						duty = -1 * duty * .5;
						vesc.SetDutyCycle(duty);
					}
						else
					{
					//neutral - does not transmit
					HAL_UART_Transmit(huart,(uint8_t*) "Neutral,", sizeof("Neutral,"), 100);
				}
				sprintf(MSG, "Position:%d Duty: %d\r\n",position,(int)(duty*100));
				HAL_UART_Transmit(huart,(uint8_t*) MSG, sizeof(MSG), 100);
				}
			}
		}
	}
}
