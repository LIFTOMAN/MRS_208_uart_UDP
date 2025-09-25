################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/NetLib/eth_driver.c 

C_DEPS += \
./NetLib/eth_driver.d 

OBJS += \
./NetLib/eth_driver.o 


EXPANDS += \
./NetLib/eth_driver.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
NetLib/eth_driver.o: c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/NetLib/eth_driver.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -DCH32V20x_D8W -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/UdpClient/User" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/SRC/Peripheral/inc" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/NetLib" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/UdpClient/Ld" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/SRC/Debug" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/SRC/Core" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

