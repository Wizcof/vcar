/*********************************************************************************************************************
 * @file        imu_task.c
 * @brief       IMU 数据采集与卡尔曼滤波姿态解算 —— 实现文件
 ********************************************************************************************************************/
#include "imu_task.h"
#include <math.h>

// ============================================================================
//  全局 IMU 数据实例
// ============================================================================
imu_data_t g_imu;

// ============================================================================
//  内部函数：卡尔曼滤波器初始化
// ============================================================================
static void kalman_init(kalman_filter_t *kf) {
  kf->angle = 0.0f;
  kf->gyro_bias = 0.0f;
  kf->P[0][0] = 1.0f;
  kf->P[0][1] = 0.0f;
  kf->P[1][0] = 0.0f;
  kf->P[1][1] = 1.0f;
  kf->Q_angle = KF_Q_ANGLE;
  kf->Q_gyro_bias = KF_Q_GYRO_BIAS;
  kf->R_measure = KF_R_MEASURE;
}

// ============================================================================
//  内部函数：卡尔曼滤波器更新
//  输入: new_angle  — 加速度计计算的角度 (rad)
//        new_rate   — 陀螺仪角速度 (rad/s)
//        dt         — 采样周期 (s)
//  输出: 滤波后的角度 (rad)
// ============================================================================
static float kalman_update(kalman_filter_t *kf, float new_angle, float new_rate,
                           float dt) {
  // ---- 预测 (Predict) ----
  // 1) 状态预测: angle += (rate - bias) * dt
  float rate = new_rate - kf->gyro_bias;
  kf->angle += dt * rate;

  // 2) 误差协方差预测
  kf->P[0][0] +=
      dt * (dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
  kf->P[0][1] -= dt * kf->P[1][1];
  kf->P[1][0] -= dt * kf->P[1][1];
  kf->P[1][1] += kf->Q_gyro_bias * dt;

  // ---- 更新 (Update) ----
  // 3) 计算卡尔曼增益
  float S = kf->P[0][0] + kf->R_measure; // 新息协方差
  float K0 = kf->P[0][0] / S;            // 增益 K[0]
  float K1 = kf->P[1][0] / S;            // 增益 K[1]

  // 4) 新息 (Innovation)
  float y = new_angle - kf->angle;

  // 5) 更新状态
  kf->angle += K0 * y;
  kf->gyro_bias += K1 * y;

  // 6) 更新误差协方差
  float P00_temp = kf->P[0][0];
  float P01_temp = kf->P[0][1];
  kf->P[0][0] -= K0 * P00_temp;
  kf->P[0][1] -= K0 * P01_temp;
  kf->P[1][0] -= K1 * P00_temp;
  kf->P[1][1] -= K1 * P01_temp;

  return kf->angle;
}

// ============================================================================
//  公开函数：IMU 初始化
// ============================================================================
uint8 imu_task_init(void) {
  uint8 ret = 0;

#if IMU_USE_IMU660RA
  ret = imu660ra_init();
#endif

  // 初始化卡尔曼滤波器
  kalman_init(&g_imu.kf_pitch);

  // 清零
  g_imu.pitch_rad = 0.0f;
  g_imu.pitch_gyro_rad_s = 0.0f;
  g_imu.yaw_gyro_rad_s = 0.0f;

  return ret;
}

// ============================================================================
//  公开函数：IMU 数据更新 (在 PIT 中断中调用)
// ============================================================================
void imu_task_update(void) {
#if IMU_USE_IMU660RA
  // ---- 读取原始数据 ----
  imu660ra_get_acc();
  imu660ra_get_gyro();

  g_imu.acc_x = imu660ra_acc_x;
  g_imu.acc_y = imu660ra_acc_y;
  g_imu.acc_z = imu660ra_acc_z;
  g_imu.gyro_x = imu660ra_gyro_x;
  g_imu.gyro_y = imu660ra_gyro_y;
  g_imu.gyro_z = imu660ra_gyro_z;

  // ---- 转换为物理量 ----
  g_imu.acc_x_g = imu660ra_acc_transition(g_imu.acc_x); // 单位: g
  g_imu.acc_y_g = imu660ra_acc_transition(g_imu.acc_y);
  g_imu.acc_z_g = imu660ra_acc_transition(g_imu.acc_z);

  g_imu.gyro_x_dps = imu660ra_gyro_transition(g_imu.gyro_x); // 单位: °/s
  g_imu.gyro_y_dps = imu660ra_gyro_transition(g_imu.gyro_y);
  g_imu.gyro_z_dps = imu660ra_gyro_transition(g_imu.gyro_z);
#endif

  // ---- 加速度计计算 Pitch 角 (rad) ----
  // 假设 acc_x 为前后方向, acc_z 为竖直方向
  // pitch = atan2(acc_x, acc_z)
  float acc_pitch_rad = atan2f(g_imu.acc_x_g, g_imu.acc_z_g);

  // 陀螺仪 Y 轴角速度 => Pitch 角速度 (°/s -> rad/s)
  float gyro_pitch_rad_s = g_imu.gyro_y_dps * (3.14159265f / 180.0f);

  // ---- 卡尔曼滤波融合 ----
  g_imu.pitch_rad = kalman_update(&g_imu.kf_pitch, acc_pitch_rad,
                                  gyro_pitch_rad_s, CONTROL_PERIOD_S);
  g_imu.pitch_gyro_rad_s = gyro_pitch_rad_s - g_imu.kf_pitch.gyro_bias;

  // ---- 偏航角速度 (Z 轴陀螺仪) ----
  g_imu.yaw_gyro_rad_s = g_imu.gyro_z_dps * (3.14159265f / 180.0f);
}
