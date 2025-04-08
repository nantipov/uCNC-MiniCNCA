// boardmap override dummy file

#ifndef BOARDMAP_OVERRIDES_H
#define BOARDMAP_OVERRIDES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "boardmap_reset.h"

#define MCU MCU_RP2040
#define KINEMATIC KINEMATIC_CARTESIAN
#define AXIS_COUNT 3
#define TOOL_COUNT 2
#define BAUDRATE 115200
#define BOARD BOARD_RPI_PICO
#define BOARD_NAME "MiniCNCA"
#define UART_PORT 0
#define UART2_PORT 1
#define SPI_PORT 0
#define I2C_PORT 0
#define ITP_TIMER 1
#define RTC_TIMER 2

// dir, step, en
// x - 2, 3, 4
// y - 6, 7, 8
// z - 10, 11, 12

#define STEP0_EN_BIT 4
#define STEP0_BIT 3
#define DIR0_BIT 2

#define STEP1_EN_BIT 8
#define STEP1_BIT 7
#define DIR1_BIT 6

#define STEP2_EN_BIT 12
#define STEP2_BIT 11
#define DIR2_BIT 10


// #define PWM0_BIT 14
// #define DOUT0_BIT 15

// #define LIMIT_X_BIT 5
// //#define LIMIT_X_PULLUP
// #define LIMIT_Y_BIT 9
// //#define LIMIT_Y_PULLUP
// #define LIMIT_Z_BIT 13
// //#define LIMIT_Z_PULLUP
// #define LIMIT_X2_BIT 21
// //#define LIMIT_X2_PULLUP
// #define LIMIT_Y2_BIT 20
// //#define LIMIT_Y2_PULLUP
// #define LIMIT_Z2_BIT 19
// //#define LIMIT_Z2_PULLUP

// #define TX_BIT 0
// #define RX_BIT 1

// //#define I2C_CLK_BIT 27
// //#define I2C_DATA_BIT 26

#define I2C_CLK_BIT 17
#define I2C_DATA_BIT 16

// //#define PROBE_BIT 17
// #define ESTOP_BIT 16
// //#define ESTOP_BIT 14
// #define ESTOP_PULLUP
// //#define IC74HC595_COUNT 4
// //#define IC74HC595_DATA DOUT8
// //#define IC74HC595_CLK DOUT9
// //#define IC74HC595_LATCH DOUT10
// //#define IC74HC595_DELAY_CYCLES 0
// //Custom configurations



#ifdef __cplusplus
}
#endif

#endif