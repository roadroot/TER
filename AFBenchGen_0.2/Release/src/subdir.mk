################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AF.cpp \
../src/AFBenchGen.cpp \
../src/Argument.cpp \
../src/SCC.cpp \
../src/SetArguments.cpp \
../src/misc.cpp 

OBJS += \
./src/AF.o \
./src/AFBenchGen.o \
./src/Argument.o \
./src/SCC.o \
./src/SetArguments.o \
./src/misc.o 

CPP_DEPS += \
./src/AF.d \
./src/AFBenchGen.d \
./src/Argument.d \
./src/SCC.d \
./src/SetArguments.d \
./src/misc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DNDEBUG -DNLOGPRECO -DNSTATSPRECO -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -I../src/ -I../src/glucose -I../src/precosat -O3 -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


