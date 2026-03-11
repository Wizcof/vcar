/*********************************************************************************************************************
 * @file        balance_ctrl.h
 * @brief       LQR 平衡控制 + PD 转向控制
 ********************************************************************************************************************/
#ifndef _BALANCE_CTRL_H_
#define _BALANCE_CTRL_H_

#include "robot_config.h"

// ============================================================================
//  控制器状态与参数结构体
// ============================================================================
typedef struct {
  // ---- LQR 增益 ----
  float k1; // θ   (pitch angle)
  float k2; // dθ  (pitch angular velocity)
  float k3; // x   (displacement)
  float k4; // dx  (linear velocity)

  // ---- 转向 PD ----
  float turn_kp;
  float turn_kd;

  // ---- 运行时目标 ----
  float target_speed;    // 目标线速度 (m/s)，正值前进
  float target_yaw_rate; // 目标偏航角速度 (rad/s)，正值左转

  // ---- 内部状态 ----
  float x_estimate; // 位移估计 (由速度积分)

  // ---- 控制输出 ----
  int32 pwm_left;
  int32 pwm_right;

  // ---- 使能标志 ----
  uint8 enabled; // 0=停机 1=运行
} balance_ctrl_t;

extern balance_ctrl_t g_balance;

// ============================================================================
//  公开函数
// ============================================================================

/**
 * @brief   初始化平衡控制器，加载默认参数
 */
void balance_ctrl_init(void);

/**
 * @brief   在 PIT 中断中调用，执行 LQR 平衡 + 转向，并设置电机 PWM
 * @note    调用前需确保 imu_task_update() 和 motor_ctrl_update_encoder() 已执行
 */
void balance_ctrl_update(void);

/**
 * @brief   设置遥控目标速度与转向
 */
void balance_ctrl_set_target(float speed_mps, float yaw_rate_rads);

/**
 * @brief   使能/禁用控制器
 */
void balance_ctrl_enable(uint8 en);

/**
 * @brief   运行时修改 LQR 增益（用于串口调参）
 */
void balance_ctrl_set_lqr_gains(float k1, float k2, float k3, float k4);

/**
 * @brief   运行时修改转向 PD 增益
 */
void balance_ctrl_set_turn_gains(float kp, float kd);

#endif // _BALANCE_CTRL_H_
