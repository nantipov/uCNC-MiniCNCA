
#include "../../../cnc.h"
#include "../../../modules/pca9685.h"

/**
 * Pen revoler
 * speed == 0 => toggle
 * positive speed = expect number of a pen
 *
 * */

#ifndef PEN_CHANGING_SERVO_NUM
#define PEN_CHANGING_SERVO_NUM 0
#endif

#ifndef PEN_TOGGLE_SERVO_NUM
#define PEN_TOGGLE_SERVO_NUM 1
#endif

#ifndef PEN_TOGGLE_SERVO_HIGH_POS
#define PEN_TOGGLE_SERVO_HIGH_POS 50
#endif

#ifndef PEN_TOGGLE_SERVO_LOW_POS
#define PEN_TOGGLE_SERVO_LOW_POS 2500
#endif

uint16_t pen_changing_servo_positions[5] = {
    50,
    1200,
    2000,
    2500,
    3500
}; // 0 .. 4096

static void startup_code(void)
{
    DBGMSG("pen_revolver startup_code()");
    pca9685_begin(0);
    pca9685_setPWMFreq(50.0f); //50 MHz
}

static void shutdown_code(void)
{

}

static int16_t range_speed(int16_t value, uint8_t conv)
{
    // DBGMSG("pen_revolver range_speed(%d, %d)", value, conv);
	return value;
}

static void change_pen(int16_t pen_number) {
    DBGMSG("pen_revolver change_pen(%d)", pen_number);
    if (pen_number < 1 || pen_number > 5) {
        return;
    }
    // turn servo #1 via PCA9685 
    pca9685_setPWM(PEN_CHANGING_SERVO_NUM, 0, pen_changing_servo_positions[pen_number]);
    mcu_delay_us(1000 * 1000);
}

static void toggle_pen() {
    DBGMSG("pen_revolver toggle_pen()");
    // turn servo #2 via PCA9685
    pca9685_setPWM(PEN_TOGGLE_SERVO_NUM, 0, PEN_TOGGLE_SERVO_HIGH_POS);
    mcu_delay_us(1000 * 1000);
    pca9685_setPWM(PEN_TOGGLE_SERVO_NUM, 0, PEN_TOGGLE_SERVO_LOW_POS);
    mcu_delay_us(1000 * 1000);
    pca9685_setPWM(PEN_TOGGLE_SERVO_NUM, 0, PEN_TOGGLE_SERVO_HIGH_POS);
    mcu_delay_us(1000 * 1000);
}

static void set_speed(int16_t value)
{
    DBGMSG("pen_revolver set_speed(%d)", value);
    if (value < 0) {
        return;
    }

    if (value == 0) {
        toggle_pen();
    }

    if (value > 0) {
        change_pen(value);
    }
}

const tool_t pen_revolver = {
	.startup_code = &startup_code,
	.shutdown_code = &shutdown_code,
	.pid_update = NULL,
	.range_speed = &range_speed,
	.get_speed = NULL,
	.set_speed = &set_speed,
	.set_coolant = NULL};
