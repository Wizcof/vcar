################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
"../code/balance_ctrl.c" \
"../code/debug_uart.c" \
"../code/imu_task.c" \
"../code/motor_ctrl.c" \
"../code/robot_init.c" 

COMPILED_SRCS += \
"code/balance_ctrl.src" \
"code/debug_uart.src" \
"code/imu_task.src" \
"code/motor_ctrl.src" \
"code/robot_init.src" 

C_DEPS += \
"./code/balance_ctrl.d" \
"./code/debug_uart.d" \
"./code/imu_task.d" \
"./code/motor_ctrl.d" \
"./code/robot_init.d" 

OBJS += \
"code/balance_ctrl.o" \
"code/debug_uart.o" \
"code/imu_task.o" \
"code/motor_ctrl.o" \
"code/robot_init.o" 


# 每个子目录必须为构建它所贡献的源提供规则
"code/balance_ctrl.src":"../code/balance_ctrl.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 "-fC:/Users/Wiz/Desktop/ZF/TC387_Library/Seekfree_TC387_Opensource_Library/Seekfree_TC387_dual_camera_Opensource_Library/Seekfree_TC387_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<"
"code/balance_ctrl.o":"code/balance_ctrl.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/debug_uart.src":"../code/debug_uart.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 "-fC:/Users/Wiz/Desktop/ZF/TC387_Library/Seekfree_TC387_Opensource_Library/Seekfree_TC387_dual_camera_Opensource_Library/Seekfree_TC387_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<"
"code/debug_uart.o":"code/debug_uart.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/imu_task.src":"../code/imu_task.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 "-fC:/Users/Wiz/Desktop/ZF/TC387_Library/Seekfree_TC387_Opensource_Library/Seekfree_TC387_dual_camera_Opensource_Library/Seekfree_TC387_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<"
"code/imu_task.o":"code/imu_task.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/motor_ctrl.src":"../code/motor_ctrl.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 "-fC:/Users/Wiz/Desktop/ZF/TC387_Library/Seekfree_TC387_Opensource_Library/Seekfree_TC387_dual_camera_Opensource_Library/Seekfree_TC387_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<"
"code/motor_ctrl.o":"code/motor_ctrl.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/robot_init.src":"../code/robot_init.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 "-fC:/Users/Wiz/Desktop/ZF/TC387_Library/Seekfree_TC387_Opensource_Library/Seekfree_TC387_dual_camera_Opensource_Library/Seekfree_TC387_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc38x -Y0 -N0 -Z0 -o "$@" "$<"
"code/robot_init.o":"code/robot_init.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-code

clean-code:
	-$(RM) ./code/balance_ctrl.d ./code/balance_ctrl.o ./code/balance_ctrl.src ./code/debug_uart.d ./code/debug_uart.o ./code/debug_uart.src ./code/imu_task.d ./code/imu_task.o ./code/imu_task.src ./code/motor_ctrl.d ./code/motor_ctrl.o ./code/motor_ctrl.src ./code/robot_init.d ./code/robot_init.o ./code/robot_init.src

.PHONY: clean-code

