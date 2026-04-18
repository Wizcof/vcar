/*********************************************************************************************************************
 * @file        robot_init.c
 * @brief       机器人系统初始化与控制主循环 —— 实现文件
 *
 * VOFA+ Printf 协议格式:
 *   "describe: data1, data2, ..., dataN\n"
 *
 * IMU 调试模式 (DEBUG_MODE_IMU_ONLY = 1) 通道说明:
 *   CH1 = Pitch 角度 (度)         —— 卡尔曼滤波输出的俯仰角
 *   CH2 = Pitch 角速度 (度/s)     —— 滤波后的俯仰角速度
 *   CH3 = Yaw 角速度 (度/s)       —— 绕 Z 轴角速度 (陀螺仪直出)
 *   CH4 = Acc X (g)               —— X 轴加速度
 *   CH5 = Acc Z (g)               —— Z 轴加速度
 *
 * 完整控制模式 (DEBUG_MODE_IMU_ONLY = 0) 通道说明:
 *   CH1 = Pitch 角度 (度)
 *   CH2 = Pitch 角速度 (度/s)
 *   CH3 = Yaw 角速度 (度/s)
 *   CH4 = Acc X (g)
 *   CH5 = Acc Z (g)
 *   CH6 = 平均线速度 (m/s)
 *   CH7 = 左电机 PWM
 *   CH8 = 右电机 PWM
 ********************************************************************************************************************/
#include "robot_init.h"
#include "imu_task.h"
#include "motor_ctrl.h"
#include "balance_ctrl.h"
#include "debug_uart.h"

// ============================================================================
//  控制环计数器 (用于降频发送调试数据)
// ============================================================================
static uint32 loop_counter = 0;

// ============================================================================
//  系统初始化
// ============================================================================
void Robot_System_Init(void)
{
    // ---- 调试串口 (始终初始化) ----
    debug_uart_init();
    printf("\r\n=============================\r\n");
    printf(" WheelLeg Robot v1.0\r\n");
    printf(" DEBUG_MODE_IMU_ONLY = %d\r\n", DEBUG_MODE_IMU_ONLY);
    printf("=============================\r\n");

    // ---- IMU 初始化 ----
    printf("[INIT] IMU ...\r\n");
    uint8 imu_ret = imu_task_init();
    if (imu_ret)
    {
        printf("[ERROR] IMU init FAILED! ret=%d\r\n", imu_ret);
    }
    else
    {
        printf("[INIT] IMU OK\r\n");
    }

#if (!DEBUG_MODE_IMU_ONLY)
    // ---- 电机与编码器 ----
    printf("[INIT] Motor & Encoder ...\r\n");
    motor_ctrl_init();
    printf("[INIT] Motor OK\r\n");

    // ---- 平衡控制器 ----
    printf("[INIT] Balance Controller ...\r\n");
    balance_ctrl_init();
    printf("[INIT] Balance OK (disabled, send 'E' to enable)\r\n");
#else
    printf("[INFO] IMU-only debug mode, motors DISABLED\r\n");
#endif

    // ---- 启动控制环定时器中断 (5ms) ----
    printf("[INIT] PIT %d us ...\r\n", CONTROL_PERIOD_US);
    pit_init(CONTROL_PIT_CH, CONTROL_PERIOD_US);
    printf("[INIT] System Ready!\r\n\r\n");

    loop_counter = 0;
}

// ============================================================================
//  控制主循环 (PIT 中断回调, 5ms 周期)
// ============================================================================
void Robot_Control_Loop(void)
{
    loop_counter++;

    // ================================================================
    //  1. IMU 数据采集与滤波 (始终执行)
    // ================================================================
    imu_task_update();

#if (!DEBUG_MODE_IMU_ONLY)
    // ================================================================
    //  2. 编码器数据更新
    // ================================================================
    motor_ctrl_update_encoder();

    // ================================================================
    //  3. 平衡控制
    // ================================================================
    balance_ctrl_update();
#endif

    // ================================================================
    //  4. VOFA+ Printf 协议调试数据发送
    //     每 4 次控制周期发一次 = 20ms = 50Hz
    //     格式: "describe: data1, data2, ..., dataN\n"
    // ================================================================
   /* if (loop_counter % 4 == 0)
    {
#if (!DEBUG_MODE_IMU_ONLY)
        // 完整控制模式: 8 通道
        printf("ctrl:%f,%f,%f,%f,%f,%f,%d,%d\n",
            g_imu.pitch_rad * 57.2958f,
            g_imu.pitch_gyro_rad_s * 57.2958f,
            g_imu.yaw_gyro_rad_s * 57.2958f,
            g_imu.acc_x_g,
            g_imu.acc_z_g,
            g_motor.speed_avg_mps,
            g_balance.pwm_left,
            g_balance.pwm_right);
#else
        // IMU 调试模式: 5 通道
        printf("imu:%f,%f,%f,%f,%f\n",
            g_imu.pitch_rad * 57.2958f,
            g_imu.pitch_gyro_rad_s * 57.2958f,
            g_imu.yaw_gyro_rad_s * 57.2958f,
            g_imu.acc_x_g,
            g_imu.acc_z_g);
#endif
    }
*/
}
