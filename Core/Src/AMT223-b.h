/**
 * @file delay.h
 * @author Purdue Solar (Joseph McAlear)
 * @brief
 * @version 0.1
 * @date 2022-11-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __AMT223_B_H
#define __AMT223_B_H


#include "run.h"


#if __cplusplus
extern "C"
{
#endif

int bitExtract(int value, int bit);
bool amt223Check(uint16_t value);
void delayMicro (uint16_t us, TIM_HandleTypeDef * htim);
HAL_StatusTypeDef sendByte(uint8_t * sendByte, bool pullLow, SPI_HandleTypeDef * hspi, TIM_HandleTypeDef * htim, uint8_t * receiveByte);
void resetEncoder(SPI_HandleTypeDef * hspi, TIM_HandleTypeDef * htim);

#if __cplusplus
#endif
}

#endif
