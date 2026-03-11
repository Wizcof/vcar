/*********************************************************************************************************************
 * @file        motor_ctrl.h
 * @brief       双路电机 PWM 驱动与编码器速度采集
 ********************************************************************************************************************/
#ifndef _MOTOR_CTRL_H_
#define _MOTOR_CTRL_H_

#include "robot_config.h"

// ============================================================================
//  电机数据结构体
// ============================================================================
typedef struct {
  // --- 有刷编码器原始数据 ---
  int16 enc_left_raw;  // 左编码器原始差分计数 (有符号)
  int16 enc_right_raw; // 右编码器原始差分计数

  // --- 无刷串口协议原始数据 ---
  uint8  receive_data_buffer[7]; // 接收缓冲数组
  uint8  receive_data_count;     // 接收计数
  uint8  sum_check_data;         // 校验位
  int16  recv_speed_left;        // 接收到的左侧电机速度数据
  int16  recv_speed_right;       // 接收到的右侧电机速度数据
  uint8  send_data_buffer[7];    // 发送缓冲区

  // --- 统合后状态量 ---
  float speed_left_mps;  // 左轮线速度 (m/s)
  float speed_right_mps; // 右轮线速度
  float speed_avg_mps;   // 质心平均线速度
  float distance_m;      // 质心累计位移 (m)，用于 LQR x 状态
} motor_data_t;

extern motor_data_t g_motor;

// ============================================================================
//  公开函数
// ============================================================================

/**
 * @brief   初始化双路电机驱动 (兼容无刷/有刷)
 */
void motor_ctrl_init(void);

/**
 * @brief   在定时器中断中调用，读取差分脉冲/串口测速值并计算统合速度
 */
void motor_ctrl_update_encoder(void);

/**
 * @brief   设置双路电机占空比
 * @param   pwm_left    左电机 PWM (-MOTOR_PWM_MAX ~ +MOTOR_PWM_MAX)
 * @param   pwm_right   右电机 PWM (-MOTOR_PWM_MAX ~ +MOTOR_PWM_MAX)
 */
void motor_ctrl_set_pwm(int32 pwm_left, int32 pwm_right);

/**
 * @brief   发送串口指令以获取速度 (若为无刷串口模式)
 */
void motor_driver_get_speed(void);

/**
 * @brief   接收串口数据的中断回调，拼合帧数据 (若为无刷串口模式)
 */
void motor_uart_callback(void);

/**
 * @brief   紧急停车，双电机置零
 */
void motor_ctrl_stop(void);

/**
 * @brief   重置累计位移
 */
void motor_ctrl_reset_distance(void);

#endif // _MOTOR_CTRL_H_
