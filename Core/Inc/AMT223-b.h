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

#include "can_lib.h"
#include "vesc.h"
#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)
#include STM32_INCLUDE(BOARD_STM32, hal_tim.h)

/**
 * @brief Checks if the received position is valid
 *
 * @param value data to be checked
 * @return Status indicating if the data passed the formula test
 */
bool amt223Check(uint16_t value);

/**
 * @brief Send a byte of data to the AMT223b optical encoder and get the data back
 *
 * @param hspi pointer to a SPI handle
 * @param htim pointer to a timer handle
 * @param sendByte byte to be sent
 * @param pullLow indicating if the CS line should be pulled low
 * @param receiveByte data byte recieved from the optical encoder
 *
 * @return HAL_Status of the byte that was sent
 */
HAL_StatusTypeDef amt223SendByte(SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim, uint8_t* sendByte, uint8_t* receiveByte, bool pullLow);

/**
 * @brief Send and receive 2 bytes of data from the AMT223b optical encoder
 * @param hspi pointer to a SPI handle
 * @param htim pointer to a timer handle
 * @param sendBytes bytes to be sent
 * @param receiveBytes data byte recieved from the optical encoder
 *
 * @return Whether the values were correctly sent or received
 */
bool amt223SendReceive(SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim, uint8_t* sendBytes, uint8_t* receiveBytes);

/**
 * @brief Reset the AMT223b optical encoder
 *
 * @param hspi pointer to the SPI setup
 * @param htim pointer to the timer
 */
void amt223Reset(SPI_HandleTypeDef* hspi, TIM_HandleTypeDef* htim);

/**
 * @brief Initialize the AMT 223-b
 * 
 * @param timer1MHz timer set to a 1MHz clock
 * @param encoderSPI SPI connected to the optical encoder
 */
void amt223Setup(TIM_HandleTypeDef* timer1MHz, SPI_HandleTypeDef* encoderSPI);

/**
 * @brief Poll the pedal and send the duty cycle to the VESC
 * 
 * @param timer1MHz pointer to a timer set to 1MHz
 * @param vesc reference to VESC interface
 * @param encoderSPI pointer to encoder SPI
 * @param debugUART pointer debug UART
 */
void amt223bPoll(SPI_HandleTypeDef* encoderSPI, TIM_HandleTypeDef* timer1MHz, PSR::VescCAN& vesc, UART_HandleTypeDef* debugUART);

#endif
