/*********************************************************************************************************************
 * @file        balance_ctrl.h
 * @brief       全状态反馈 LQR 矩阵控制
 ********************************************************************************************************************/
#ifndef _BALANCE_CTRL_H_
#define _BALANCE_CTRL_H_

#include "robot_config.h"

typedef struct {
  // ---- LQR 反馈增益矩阵 [2x6] ----
  float K[2][6];

  // ---- 状态向量 x [6x1] ----
  // x[0]: 位移误差
  // x[1]: 速度误差
  // x[2]: 俯仰角误差
  // x[3]: 俯仰角速度误差
  // x[4]: 偏航角误差
  // x[5]: 偏航角速度误差
  float x[6]; 

  // ---- 运行时目标 ----
  float target_speed;    
  float target_pitch;    
  float target_yaw;      

  // ---- 内部状态 ----
  float displacement_estimate; 
  float yaw_estimate;
  float yaw_last_error;

  // ---- 控制输出 ----
  float u[2]; // u[0]: Left Torque, u[1]: Right Torque
  int32 pwm_left;
  int32 pwm_right;

  uint8 enabled; 
} balance_ctrl_t;

extern balance_ctrl_t g_balance;

void balance_ctrl_init(void);
void balance_ctrl_update(void);
void balance_ctrl_enable(uint8 en);
void balance_ctrl_set_target(float speed_mps, float yaw_rads);

#endif // _BALANCE_CTRL_H_