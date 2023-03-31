#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)

#include "AMT223-b.h"
#include "can_lib.h"
#include "run.h"
#include "telemetry.h"
#include "vesc.h"

using namespace PSR;

extern "C"
{
	// String constants for printing.
	const std::string CRUISE_NOT_SET = "Cruise Not Set";
	const std::string CRUISE_SET     = "Cruise Set";
	const std::string FORWARD        = "Forward";
	const std::string NEUTRAL        = "Neutral";
	const std::string REVERSE        = "Reverse";

	void run(
		TIM_HandleTypeDef* htim,
		CAN_HandleTypeDef* hcan,
		SPI_HandleTypeDef* hspi,
		UART_HandleTypeDef* huart,
		UART_HandleTypeDef* uart3)
	{
		// Initialize timer
		HAL_TIM_Base_Start(htim);

		// Initializes the can communications
		CANBus::Config config = { .AutoRetransmit = true, .Mode = CANBus::ReceiveMode::Polling };

		CANBus can = PSR::CANBus(*hcan, config);
		can.Init();
		VescCAN vesc = VescCAN(can, 113);

		Telemetry telem = Telemetry(can, uart3);

		HAL_Delay(250);

		// SPI Setup
		uint8_t spi_Tx[2];
		uint8_t spi_Rx[2];

		uint16_t position = 1;

		spi_Tx[0] = 0x00;
		spi_Tx[1] = 0x70;
		while (position != 0)
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
		FP16_16 previousDuty = 0;
		while (true)
		{
			telem.SendCAN(); // Add this to the CAN interrupt

			// TODO: Joey add this to a timer interrupt.
			spi_Tx[0] = 0x00;
			spi_Tx[1] = 0x00;
			if (amt223SendReceive(hspi, htim, spi_Tx, spi_Rx))
			{
				position = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

				if (amt223Check(position))
				{
					char msg[129] = { '\0' };
					position &= 0x3FFF;

					// clamp position at 0
					if (position > (MAX_POS * 2))
					{
						position = 0;
					}

					FP16_16 duty = FP16_16(position) / FixedPoint<1>(MAX_POS);
					// Clamp output
					if (duty > 1)
					{
						duty = 1;
					}
					// Dead zone
					if (duty < .05)
					{
						duty = 0;
					}

					int fnr    = ~(GPIOC->IDR >> 2) & 3; // Active Low [1:0], so invert bits
					int cruise = ~(GPIOC->IDR >> 1) & 1; // Active Low, so invert bits

					FP16_16 currentDuty = 0;
					if (cruise == 0)
						previousDuty = duty;
					if (fnr == 0b01)
						currentDuty = previousDuty;
					if (fnr == 0b10)
						currentDuty = -0.5_fp * previousDuty;
					if (fnr != 0)
						vesc.SetDutyCycle(currentDuty);

					const std::string& cruiseStr = cruise == 0 ? CRUISE_NOT_SET : CRUISE_SET;
					const std::string& fnrStr = fnr == 0b01 ? FORWARD : fnr == 0b10 ? REVERSE : NEUTRAL;

					sprintf(msg, "%s, %s, Position: %d, Duty: %ld\r\n", cruiseStr.c_str(), fnrStr.c_str(), position, (int32_t)(100_fp * currentDuty));
					HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), 100);
				}
			}
		}
	}
}
