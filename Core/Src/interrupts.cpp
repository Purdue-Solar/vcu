/**
 * @file interrupts.cpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Implementation file for interrupt handling
 * @version 0.1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "interrupts.h"
#include "AMT223-b.h"
#include "stm32_includer.h"
#include "telemetry.h"
#include STM32_INCLUDE(BOARD_STM32, hal.h)
#include STM32_INCLUDE(BOARD_STM32, hal_def.h)
#include STM32_INCLUDE(BOARD_STM32, hal_tim.h)

static TIM_HandleTypeDef* _pollingTimer;
static SPI_HandleTypeDef* _encoderSpi;
static TIM_HandleTypeDef* _timer1MHz;
static PSR::VescCAN* _vesc;
static UART_HandleTypeDef* _debugUART;

void PedalInterruptSetup(TIM_HandleTypeDef* pollingTimer, IRQn_Type irq, SPI_HandleTypeDef* encoderSPI, TIM_HandleTypeDef* timer1MHz, PSR::VescCAN& vesc, UART_HandleTypeDef* debugUART)
{
	pollingTimer->Instance->DIER |= TIM_DIER_UIE;
	HAL_NVIC_EnableIRQ(irq);

	_pollingTimer = pollingTimer;
	_encoderSpi   = encoderSPI;
	_timer1MHz    = timer1MHz;
	_vesc         = &vesc;
	_debugUART    = debugUART;

	HAL_TIM_Base_Start(timer1MHz);
	HAL_TIM_Base_Start(pollingTimer);
}

static TIM_HandleTypeDef* _telemetryTimer;
static PSR::Telemetry* _telem;


void TelemetryInterruptSetup(TIM_HandleTypeDef* telemetryTimer, IRQn_Type irq, PSR::Telemetry& telem)
{
	telemetryTimer->Instance->DIER |= TIM_DIER_UIE;
	HAL_NVIC_EnableIRQ(irq);

	_telemetryTimer = telemetryTimer;
	_telem          = &telem;

	HAL_TIM_Base_Start(telemetryTimer);
}

extern "C"
{
	void TIM7_IRQHandler()
	{
		amt223bPoll(_encoderSpi, _timer1MHz, *_vesc, _debugUART);
	}

	void TIM6_DAC_IRQHandler()
	{
		_telem->SendCAN();
	}
}
