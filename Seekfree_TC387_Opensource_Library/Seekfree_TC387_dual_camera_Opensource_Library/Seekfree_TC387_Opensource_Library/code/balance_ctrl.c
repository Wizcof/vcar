/*********************************************************************************************************************
 * @file        balance_ctrl.c
 * @brief       全状态反馈 LQR 矩阵控制 —— 实现文件
 ********************************************************************************************************************/
#include "balance_ctrl.h"
#include "imu_task.h"
#include "motor_ctrl.h"

balance_ctrl_t g_balance;

static float clamp_f(float val, float min_val, float max_val) {
  if (val > max_val) return max_val;
  if (val < min_val) return min_val;
  return val;
}

// 载入示例工程中的 K 矩阵
// 载入 MATLAB/Python 仿真计算得出的 LQR K 矩阵
static void load_k_matrix(void) {
    // ====================================================================
    // 左轮推力控制量 (对应 u[0])
    // ====================================================================
    g_balance.K[0][0] = -0.022361f;   // x[0]: 位移增益
    g_balance.K[0][1] = -2.766893f;   // x[1]: 速度增益
    g_balance.K[0][2] = -13.737548f;  // x[2]: 俯仰角增益 (抵抗倾倒核心)
    g_balance.K[0][3] = -0.523119f;   // x[3]: 俯仰角速度增益 (阻尼项)
    g_balance.K[0][4] =  0.223607f;   // x[4]: 偏航角增益
    g_balance.K[0][5] =  0.032340f;   // x[5]: 偏航角速度增益

    // ====================================================================
    // 右轮推力控制量 (对应 u[1])
    // ====================================================================
    g_balance.K[1][0] = -0.022361f;   // x[0]: 位移增益
    g_balance.K[1][1] = -2.766893f;   // x[1]: 速度增益
    g_balance.K[1][2] = -13.737548f;  // x[2]: 俯仰角增益 (抵抗倾倒核心)
    g_balance.K[1][3] = -0.523119f;   // x[3]: 俯仰角速度增益 (阻尼项)
    
    // 注意：偏航控制对于左右轮是对称相反的，这样才能产生差速转向力矩
    g_balance.K[1][4] = -0.223607f;   // x[4]: 偏航角增益 
    g_balance.K[1][5] = -0.032340f;   // x[5]: 偏航角速度增益
}

void balance_ctrl_init(void) {
  load_k_matrix();

  g_balance.target_speed = 0.0f;
  g_balance.target_pitch = 0.0f;
  g_balance.target_yaw = 0.0f;

  g_balance.displacement_estimate = 0.0f;
  g_balance.yaw_estimate = 0.0f;
  g_balance.yaw_last_error = 0.0f;
  g_balance.enabled = 1; 
}

void balance_ctrl_update(void) {
  if (!g_balance.enabled) {
    motor_ctrl_stop();
    return;
  }

  // 1. 安全检测
  float pitch = g_imu.pitch_rad;
  if (pitch > 0.61f || pitch < -0.61f) {
    motor_ctrl_stop();
    g_balance.enabled = 0;
    return;
  }

  // 2. 状态变量更新 (对应参考代码 LQR_Variable)
  float speed_err = g_motor.speed_avg_mps - g_balance.target_speed;
  speed_err = clamp_f(speed_err, -0.2f, 0.2f); // 限幅保护

  g_balance.displacement_estimate += speed_err * CONTROL_PERIOD_S;
  g_balance.yaw_estimate += g_imu.yaw_gyro_rad_s * CONTROL_PERIOD_S;

  // 组装状态向量 x
  g_balance.x[0] = g_balance.displacement_estimate;         // Displacement_Error
  g_balance.x[1] = speed_err;                               // Speed_Error
  g_balance.x[2] = pitch - g_balance.target_pitch;          // Pitch_Error
  g_balance.x[3] = g_imu.pitch_gyro_rad_s;                  // Pitch_Rate_Error
  
  float current_yaw_err = g_balance.yaw_estimate - g_balance.target_yaw;
  g_balance.x[4] = current_yaw_err;                         // Yaw_Error
  g_balance.x[5] = (current_yaw_err - g_balance.yaw_last_error) / CONTROL_PERIOD_S; // Yaw_Rate_Error
  
  g_balance.yaw_last_error = current_yaw_err;

  // 3. 矩阵相乘计算控制律: u = -K * x
  for (int i = 0; i < 2; i++) {
      g_balance.u[i] = 0;
      for (int j = 0; j < 6; j++) {
          g_balance.u[i] += g_balance.K[i][j] * g_balance.x[j];
      }
      g_balance.u[i] = -g_balance.u[i];
  }

  // 4. 输出到电机
  // u[0] 对应左轮扭矩，u[1] 对应右轮扭矩
  float pwm_l_f = g_balance.u[0] * LQR_OUTPUT_SCALE;
  float pwm_r_f = g_balance.u[1] * LQR_OUTPUT_SCALE;

  g_balance.pwm_left = -(int32)pwm_l_f;
  g_balance.pwm_right = -(int32)pwm_r_f;

  motor_ctrl_set_pwm(g_balance.pwm_left, g_balance.pwm_right);
}

void balance_ctrl_enable(uint8 en) {
  if (en && !g_balance.enabled) {
    g_balance.displacement_estimate = 0.0f;
    g_balance.yaw_estimate = 0.0f;
    motor_ctrl_reset_distance();
  }
  g_balance.enabled = en;
}

void balance_ctrl_set_target(float speed_mps, float yaw_rads) {
  g_balance.target_speed = speed_mps;
  g_balance.target_yaw = yaw_rads;
}