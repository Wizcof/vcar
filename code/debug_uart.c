/*********************************************************************************************************************
 * @file        debug_uart.c
 * @brief       串口调试输出与串口指令解析 —— 实现文件
 * @note        上位机推荐使用 VOFA+ (JustFloat 协议) 实时观察波形
 ********************************************************************************************************************/
#include "debug_uart.h"
#include "balance_ctrl.h"
#include "imu_task.h"
#include "motor_ctrl.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


// ============================================================================
//  串口接收缓冲区
// ============================================================================
#define RX_BUF_SIZE (64)
static uint8 rx_buf[RX_BUF_SIZE];
static uint8 rx_index = 0;

// ============================================================================
//  VOFA+ JustFloat 帧尾 (4字节: 0x00 0x00 0x80 0x7F)
// ============================================================================
static const uint8 vofa_tail[4] = {0x00, 0x00, 0x80, 0x7F};

// ============================================================================
//  初始化
// ============================================================================
void debug_uart_init(void) {
  uart_init(DEBUG_UART, DEBUG_UART_BAUD, DEBUG_UART_TX, DEBUG_UART_RX);
  rx_index = 0;
  memset(rx_buf, 0, sizeof(rx_buf));
}

// ============================================================================
//  VOFA+ JustFloat 协议发送
//  每帧: [ch0_float32][ch1_float32]...[chN_float32][0x00 0x00 0x80 0x7F]
// ============================================================================
void debug_uart_send_vofa(float *data, uint8 channel_cnt) {
  if (channel_cnt > VOFA_CHANNEL_MAX)
    channel_cnt = VOFA_CHANNEL_MAX;

  // 发送各通道浮点数据 (小端序，直接发送内存)
  uart_write_buffer(DEBUG_UART, (const uint8 *)data,
                    channel_cnt * sizeof(float));

  // 发送帧尾
  uart_write_buffer(DEBUG_UART, vofa_tail, 4);
}

// ============================================================================
//  格式化打印
// ============================================================================
void debug_uart_printf(const char *fmt, ...) {
  static char print_buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(print_buf, sizeof(print_buf), fmt, args);
  va_end(args);
  uart_write_string(DEBUG_UART, print_buf);
}

// ============================================================================
//  轮询解析串口指令 (非阻塞)
//  简易协议:
//    单字节指令: 'E' 使能, 'D' 禁用, 'W' 前进, 'S' 后退, 'A' 左转, 'd' 右转,
//    'X' 停 参数指令:   "K1=350.0\n" — 设置 LQR k1
// ============================================================================
debug_cmd_t debug_uart_poll_cmd(void) {
  uint8 dat;

  while (uart_query_byte(DEBUG_UART, &dat)) {
    // 单字节指令检测
    if (rx_index == 0) {
      switch (dat) {
      case 'E':
        return CMD_ENABLE;
      case 'D':
        return CMD_DISABLE;
      case 'W':
        return CMD_FORWARD;
      case 'S':
        return CMD_BACKWARD;
      case 'A':
        return CMD_LEFT;
      case 'd':
        return CMD_RIGHT;
      case 'X':
        return CMD_STOP;
      case 'K':
        // 进入多字节参数模式
        rx_buf[rx_index++] = dat;
        break;
      default:
        break;
      }
    } else {
      // 多字节模式：收集到 '\n' 或缓冲区满
      if (dat == '\n' || dat == '\r' || rx_index >= RX_BUF_SIZE - 1) {
        rx_buf[rx_index] = '\0';

        // 解析 "Kn=xxx.x"
        debug_cmd_t cmd = CMD_NONE;
        float val = 0.0f;

        if (rx_buf[0] == 'K' && rx_buf[2] == '=') {
          sscanf((const char *)&rx_buf[3], "%f", &val);
          switch (rx_buf[1]) {
          case '1':
            cmd = CMD_SET_K1;
            break;
          case '2':
            cmd = CMD_SET_K2;
            break;
          case '3':
            cmd = CMD_SET_K3;
            break;
          case '4':
            cmd = CMD_SET_K4;
            break;
          default:
            break;
          }

          // 立即应用参数
          if (cmd != CMD_NONE) {
            switch (cmd) {
            case CMD_SET_K1:
              // K1 对应 Pitch (俯仰角) 权重，即状态变量 x[2]
              g_balance.K[0][2] = -val; 
              g_balance.K[1][2] = -val;
              break;
            case CMD_SET_K2:
              // K2 对应 Pitch Rate (俯仰角速度) 权重，即状态变量 x[3]
              g_balance.K[0][3] = -val;
              g_balance.K[1][3] = -val;
              break;
            case CMD_SET_K3:
              // K3 对应 位移 权重，即状态变量 x[0]
              g_balance.K[0][0] = -val;
              g_balance.K[1][0] = -val;
              break;
            case CMD_SET_K4:
              // K4 对应 速度 权重，即状态变量 x[1]
              g_balance.K[0][1] = -val;
              g_balance.K[1][1] = -val;
              break;
            default:
              break;
            }
            debug_uart_printf("OK: K%c=%.2f\r\n", rx_buf[1], val);
          }
        }

        rx_index = 0;
        return cmd;
      } else {
        rx_buf[rx_index++] = dat;
      }
    }
  }

  return CMD_NONE;
}

// ============================================================================
//  处理指令 (在 CPU1 主循环中调用)
// ============================================================================
void debug_uart_process_cmd(void) {
  debug_cmd_t cmd = debug_uart_poll_cmd();

  switch (cmd) {
  case CMD_ENABLE:
    balance_ctrl_enable(1);
    debug_uart_printf("Balance ENABLED\r\n");
    break;

  case CMD_DISABLE:
    balance_ctrl_enable(0);
    debug_uart_printf("Balance DISABLED\r\n");
    break;

  case CMD_FORWARD:
    balance_ctrl_set_target(0.3f, 0.0f); // 前进 0.3 m/s
    debug_uart_printf("CMD: Forward\r\n");
    break;

  case CMD_BACKWARD:
    balance_ctrl_set_target(-0.3f, 0.0f); // 后退
    debug_uart_printf("CMD: Backward\r\n");
    break;

  case CMD_LEFT:
    balance_ctrl_set_target(0.0f, 1.0f); // 左转 1 rad/s
    debug_uart_printf("CMD: Left\r\n");
    break;

  case CMD_RIGHT:
    balance_ctrl_set_target(0.0f, -1.0f); // 右转
    debug_uart_printf("CMD: Right\r\n");
    break;

  case CMD_STOP:
    balance_ctrl_set_target(0.0f, 0.0f); // 停止运动
    debug_uart_printf("CMD: Stop\r\n");
    break;

  default:
    break;
  }
}
