#include "telemetry.h"
#include "can_lib.h"
#include "stm32_includer.h"
#include STM32_INCLUDE(BOARD_STM32, hal_uart.h)
#include <cstring>
#include <cstdint>

namespace PSR
{

void Telemetry::SendCAN()
{
	CANBus::Frame frame; 	
	while (this->_can.Receive(frame))
	{
		union
		{
			struct
			{
				uint32_t Id;        // CAN Extended or Standard Id
				uint8_t IsExtended; // 1 or 0, whether the frame is extended
				uint8_t IsRTR;      // 1 or 0, whether the frame is a remote transmission
				uint8_t Length;     // Number of bytes in the data
				uint64_t Data; 	// Payload of the CAN frame
			};
			uint8_t Bytes[16];
		} buffer;
		memset(buffer.Bytes, 0, sizeof(buffer));

		buffer.Id         = frame.Id;
		buffer.IsExtended = frame.IsExtended ? 1 : 0;
		buffer.IsRTR      = frame.IsRTR ? 1 : 0;
		buffer.Length     = frame.Length;
		buffer.Data = frame.Data.Value;

		HAL_UART_Transmit(this->_uart, buffer.Bytes, sizeof(buffer), 100);
		this->_syncCount++;

		if (this->_syncCount == SYNC_RATE)
		{
			// Send a signal to sync frame bytes
			// Sends "SYNC" followed by 12 bytes of 0xFF to mark the byte sync area
			uint8_t data[16];
			memset(data, 0xFF, sizeof(data));
			memcpy(data, "SYNC", 4);
			HAL_UART_Transmit(this->_uart, data, sizeof(data), 100);

			this->_syncCount = 0;
		}
	}
}

} // namespace PSR
