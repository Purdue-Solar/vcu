/*
 * run.h
 *
 *  Created on: Jan 9, 2023
 *      Author: aidan
 */

#ifndef INC_RUN_H_
#define INC_RUN_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)

#ifdef __cplusplus
extern "C"
{
#endif

	void run(
		TIM_HandleTypeDef* timer1MHz,
		CAN_HandleTypeDef* hcan,
		SPI_HandleTypeDef* encoderSPI,
		UART_HandleTypeDef* debugUART,
		UART_HandleTypeDef* telemUART,
		TIM_HandleTypeDef* pedalTimer,
		TIM_HandleTypeDef* telemTimer,
		TIM_HandleTypeDef* indicatorTimer);

#ifdef __cplusplus
}
#endif

#endif /* INC_RUN_H_ */
