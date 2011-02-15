################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BaseApplication.cpp \
../src/CubeGenerator.cpp \
../src/VisualizerApplication.cpp \
../src/main.cpp 

OBJS += \
./src/BaseApplication.o \
./src/CubeGenerator.o \
./src/VisualizerApplication.o \
./src/main.o 

CPP_DEPS += \
./src/BaseApplication.d \
./src/CubeGenerator.d \
./src/VisualizerApplication.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/OGRE -I/usr/include -I/usr/include/SDL -I/usr/local/include/CEGUI -I/usr/include/OIS -O3 -g3 -p -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


