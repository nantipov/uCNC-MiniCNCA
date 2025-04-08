// boardmap override dummy file

#ifndef CNC_HAL_OVERRIDES_H
#define CNC_HAL_OVERRIDES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cnc_hal_reset.h"

#define S_CURVE_ACCELERATION_LEVEL 0
#define DISABLE_ALL_CONTROLS
#define DISABLE_ALL_LIMITS
#define DISABLE_PROBE
#define TOOL1 spindle_pwm
#define SPINDLE_PWM PWM0
#define SPINDLE_PWM_DIR DOUT0
#define ENCODERS 0
#define DISABLE_SAFE_SETTINGS
#define DEFAULT_LIMIT_INV_MASK 0b00000111 // invert limits for X1 X2 Y1 Y2 Z1 Z2
#define DEFAULT_STEP_INV_MASK 255 // invert EN-pin for TMC2208
#define DEFAULT_STEP_PER_MM 1600
#define DEFAULT_MAX_FEED 5000
//Custom configurations

#ifdef __cplusplus
}
#endif

#endif