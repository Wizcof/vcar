################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
"../libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.c" \
"../libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.c" 

COMPILED_SRCS += \
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.src" \
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.src" 

C_DEPS += \
"./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.d" \
"./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.d" 

OBJS += \
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.o" \
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.o" 


# 每个子目录必须为构建它所贡献的源提供规则
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.src":"../libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.c" "libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc38x "-fC:/Users/Wiz/Desktop/ZF/TC387_Library/Example/Example_dual_camera/Coreboard_Demo/E02_uart_demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<"
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.o":"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.src" "libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.src":"../libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.c" "libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc38x "-fC:/Users/Wiz/Desktop/ZF/TC387_Library/Example/Example_dual_camera/Coreboard_Demo/E02_uart_demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<"
"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.o":"libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.src" "libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-libraries-2f-infineon_libraries-2f-Service-2f-CpuGeneric-2f-SysSe-2f-Comm

clean-libraries-2f-infineon_libraries-2f-Service-2f-CpuGeneric-2f-SysSe-2f-Comm:
	-$(RM) ./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.d ./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.o ./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Console.src ./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.d ./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.o ./libraries/infineon_libraries/Service/CpuGeneric/SysSe/Comm/Ifx_Shell.src

.PHONY: clean-libraries-2f-infineon_libraries-2f-Service-2f-CpuGeneric-2f-SysSe-2f-Comm

