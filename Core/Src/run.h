/*JosephMcAlear — Today at 10:44 AM
* @file delay.h
* @author Purdue Solar (Joseph McAlear)
* @brief
* @version 0.1
* @date 2022-11-12
*
* @copyright Copyright (c) 2022
*
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "vesc.h"
#include "AMT223-b.h"
#include "stm32f0xx_hal.h"

#ifndef __RUN_H
#define __RUN_H

#if __cplusplus
extern "C"
{
#endif


void startUp(TIM_HandleTypeDef * htim, CAN_HandleTypeDef * hcan,SPI_HandleTypeDef * hspi);

#if __cplusplus
#endif
}

#endif
