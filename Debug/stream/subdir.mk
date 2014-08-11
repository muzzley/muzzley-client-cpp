################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../stream/SocketStreams.cpp 

OBJS += \
./stream/SocketStreams.o 

CPP_DEPS += \
./stream/SocketStreams.d 


# Each subdirectory must supply rules for building sources it contributes
stream/%.o: ../stream/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -DMUZZLEY_DEBUG -I../ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


