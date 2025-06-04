
#include "../../../cnc.h"
#include "../../../modules/pca9685.h"

/**
 * Pen revoler
 * speed == 0 => toggle (up / down / up)
 * positive speed = switch pen to absolute value (e.g. speed 1 = 1st pen)
 *
 * */

#ifndef PEN_CHANGING_SERVO_NUM
#define PEN_CHANGING_SERVO_NUM 0
#endif

#ifndef PEN_TOGGLE_SERVO_NUM
#define PEN_TOGGLE_SERVO_NUM 1
#endif

#ifndef PEN_TOGGLE_SERVO_HIGH_POS
#define PEN_TOGGLE_SERVO_HIGH_POS 470
#endif

#ifndef PEN_TOGGLE_SERVO_LOW_POS
#define PEN_TOGGLE_SERVO_LOW_POS 79
#endif

uint16_t pen_changing_servo_positions[5] = {
    72UL,
    154UL,
    242UL,
    334UL,
    424UL
}; // 0 .. 4096

// uint16_t pen_changing_servo_positions[5] = {
//     108UL,
//     190UL,
//     278UL,
//     370UL,
//     460UL
// }; // 0 .. 4096

uint16_t current_pos_revolver = 50UL;
bool is_pen_engaged = false;

static void startup_code(void)
{
    DBGMSG("pen_revolver startup_code()");
    pca9685_begin(0);
    pca9685_setPWMFreq(50.0f); //50 Hz
    cnc_delay_ms(10);
}

static void shutdown_code(void)
{

}

static int16_t range_speed(int16_t value, uint8_t conv)
{
    // DBGMSG("pen_revolver range_speed(%d, %d)", value, conv);
	return value;
}

static void gradual_move(uint8_t servo_num, uint16_t from_val, uint16_t to_val) {
    if (to_val == from_val) {
        return;
    }

    uint16_t delta = abs(to_val - from_val);
    uint16_t speed = 10;
    uint32_t delay = 20;
    // if (delta > 1000) {
    //     speed = 500;
    //     delay = 50;
    // } else if (delta > 500) {
    //     speed = 100;
    //     delay = 30;
    // } else if (delta > 100) {
    //     speed = 50;
    //     delay = 20;
    // } else if (delta > 50) {
    //     speed = 10;
    //     delay = 20;
    // } else if (delta > 10) {
    //     speed = 5;
    //     delay = 10;
    // }

    int8_t direction = 1;
    if (to_val < from_val) {
        direction = -1;
    }

    DBGMSG("pen_revolver gradual_move(%d, %d, %d), direction=%d, delta=%d, speed=%d, delay=%d", servo_num, from_val, to_val, direction, delta, speed, delay);

    for (uint16_t i = 0; i < delta; i = i + speed) {
        pca9685_setPWM(servo_num, 0, from_val + i * direction);
        cnc_delay_ms(delay);
    }

    pca9685_setPWM(servo_num, 0, to_val);
    cnc_delay_ms(delay);
}

static void change_pen(int16_t pen_number) {
    DBGMSG("pen_revolver change_pen(%d)", pen_number);
    if (pen_number < 1 || pen_number > 5) {
        return;
    }
    // turn servo #1 via PCA9685 
    uint16_t pos = pen_changing_servo_positions[pen_number - 1];
    DBGMSG("pen_revolver change_pen(), pos=%d", pos);
    gradual_move(PEN_CHANGING_SERVO_NUM, current_pos_revolver, pos);
    current_pos_revolver = pos;
    cnc_delay_ms(1000);
}

static void tune_pen(int16_t pwm_value) {
    DBGMSG("pen_revolver tune_pen(%d)", pwm_value);

    // turn servo #1 via PCA9685 
    uint16_t pos = pwm_value;
    DBGMSG("pen_revolver tune_pen(), pos=%d", pos);
    gradual_move(PEN_CHANGING_SERVO_NUM, current_pos_revolver, pos);
    current_pos_revolver = pos;
    cnc_delay_ms(1000);
}

static void release_pen_state() {
    DBGMSG("pen_revolver release_pen_state()");
    is_pen_engaged = false;
}

static void toggle_pen() {
    DBGMSG("pen_revolver toggle_pen()");
    if (is_pen_engaged) {
        DBGMSG("pen was not released; duplicate call?");
        return;
    }
    is_pen_engaged = true;
    // turn servo #2 via PCA9685

    // move to the top position if it is not
    pca9685_setPWM(PEN_TOGGLE_SERVO_NUM, 0, PEN_TOGGLE_SERVO_HIGH_POS);
    cnc_delay_ms(500);

    // down
    gradual_move(PEN_TOGGLE_SERVO_NUM, PEN_TOGGLE_SERVO_HIGH_POS, PEN_TOGGLE_SERVO_LOW_POS);
    cnc_delay_ms(500);

    // up
    gradual_move(PEN_TOGGLE_SERVO_NUM, PEN_TOGGLE_SERVO_LOW_POS, PEN_TOGGLE_SERVO_HIGH_POS);
    cnc_delay_ms(500);
}

static void set_speed(int16_t value)
{
    DBGMSG("pen_revolver set_speed(%d)", value);

    if (value == -1) {
        release_pen_state();
    }

    if (value == -5) {
        toggle_pen();
    }

    if (value < -50) {
        tune_pen(value * -1);
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
