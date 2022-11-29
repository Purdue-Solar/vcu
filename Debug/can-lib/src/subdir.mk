################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../can-lib/src/arduino_due.cpp \
../can-lib/src/arduino_mega.cpp \
../can-lib/src/arduino_uno.cpp \
../can-lib/src/stm32.cpp \
../can-lib/src/vesc.cpp 

OBJS += \
./can-lib/src/arduino_due.o \
./can-lib/src/arduino_mega.o \
./can-lib/src/arduino_uno.o \
./can-lib/src/stm32.o \
./can-lib/src/vesc.o 

CPP_DEPS += \
./can-lib/src/arduino_due.d \
./can-lib/src/arduino_mega.d \
./can-lib/src/arduino_uno.d \
./can-lib/src/stm32.d \
./can-lib/src/vesc.d 


# Each subdirectory must supply rules for building sources it contributes
can-lib/src/%.o can-lib/src/%.su: ../can-lib/src/%.cpp can-lib/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F072xB -DBOARD_STM32F=0 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/joemc/OneDrive/Documents/GitHub/AMT223b-SPI/can-lib/inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-can-2d-lib-2f-src

clean-can-2d-lib-2f-src:
	-$(RM) ./can-lib/src/arduino_due.d ./can-lib/src/arduino_due.o ./can-lib/src/arduino_due.su ./can-lib/src/arduino_mega.d ./can-lib/src/arduino_mega.o ./can-lib/src/arduino_mega.su ./can-lib/src/arduino_uno.d ./can-lib/src/arduino_uno.o ./can-lib/src/arduino_uno.su ./can-lib/src/stm32.d ./can-lib/src/stm32.o ./can-lib/src/stm32.su ./can-lib/src/vesc.d ./can-lib/src/vesc.o ./can-lib/src/vesc.su

.PHONY: clean-can-2d-lib-2f-src

