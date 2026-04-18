/*********************************************************************************************************************
 * @file        debug_uart.h
 * @brief       串口调试输出与串口指令解析（含虚拟示波器协议）
 ********************************************************************************************************************/
#ifndef _DEBUG_UART_H_
#define _DEBUG_UART_H_

#include "robot_config.h"

// ============================================================================
//  虚拟示波器最多发送通道数
// ============================================================================
#define VOFA_CHANNEL_MAX (8)

// ============================================================================
//  串口接收指令类型
// ============================================================================
typedef enum {
  CMD_NONE = 0, // 无指令
  CMD_ENABLE,   // 使能平衡 'E'
  CMD_DISABLE,  // 禁用平衡 'D'
  CMD_FORWARD,  // 前进 'W'
  CMD_BACKWARD, // 后退 'S'
  CMD_LEFT,     // 左转 'A'
  CMD_RIGHT,    // 右转 'd'  (小写)
  CMD_STOP,     // 停止运动 'X'
  CMD_SET_K1,   // 设置 LQR k1 (格式: "K1=xxx.x\n")
  CMD_SET_K2,
  CMD_SET_K3,
  CMD_SET_K4,
} debug_cmd_t;

// ============================================================================
//  公开函数
// ============================================================================

/**
 * @brief   初始化调试串口
 */
void debug_uart_init(void);

/**
 * @brief   通过 VOFA+ JustFloat 协议发送多通道浮点数据到上位机
 * @param   data        浮点数组
 * @param   channel_cnt 通道数 (不超过 VOFA_CHANNEL_MAX)
 */
void debug_uart_send_vofa(float *data, uint8 channel_cnt);

/**
 * @brief   格式化输出一行调试信息
 */
void debug_uart_printf(const char *fmt, ...);

/**
 * @brief   在 CPU1 或主循环中调用，轮询解析串口接收到的指令
 * @return  解析出的指令类型
 */
debug_cmd_t debug_uart_poll_cmd(void);

/**
 * @brief   处理接收到的指令，修改平衡控制器参数
 * @note    应在 CPU1 主循环中调用
 */
void debug_uart_process_cmd(void);

#endif // _DEBUG_UART_H_
