#include "zf_common_headfile.h"
#include "robot_init.h"
#include "imu_task.h"
#include "debug_uart.h"
#include "motor_ctrl.h"
#include "balance_ctrl.h"

#pragma section all "cpu0_dsram"

int core0_main(void)
{
    clock_init();                   // 获取时钟频率
    debug_init();                   // 初始化默认调试串口

    Robot_System_Init();            // 初始化 IMU、电机并启动 PIT 控制中断

    // 注意：不要在这里强制 balance_ctrl_enable(1); 
    // 因为平放时会瞬间触发大于35度的保护被强制关停。

    cpu_wait_event_ready();         
    
    while (TRUE)
    {
        // 1. 串口指令处理 (可以通过串口助手发 'E' 强行启动)
        debug_uart_process_cmd();

        // 2. 自动使能逻辑 (极大提升调试体验)
        // 如果电机当前是关闭状态，且检测到你用手把车身扶正了（倾角在 ±10度 / 0.17 rad 内）
        if (g_balance.enabled == 0 && 
            g_imu.pitch_rad < 0.17f && g_imu.pitch_rad > -0.17f) 
        {
            balance_ctrl_enable(1); // 自动使能
        }

        // 3. 安全且不冲突的串口打印
        // 将打印放在主循环，完全杜绝乱码问题
       // cpu0_main.c 中的 printf 替换为：
        /*printf("Pitch:%.2f° | En:%d | PWM L:%d R:%d | u_LQR:P*%.1f\r\n",
               (g_imu.pitch_rad - BALANCE_ANGLE_OFFSET) * 57.2958f,
               g_balance.enabled,
               g_balance.pwm_left, g_balance.pwm_right,
               g_balance.K[0][2] * (g_imu.pitch_rad - BALANCE_ANGLE_OFFSET));
*/
        // 如果你需要用 VOFA+ 的波形，取消下面这行的注释即可：
         printf("ctrl:%f,%f,%f,%f,%d,%d\n", g_imu.pitch_rad, g_imu.pitch_gyro_rad_s, g_motor.speed_avg_mps, g_motor.distance_m, g_balance.pwm_left, g_balance.pwm_right);

        system_delay_ms(50);
    }
}
#pragma section all restore
