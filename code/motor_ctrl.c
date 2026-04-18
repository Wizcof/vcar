/*********************************************************************************************************************
 * @file        motor_ctrl.c
 * @brief       双路电机 PWM 驱动与编码器速度采集 —— 实现文件
 ********************************************************************************************************************/
#include "motor_ctrl.h"

// ============================================================================
//  全局电机数据
// ============================================================================
motor_data_t g_motor;

// ============================================================================
//  内部辅助：限幅
// ============================================================================
static int32 clamp_i32(int32 val, int32 min_val, int32 max_val) {
  if (val > max_val)
    return max_val;
  if (val < min_val)
    return min_val;
  return val;
}

// ============================================================================
//  初始化
// ============================================================================
void motor_ctrl_init(void) {
#if MOTOR_USE_BRUSHLESS_UART
  // ---- 1. 无刷电机：初始化串口通信 ----
  uart_init(MOTOR_UART, MOTOR_BAUDRATE, MOTOR_UART_TX, MOTOR_UART_RX);
  uart_rx_interrupt(MOTOR_UART, 1);    // 使能串口接收中断
  
  // 清零串口缓冲区
  memset(g_motor.send_data_buffer, 0, 7);
  memset(g_motor.receive_data_buffer, 0, 7);
  g_motor.receive_data_count = 0;
  g_motor.sum_check_data = 0;
  g_motor.recv_speed_right = 0;
  g_motor.recv_speed_left = 0;

  // 顺便初始化一下实体编码器外设，以防用户想要读取真实的外挂编码器作为参考
  encoder_quad_init(ENCODER_L, ENCODER_L_CH1, ENCODER_L_CH2);
  encoder_quad_init(ENCODER_R, ENCODER_R_CH1, ENCODER_R_CH2);

  // 设置 0 占空比并获取实时速度数据
  motor_ctrl_set_pwm(0, 0);
  motor_driver_get_speed();

#else
  // ---- 2. 有刷电机：初始化 PWM 与编码器 ----
  pwm_init(MOTOR_L_PWM_A, MOTOR_PWM_FREQ, 0);
  pwm_init(MOTOR_L_PWM_B, MOTOR_PWM_FREQ, 0);
  pwm_init(MOTOR_R_PWM_A, MOTOR_PWM_FREQ, 0);
  pwm_init(MOTOR_R_PWM_B, MOTOR_PWM_FREQ, 0);

  // 初始化编码器 (正交模式)
  encoder_quad_init(ENCODER_L, ENCODER_L_CH1, ENCODER_L_CH2);
  encoder_quad_init(ENCODER_R, ENCODER_R_CH1, ENCODER_R_CH2);
#endif

  // ---- 清零数据 ----
  g_motor.enc_left_raw = 0;
  g_motor.enc_right_raw = 0;
  g_motor.speed_left_mps = 0.0f;
  g_motor.speed_right_mps = 0.0f;
  g_motor.speed_avg_mps = 0.0f;
  g_motor.distance_m = 0.0f;
}

// ============================================================================
//  编码器更新 (在 PIT 中断中调用)
// ============================================================================
/*void motor_ctrl_update_encoder(void) {
#if MOTOR_USE_BRUSHLESS_UART
  // ---- 1. 无刷电机：读取串口接收到的转速数据 ----
  // 直接采用中断中拼装的 recv_speed_left / right 数据
  // 此处可根据转速向 m/s 转换 (假设转速单位也是类似编码器差分变化)
  g_motor.enc_left_raw = g_motor.recv_speed_left;
  g_motor.enc_right_raw = g_motor.recv_speed_right;
  
  // v = (rpm转每分 / 60) * 周长 -> 或者由底层固件特定单位决定，此处假设与轮毂有刷同比例或直接使用factor
  // 由于 CYT2BL3 底层直接返回的转速与实际的比例关系需要测试，这里暂时按照原 factor 换算
  // 实际底层反馈如果是 rpm 需要自行乘以 PI*D/60
  // （例程由于没有详细转速单位解释，我们保留并复用原转换逻辑，如果有偏差，后续调整 factor 即可）
  float factor = WHEEL_PERIMETER_M / ((float)ENCODER_PPR * CONTROL_PERIOD_S);
  
  // 无刷驱动中，正负与机器朝向可能需要调整，这里与有刷一致直接相乘
  g_motor.speed_left_mps = (float)g_motor.enc_left_raw * factor;
  g_motor.speed_right_mps = (float)g_motor.enc_right_raw * factor;
*/
  void motor_ctrl_update_encoder(void) {
#if MOTOR_USE_BRUSHLESS_UART
  // 【关键修改】：每次进入 5ms 编码器更新环，主动向驱动器发送速度请求
  // 这样 5ms 后的下一次中断到来前，串口接收中断就能把最新的速度解析好
  motor_driver_get_speed(); 

  // ---- 读取串口接收到的转速数据 ----
  g_motor.enc_left_raw = g_motor.recv_speed_left;
  g_motor.enc_right_raw = g_motor.recv_speed_right;
  
  float factor = WHEEL_PERIMETER_M / ((float)ENCODER_PPR * CONTROL_PERIOD_S);
  
  g_motor.speed_left_mps = (float)g_motor.enc_left_raw * factor;
  g_motor.speed_right_mps = (float)g_motor.enc_right_raw * factor;
#else
  // ---- 2. 有刷电机：读取差分脉冲并清零 ----
  g_motor.enc_left_raw = encoder_get_count(ENCODER_L);
  g_motor.enc_right_raw = encoder_get_count(ENCODER_R);
  encoder_clear_count(ENCODER_L);
  encoder_clear_count(ENCODER_R);

  // ---- 将脉冲转换为线速度 (m/s) ----
  // v = (pulse / PPR) * 周长 / dt
  float factor = WHEEL_PERIMETER_M / ((float)ENCODER_PPR * CONTROL_PERIOD_S);
  g_motor.speed_left_mps = (float)g_motor.enc_left_raw * factor;
  g_motor.speed_right_mps = (float)g_motor.enc_right_raw * factor;
#endif

  // ---- 质心平均速度 ----
  g_motor.speed_avg_mps =
      (g_motor.speed_left_mps + g_motor.speed_right_mps) * 0.5f;

  // ---- 累计位移 ----
  g_motor.distance_m += g_motor.speed_avg_mps * CONTROL_PERIOD_S;
}

// ============================================================================
//  设置电机 PWM (带死区补偿与限幅)
//  正值前进，负值后退
// ============================================================================
  void motor_ctrl_set_pwm(int32 pwm_left, int32 pwm_right) {
    pwm_left = (int32)((float)pwm_left * 1.00);
    pwm_right = (int32)((float)pwm_right * 1.5);
    // ---- 增加死区补偿 ----
    if (pwm_left > 0) pwm_left += MOTOR_L_PWM_DEAD;
    else if (pwm_left < 0) pwm_left -= MOTOR_L_PWM_DEAD;

    if (pwm_right > 0) pwm_right += MOTOR_R_PWM_DEAD;
    else if (pwm_right < 0) pwm_right -= MOTOR_R_PWM_DEAD;

  // ---- 限幅 ----
  pwm_left = clamp_i32(pwm_left, -MOTOR_PWM_MAX, MOTOR_PWM_MAX);
  pwm_right = clamp_i32(pwm_right, -MOTOR_PWM_MAX, MOTOR_PWM_MAX);

#if MOTOR_USE_BRUSHLESS_UART
  // ---- 1. 无刷电机串口发送占空比 ----
  g_motor.send_data_buffer[0] = 0xA5;         // 帧头
  g_motor.send_data_buffer[1] = 0X01;         // 功能字
  g_motor.send_data_buffer[2] = (uint8)(((uint16)pwm_right & 0xFF00) >> 8); // 填入右轮占空比
  g_motor.send_data_buffer[3] = (uint8)((uint16)pwm_right & 0x00FF);        // 填入右轮占空比
  g_motor.send_data_buffer[4] = (uint8)(((uint16)pwm_left & 0xFF00) >> 8);  // 填入左轮占空比
  g_motor.send_data_buffer[5] = (uint8)((uint16)pwm_left & 0x00FF);         // 填入左轮占空比
  g_motor.send_data_buffer[6] = 0;            // 校验清零
  for(int i = 0; i < 6; i ++) {
      g_motor.send_data_buffer[6] += g_motor.send_data_buffer[i]; // 计算校验位
  }
  uart_write_buffer(MOTOR_UART, g_motor.send_data_buffer, 7); // 发送
#else
  // ---- 2. 有刷电机直接输 PWM 到引脚 ----
  // ---- 左电机 (H桥: A正转 B反转) ----
  if (pwm_left >= 0) {
    pwm_set_duty(MOTOR_L_PWM_A, (uint32)pwm_left);
    pwm_set_duty(MOTOR_L_PWM_B, 0);
  } else {
    pwm_set_duty(MOTOR_L_PWM_A, 0);
    pwm_set_duty(MOTOR_L_PWM_B, (uint32)(-pwm_left));
  }

  // ---- 右电机 ----
  if (pwm_right >= 0) {
    pwm_set_duty(MOTOR_R_PWM_A, (uint32)pwm_right);
    pwm_set_duty(MOTOR_R_PWM_B, 0);
  } else {
    pwm_set_duty(MOTOR_R_PWM_A, 0);
    pwm_set_duty(MOTOR_R_PWM_B, (uint32)(-pwm_right));
  }
#endif
}

// ============================================================================
//  获取速度请求 (仅用于初始化开启周期返回数据通道)
// ============================================================================
void motor_driver_get_speed(void) {
#if MOTOR_USE_BRUSHLESS_UART
  g_motor.send_data_buffer[0] = 0xA5;
  g_motor.send_data_buffer[1] = 0X02;
  g_motor.send_data_buffer[2] = 0x00;
  g_motor.send_data_buffer[3] = 0x00;
  g_motor.send_data_buffer[4] = 0x00;
  g_motor.send_data_buffer[5] = 0x00;
  g_motor.send_data_buffer[6] = 0xA7; // 校验位
  uart_write_buffer(MOTOR_UART, g_motor.send_data_buffer, 7);
#endif
}

// ============================================================================
//  无刷电机串口接收解析回调
//  需在对应的串口 RX 中断中调用
// ============================================================================
void motor_uart_callback(void) {
#if MOTOR_USE_BRUSHLESS_UART
  uint8 receive_data; // 定义临时变量
  if(uart_query_byte(MOTOR_UART, &receive_data)) { // 接收串口数据
    if(receive_data == 0xA5 && g_motor.receive_data_buffer[0] != 0xA5) { // 寻找帧头
        g_motor.receive_data_count = 0; // 重置接收计数
    }
    
    g_motor.receive_data_buffer[g_motor.receive_data_count ++] = receive_data;
    
    if(g_motor.receive_data_count >= 7) { // 已接收满7字节
        if(g_motor.receive_data_buffer[0] == 0xA5) { // 检查帧头
            g_motor.sum_check_data = 0;
            for(int i = 0; i < 6; i ++) {
                g_motor.sum_check_data += g_motor.receive_data_buffer[i];
            }
            if(g_motor.sum_check_data == g_motor.receive_data_buffer[6]) { // 校验位正确
                if(g_motor.receive_data_buffer[1] == 0x02) { // 检查指令：速度响应
                    g_motor.recv_speed_right = (int16)(((uint16)g_motor.receive_data_buffer[2] << 8) | g_motor.receive_data_buffer[3]);
                    g_motor.recv_speed_left  = (int16)(((uint16)g_motor.receive_data_buffer[4] << 8) | g_motor.receive_data_buffer[5]);
                }
            }
        }
        g_motor.receive_data_count = 0;
        memset(g_motor.receive_data_buffer, 0, 7); // 清除缓冲区
    }
  }
#endif
}

// ============================================================================
//  紧急停车
// ============================================================================
void motor_ctrl_stop(void) {
#if MOTOR_USE_BRUSHLESS_UART
  motor_ctrl_set_pwm(0, 0);
#else  
  pwm_set_duty(MOTOR_L_PWM_A, 0);
  pwm_set_duty(MOTOR_L_PWM_B, 0);
  pwm_set_duty(MOTOR_R_PWM_A, 0);
  pwm_set_duty(MOTOR_R_PWM_B, 0);
#endif
}

// ============================================================================
//  重置累计位移
// ============================================================================
void motor_ctrl_reset_distance(void) { g_motor.distance_m = 0.0f; }
