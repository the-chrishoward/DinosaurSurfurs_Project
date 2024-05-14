################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
HAL/%.obj: ../HAL/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/CSSstudio/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/icebe/Documents/VT Spring 2024/ECE 2564 - Embedded Systems/s24-proj2-the-chrishoward" --include_path="C:/Users/icebe/Documents/VT Spring 2024/ECE 2564 - Embedded Systems/s24-proj2-the-chrishoward" --include_path="X:/CSSstudio_workspace/simplelink_msp432p4_sdk_3_40_01_02/source" --include_path="X:/CSSstudio_workspace/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/CMSIS/Include" --include_path="D:/CSSstudio/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include" --advice:power=none --define=__MSP432P401R__ --define=DeviceFamily_MSP432P401x -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="HAL/$(basename $(<F)).d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


