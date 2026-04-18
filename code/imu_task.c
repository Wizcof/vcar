/*********************************************************************************************************************
 * @file        imu_task.c
 * @brief       IMU 数据采集与 EKF 姿态解算 —— 实现文件
 ********************************************************************************************************************/
#include "imu_task.h"
#include <math.h>

imu_data_t g_imu;

// --- 移植的 EKF 俯仰角滤波器 (单位适配为 rad) ---
static float Kalman_Filter_EKF_Pitch(float Accel_rad, float Gyro_rads) {
    static float angle = 0.0f;
    static float Q_bias = 0.0f;
    static float PP[2][2] = {{1, 0}, {0, 1}};

    float Q_angle = 0.001f;
    float Q_gyro = 0.003f;
    float R_angle = 0.4f;

    float angle_pred = angle + (Gyro_rads - Q_bias) * CONTROL_PERIOD_S;
    float bias_pred = Q_bias;

    float F[2][2] = {
        {1, -CONTROL_PERIOD_S},
        {0, 1}
    };

    float FP[2][2];
    FP[0][0] = F[0][0] * PP[0][0] + F[0][1] * PP[1][0];
    FP[0][1] = F[0][0] * PP[0][1] + F[0][1] * PP[1][1];
    FP[1][0] = F[1][0] * PP[0][0] + F[1][1] * PP[1][0];
    FP[1][1] = F[1][0] * PP[0][1] + F[1][1] * PP[1][1];

    PP[0][0] = FP[0][0] * F[0][0] + FP[0][1] * F[0][1] + Q_angle;
    PP[0][1] = FP[0][0] * F[1][0] + FP[0][1] * F[1][1];
    PP[1][0] = FP[1][0] * F[0][0] + FP[1][1] * F[0][1];
    PP[1][1] = FP[1][0] * F[1][0] + FP[1][1] * F[1][1] + Q_gyro;

    float Angle_err = Accel_rad - angle_pred;
    float H[2] = {1, 0};

    float S = H[0] * PP[0][0] * H[0] + H[1] * PP[1][0] * H[0] + R_angle;
    float K[2];
    K[0] = PP[0][0] * H[0] / S;
    K[1] = PP[1][0] * H[0] / S;

    angle = angle_pred + K[0] * Angle_err;
    Q_bias = bias_pred + K[1] * Angle_err;

    PP[0][0] -= K[0] * PP[0][0];
    PP[0][1] -= K[0] * PP[0][1];
    PP[1][0] -= K[1] * PP[0][0];
    PP[1][1] -= K[1] * PP[0][1];

    g_imu.pitch_gyro_rad_s = Gyro_rads - Q_bias; // 记录去零偏后的真实角速度
    return angle;
}

uint8 imu_task_init(void) {
    uint8 ret = 0;
#if IMU_USE_IMU660RA
    ret = imu660ra_init();
#endif
    return ret;
}

void imu_task_update(void) {
#if IMU_USE_IMU660RA
    // 读取原始数据并转换单位
    imu660ra_get_acc();
    imu660ra_get_gyro();
    // 假设已有单位转换宏，统一转换为 g 和 rad/s
    g_imu.acc_x_g = imu660ra_acc_transition(imu660ra_acc_x);
    g_imu.acc_z_g = imu660ra_acc_transition(imu660ra_acc_z);
    g_imu.gyro_y_rads = imu660ra_gyro_transition(imu660ra_gyro_y) * (3.14159265f / 180.0f);
    g_imu.gyro_z_rads = imu660ra_gyro_transition(imu660ra_gyro_z) * (3.14159265f / 180.0f);
#endif

    // 加速度计计算观测俯仰角 (rad)
    float acc_pitch_rad = atan2f(g_imu.acc_x_g, g_imu.acc_z_g);

    // EKF 融合
    g_imu.pitch_rad = Kalman_Filter_EKF_Pitch(acc_pitch_rad, g_imu.gyro_y_rads);
    
    // 偏航角速度
    g_imu.yaw_gyro_rad_s = g_imu.gyro_z_rads;
    
    // (可选) 如果有磁力计数据，可在此处调用移植过来的 calculate_yaw() 进行融合
}