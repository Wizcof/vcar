/*********************************************************************************************************************
 * @file        balance_ctrl.c
 * @brief       LQR 平衡控制 + PD 转向控制 —— 实现文件
 ********************************************************************************************************************/
#include "balance_ctrl.h"
#include "imu_task.h"
#include "motor_ctrl.h"

// ============================================================================
//  全局控制器实例
// ============================================================================
balance_ctrl_t g_balance;

// ============================================================================
//  内部辅助：浮点限幅
// ============================================================================
static float clamp_f(float val, float min_val, float max_val) {
  if (val > max_val)
    return max_val;
  if (val < min_val)
    return min_val;
  return val;
}

// ============================================================================
//  初始化
// ============================================================================
void balance_ctrl_init(void) {
  // ---- LQR 默认增益 ----
  g_balance.k1 = LQR_K1_DEFAULT;
  g_balance.k2 = LQR_K2_DEFAULT;
  g_balance.k3 = LQR_K3_DEFAULT;
  g_balance.k4 = LQR_K4_DEFAULT;

  // ---- 转向 PD 默认参数 ----
  g_balance.turn_kp = TURN_KP_DEFAULT;
  g_balance.turn_kd = TURN_KD_DEFAULT;

  // ---- 目标初始化 ----
  g_balance.target_speed = 0.0f;
  g_balance.target_yaw_rate = 0.0f;

  // ---- 内部状态 ----
  g_balance.x_estimate = 0.0f;
  g_balance.pwm_left = 0;
  g_balance.pwm_right = 0;
  g_balance.enabled = 0; // 默认禁用，待传感器稳定后使能
}

// ============================================================================
//  控制器更新 (在 PIT 中断中调用)
//
//  控制流程:
//    1. 倾角安全检测 (倾角过大则停机保护)
//    2. LQR 平衡控制: u_balance = -(k1*θ + k2*dθ + k3*x + k4*dx)
//    3. PD 转向控制:  u_turn = kp*(ω_ref - ω) + kd*(0 - dω/dt) [简化为 P 控制]
//    4. 叠加输出并设置电机
// ============================================================================
void balance_ctrl_update(void) {
  if (!g_balance.enabled) {
    motor_ctrl_stop();
    g_balance.pwm_left = 0;
    g_balance.pwm_right = 0;
    return;
  }

  // ================================================================
  //  1. 安全检测：倾角超过 ±35° (0.61 rad) 停机
  // ================================================================
  float pitch = g_imu.pitch_rad - BALANCE_ANGLE_OFFSET;
  if (pitch > 0.61f || pitch < -0.61f) {
    motor_ctrl_stop();
    g_balance.enabled = 0;
    g_balance.pwm_left = 0;
    g_balance.pwm_right = 0;
    return;
  }

  // ================================================================
  //  2. LQR 平衡控制
  // ================================================================
  // 状态量
  float theta = pitch;                   // 倾角 (rad)
  float dtheta = g_imu.pitch_gyro_rad_s; // 角速度 (rad/s)
  float dx = g_motor.speed_avg_mps;      // 线速度 (m/s)

  // 位移状态: 用速度误差积分作为位移项
  // 当目标速度为 0 时，x 累积; 当有目标速度时，x 跟踪目标位移
  float speed_err = dx - g_balance.target_speed;
  g_balance.x_estimate += speed_err * CONTROL_PERIOD_S;

  // 位移积分限幅，防止长时间漂移导致积分饱和
  g_balance.x_estimate = clamp_f(g_balance.x_estimate, -0.5f, 0.5f);

  float x = g_balance.x_estimate;

  // LQR 反馈控制律: u = -(k1*θ + k2*dθ + k3*x + k4*dx_err)
  float u_balance = -(g_balance.k1 * theta + g_balance.k2 * dtheta +
                      g_balance.k3 * x + g_balance.k4 * speed_err);

  // ================================================================
  //  3. PD 转向控制
  // ================================================================
  float yaw_err = g_balance.target_yaw_rate - g_imu.yaw_gyro_rad_s;
  float u_turn = g_balance.turn_kp * yaw_err;
  // 注: kd 项可在后续加入角加速度反馈

  // ================================================================
  //  4. 叠加与放大输出
  // ================================================================
  // 此时算出的 u_balance 一般在 100~200，但无刷电机 PWM 幅度可达 10000 且在 1000 以内可能不转
  // 所以需要通过 LQR_OUTPUT_SCALE 统一放大以便实际驱动
  float pwm_l_f = (u_balance + u_turn) * LQR_OUTPUT_SCALE;
  float pwm_r_f = (u_balance - u_turn) * LQR_OUTPUT_SCALE;

  // 转换为整数并限幅 (注意右侧电机反向)
  g_balance.pwm_left = (int32)pwm_l_f;
  g_balance.pwm_right = -(int32)pwm_r_f;

  // ---- 设置电机 ----
  motor_ctrl_set_pwm(g_balance.pwm_left, g_balance.pwm_right);
}

// ============================================================================
//  设置遥控目标
// ============================================================================
void balance_ctrl_set_target(float speed_mps, float yaw_rate_rads) {
  g_balance.target_speed = speed_mps;
  g_balance.target_yaw_rate = yaw_rate_rads;
}

// ============================================================================
//  使能控制
// ============================================================================
void balance_ctrl_enable(uint8 en) {
  if (en && !g_balance.enabled) {
    // 使能时重置位移积分
    g_balance.x_estimate = 0.0f;
    motor_ctrl_reset_distance();
  }
  g_balance.enabled = en;
}

// ============================================================================
//  运行时修改 LQR 增益
// ============================================================================
void balance_ctrl_set_lqr_gains(float k1, float k2, float k3, float k4) {
  g_balance.k1 = k1;
  g_balance.k2 = k2;
  g_balance.k3 = k3;
  g_balance.k4 = k4;
}

// ============================================================================
//  运行时修改转向 PD 增益
// ============================================================================
void balance_ctrl_set_turn_gains(float kp, float kd) {
  g_balance.turn_kp = kp;
  g_balance.turn_kd = kd;
}
