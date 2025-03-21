################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Util/util/DWT_Delay.c \
../Util/util/Timer_Delay.c 

OBJS += \
./Util/util/DWT_Delay.o \
./Util/util/Timer_Delay.o 

C_DEPS += \
./Util/util/DWT_Delay.d \
./Util/util/Timer_Delay.d 


# Each subdirectory must supply rules for building sources it contributes
Util/util/%.o Util/util/%.su Util/util/%.cyclo: ../Util/util/%.c Util/util/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Util-2f-util

clean-Util-2f-util:
	-$(RM) ./Util/util/DWT_Delay.cyclo ./Util/util/DWT_Delay.d ./Util/util/DWT_Delay.o ./Util/util/DWT_Delay.su ./Util/util/Timer_Delay.cyclo ./Util/util/Timer_Delay.d ./Util/util/Timer_Delay.o ./Util/util/Timer_Delay.su

.PHONY: clean-Util-2f-util

