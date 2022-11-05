################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../can-lib-main/can-lib-main/src/arduino_due.cpp \
../can-lib-main/can-lib-main/src/arduino_mega.cpp \
../can-lib-main/can-lib-main/src/arduino_uno.cpp \
../can-lib-main/can-lib-main/src/stm32.cpp \
../can-lib-main/can-lib-main/src/vesc.cpp 

OBJS += \
./can-lib-main/can-lib-main/src/arduino_due.o \
./can-lib-main/can-lib-main/src/arduino_mega.o \
./can-lib-main/can-lib-main/src/arduino_uno.o \
./can-lib-main/can-lib-main/src/stm32.o \
./can-lib-main/can-lib-main/src/vesc.o 

CPP_DEPS += \
./can-lib-main/can-lib-main/src/arduino_due.d \
./can-lib-main/can-lib-main/src/arduino_mega.d \
./can-lib-main/can-lib-main/src/arduino_uno.d \
./can-lib-main/can-lib-main/src/stm32.d \
./can-lib-main/can-lib-main/src/vesc.d 


# Each subdirectory must supply rules for building sources it contributes
can-lib-main/can-lib-main/src/%.o can-lib-main/can-lib-main/src/%.su: ../can-lib-main/can-lib-main/src/%.cpp can-lib-main/can-lib-main/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F072xB -DBOARD_STM32F=0 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/joemc/OneDrive/Documents/GitHub/AMT223b-SPI/can-lib-main/can-lib-main/inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-can-2d-lib-2d-main-2f-can-2d-lib-2d-main-2f-src

clean-can-2d-lib-2d-main-2f-can-2d-lib-2d-main-2f-src:
	-$(RM) ./can-lib-main/can-lib-main/src/arduino_due.d ./can-lib-main/can-lib-main/src/arduino_due.o ./can-lib-main/can-lib-main/src/arduino_due.su ./can-lib-main/can-lib-main/src/arduino_mega.d ./can-lib-main/can-lib-main/src/arduino_mega.o ./can-lib-main/can-lib-main/src/arduino_mega.su ./can-lib-main/can-lib-main/src/arduino_uno.d ./can-lib-main/can-lib-main/src/arduino_uno.o ./can-lib-main/can-lib-main/src/arduino_uno.su ./can-lib-main/can-lib-main/src/stm32.d ./can-lib-main/can-lib-main/src/stm32.o ./can-lib-main/can-lib-main/src/stm32.su ./can-lib-main/can-lib-main/src/vesc.d ./can-lib-main/can-lib-main/src/vesc.o ./can-lib-main/can-lib-main/src/vesc.su

.PHONY: clean-can-2d-lib-2d-main-2f-can-2d-lib-2d-main-2f-src

