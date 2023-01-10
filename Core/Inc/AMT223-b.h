/**
 * @file AMT223-b.h
 * @author Purdue Solar Racing (Joseph McAlear)
 * @brief
 * @version 0.1
 * @date 2023-01-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __AMT223_B_H
#define __AMT223_B_H

#include "run.h"

#if __cplusplus
extern "C"
{
#endif

bool amt223Check(uint16_t value);

HAL_StatusTypeDef sendByte(uint8_t * sendByte, bool pullLow, SPI_HandleTypeDef * hspi, TIM_HandleTypeDef * htim, uint8_t * receiveByte);

void resetEncoder(SPI_HandleTypeDef * hspi, TIM_HandleTypeDef * htim);

#if __cplusplus
#endif
}

#endif
