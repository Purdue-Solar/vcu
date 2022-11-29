#define BOARD_STM32F 0
#include "run.h"
#include "can_lib.h"
#include "AMT223-b.h"
using namespace PSR;

extern "C"
{
VescCAN* vesc;

void startUp(TIM_HandleTypeDef * htim, CAN_HandleTypeDef * hcan,SPI_HandleTypeDef * hspi)
{
	//initializes the can communications
	CANBus::Config config = {.AutoRetransmit = true, .FilterMask = 0x7FF};
	CANBus can = PSR::CANBus(*hcan,config);
	can.Init();
	vesc = new VescCAN(can,113);
	HAL_Delay(1000);

	//Reset the optical encoder
	resetEncoder(hspi,htim);
}
void loop(TIM_HandleTypeDef * htim, CAN_HandleTypeDef * hcan,SPI_HandleTypeDef * hspi)
{
	//SPI Setup
	uint8_t spi_Tx[2];
	uint8_t spi_Rx[2];

	HAL_StatusTypeDef checkStatus;

	//get position
	spi_Tx[0] = 0x00;
	spi_Tx[1] = 0x70;

	checkStatus = sendByte(spi_Tx,true,hspi,htim,spi_Rx);
	if(checkStatus == HAL_OK)
	{
		checkStatus = sendByte(spi_Tx + 1,false,hspi,htim,spi_Rx + 1);
		if(checkStatus == HAL_OK)
		{
			//If both positions are valid and correct, then we can use the checkbit formula
			int16_t finalPosition = ((uint16_t)spi_Rx[0] << 8) | (uint16_t)spi_Rx[1];

			if(amt223Check(finalPosition))
			{
				finalPosition &= 0b0011111111111111;
				float duty = finalPosition/16000.0;
				vesc->SetDutyCycle(duty);
			}
		}
	}
}
}
