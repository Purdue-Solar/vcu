/*
 * run.h
 *
 *  Created on: Jan 9, 2023
 *      Author: aidan
 */

#ifndef INC_RUN_H_
#define INC_RUN_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
constexpr uint16_t MAX_AMT223B_VALUE = 16383;
#else
#define MAX_AMT223B_VALUE 16383
#endif

void run(TIM_HandleTypeDef * htim, CAN_HandleTypeDef * hcan,SPI_HandleTypeDef * hspi);

#ifdef __cplusplus
}
#endif

#endif /* INC_RUN_H_ */
