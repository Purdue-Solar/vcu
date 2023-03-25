/**
 * @file telemetry.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Handles communication with the telemetry system
 * @version 0.1
 * @date 2023-03-25
 *
 */

#ifndef __TELEMTRY_H
#define __TELEMTRY_H

#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)
#include STM32_INCLUDE(BOARD_STM32, hal_uart.h)
#include "can_lib.h"

namespace PSR
{

class Telemetry
{
	CANBus& _can;
	UART_HandleTypeDef* _uart;
    int _syncCount;

  public:
	Telemetry(CANBus& can, UART_HandleTypeDef* uart)
		: _can(can), _uart(uart), _syncCount(0)
	{}

    static constexpr int SYNC_RATE = 31;
	void SendCAN();
};

} // namespace PSR

#endif // end of include guard for telemtry.h