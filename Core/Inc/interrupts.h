/**
 * @file interrupts.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Handles interrupts for the VCU
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "can_lib.h"
#include "vesc.h"
#include "stm32_includer.h"
#include "telemetry.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)
#include STM32_INCLUDE(BOARD_STM32, hal_tim.h)
#include STM32_INCLUDE(BOARD_STM32, hal_spi.h)

/**
 * @brief Initializes the pedal polling interrupt
 * 
 * @param pollingTimer Pointer to timer used to poll the pedal position
 * @param irq IRQ type to enable
 * @param encoderSPI Pointer to SPI for reading encoder position
 * @param timer1MHz Pointer to a 1MHz clock signal
 * @param vesc Reference to VESC interface
 * @param debugUART Pointer to debugging UART
 */
void PedalInterruptSetup(TIM_HandleTypeDef* pollingTimer, IRQn_Type irq, SPI_HandleTypeDef* encoderSPI, TIM_HandleTypeDef* timer1MHz, PSR::VescCAN& vesc,  UART_HandleTypeDef* debugUART);

/**
 * @brief Initializes the telemetry handler interrupt
 * 
 * @param telemetryTimer Pointer to timer used to handle telemetry
 * @param irq IRQ type to enable
 * @param telem Reference to telemetry interface
 */
void TelemetryInterruptSetup(TIM_HandleTypeDef* telemetryTimer, IRQn_Type irq, PSR::Telemetry& telem);

extern "C"
{
	void TIM7_IRQHandler();
	void TIM6_DAC_IRQHandler();
}

#endif // end of include guard for interrupt.h
