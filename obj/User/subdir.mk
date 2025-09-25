################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v20x_it.c \
../User/main.c \
../User/system_ch32v20x.c 

C_DEPS += \
./User/ch32v20x_it.d \
./User/main.d \
./User/system_ch32v20x.d 

OBJS += \
./User/ch32v20x_it.o \
./User/main.o \
./User/system_ch32v20x.o 


EXPANDS += \
./User/ch32v20x_it.c.234r.expand \
./User/main.c.234r.expand \
./User/system_ch32v20x.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -DCH32V20x_D8W -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/UdpClient/User" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/SRC/Peripheral/inc" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/NetLib" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/ETH/UdpClient/Ld" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/SRC/Debug" -I"c:/Users/cf-19/Desktop/_208_/EXAM_208/SRC/Core" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

