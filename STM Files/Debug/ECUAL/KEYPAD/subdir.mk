################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ECUAL/KEYPAD/KEYPAD.c \
../ECUAL/KEYPAD/KEYPAD_cfg.c 

OBJS += \
./ECUAL/KEYPAD/KEYPAD.o \
./ECUAL/KEYPAD/KEYPAD_cfg.o 

C_DEPS += \
./ECUAL/KEYPAD/KEYPAD.d \
./ECUAL/KEYPAD/KEYPAD_cfg.d 


# Each subdirectory must supply rules for building sources it contributes
ECUAL/KEYPAD/%.o ECUAL/KEYPAD/%.su ECUAL/KEYPAD/%.cyclo: ../ECUAL/KEYPAD/%.c ECUAL/KEYPAD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ECUAL-2f-KEYPAD

clean-ECUAL-2f-KEYPAD:
	-$(RM) ./ECUAL/KEYPAD/KEYPAD.cyclo ./ECUAL/KEYPAD/KEYPAD.d ./ECUAL/KEYPAD/KEYPAD.o ./ECUAL/KEYPAD/KEYPAD.su ./ECUAL/KEYPAD/KEYPAD_cfg.cyclo ./ECUAL/KEYPAD/KEYPAD_cfg.d ./ECUAL/KEYPAD/KEYPAD_cfg.o ./ECUAL/KEYPAD/KEYPAD_cfg.su

.PHONY: clean-ECUAL-2f-KEYPAD

