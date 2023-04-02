#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)

#include "AMT223-b.h"
#include "can_lib.h"
#include "run.h"
#include "telemetry.h"
#include "vesc.h"
#include "interrupts.h"

using namespace PSR;

extern "C"
{
	void run(
		TIM_HandleTypeDef* timer1MHz,
		CAN_HandleTypeDef* hcan,
		SPI_HandleTypeDef* encoderSPI,
		UART_HandleTypeDef* debugUART,
		UART_HandleTypeDef* telemUART,
		TIM_HandleTypeDef* pedalTimer,
		TIM_HandleTypeDef* telemTimer)
	{
		// Initialize timer
		HAL_TIM_Base_Start(timer1MHz);

		// Initialize CAN communications
		CANBus::Config config = { .AutoRetransmit = true, .Mode = CANBus::ReceiveMode::Polling };
		CANBus can = PSR::CANBus(*hcan, config);
		can.Init();

		VescCAN vesc = VescCAN(can, 113);

		// Initialize the telemetry system
		Telemetry telem = Telemetry(can, telemUART);

		amt223Setup(timer1MHz, encoderSPI);

		TelemetryInterruptSetup(telemTimer, TIM6_DAC_IRQn, telem);
		PedalInterruptSetup(pedalTimer, TIM7_IRQn, encoderSPI, timer1MHz, vesc, debugUART);

		// Infinite Loop
		while (true)
			asm volatile ("wfi");
	}
}
