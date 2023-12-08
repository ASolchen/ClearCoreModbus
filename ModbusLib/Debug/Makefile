################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../inc/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../inc/modbus_tcp_server.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
inc/modbus_tcp_server.o

OBJS_AS_ARGS +=  \
inc/modbus_tcp_server.o

C_DEPS +=  \
inc/modbus_tcp_server.d

C_DEPS_AS_ARGS +=  \
inc/modbus_tcp_server.d

OUTPUT_FILE_PATH +=libModbusLib.a

OUTPUT_FILE_PATH_AS_ARGS +=libModbusLib.a

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler
inc/modbus_tcp_server.o: ../inc/modbus_tcp_server.cpp
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 6.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-g++.exe$(QUOTE) -mthumb -D__SAME53N19A__ -DARM_MATH_CM4 -DDEBUG -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\CMSIS\4.5.0\CMSIS\Include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAME53_DFP\1.1.118\include" -I"../inc" -I"../../LwIP/LwIP/port/include" -I"../../LwIP/LwIP/src/include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\CMSIS\5.4.0\CMSIS\Core\Include" -I"../../libClearCore/inc"  -O1 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g3 -Wall -Wextra -pedantic -mcpu=cortex-m4 -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu++11 -fno-threadsafe-statics -nostdlib --param max-inline-insns-single=500 -MMD -v -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	




# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)


$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: ARM/GNU Archiver : 6.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-ar.exe$(QUOTE) -r  -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) 
	@echo Finished building target: $@
	
	




# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "libModbusLib.elf" "libModbusLib.a" "libModbusLib.hex" "libModbusLib.bin" "libModbusLib.lss" "libModbusLib.eep" "libModbusLib.map" "libModbusLib.srec"
	