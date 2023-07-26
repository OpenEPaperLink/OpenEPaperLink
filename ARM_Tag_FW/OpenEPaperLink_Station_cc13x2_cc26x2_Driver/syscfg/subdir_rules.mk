################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
syscfg/%.o: ../syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/gcc_arm_none_eabi_9_2_1/bin/arm-none-eabi-gcc-9.2.1.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DDeviceFamily_CC26X2 -I"C:/Users/pc/workspace_v12/rfPacketRx_LP_CC2652PSIP_tirtos7_gcc/syscfg" -I"C:/Users/pc/workspace_v12/rfPacketRx_LP_CC2652PSIP_tirtos7_gcc" -I"C:/Users/pc/workspace_v12/rfPacketRx_LP_CC2652PSIP_tirtos7_gcc/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_40_00_13/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_40_00_13/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_40_00_13/source/ti/posix/gcc" -I"C:/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include/newlib-nano" -I"C:/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/pc/workspace_v12/rfPacketRx_LP_CC2652PSIP_tirtos7_gcc/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


