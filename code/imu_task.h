/*********************************************************************************************************************
 * @file        imu_task.h
 * @brief       IMU 数据采集与 EKF 卡尔曼滤波姿态解算
 ********************************************************************************************************************/
#ifndef _IMU_TASK_H_
#define _IMU_TASK_H_

#include "robot_config.h"

//#define CONTROL_PERIOD_S 0.005f  // 5ms 控制周期

typedef struct {
  // 原始数据
  int16 gyro_x, gyro_y, gyro_z;
  int16 acc_x, acc_y, acc_z;
  int16 mag_x, mag_y, mag_z; // 新增磁力计数据

  // 物理量
  float gyro_x_rads, gyro_y_rads, gyro_z_rads; // rad/s
  float acc_x_g, acc_y_g, acc_z_g;             // g
  float mag_x_f, mag_y_f, mag_z_f;             // 校准后磁场强度

  // 姿态解算结果 (rad)
  float pitch_rad;        
  float roll_rad;
  float yaw_rad;

  float pitch_gyro_rad_s; 
  float yaw_gyro_rad_s;   
} imu_data_t;

extern imu_data_t g_imu;

uint8 imu_task_init(void);
void imu_task_update(void);

#endif // _IMU_TASK_H_