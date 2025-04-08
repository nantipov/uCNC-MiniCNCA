#include "../cnc.h"
#include "pca9685.h"
#include "softi2c.h"

uint32_t _oscillator_freq = FREQUENCY_OSCILLATOR;

bool write_buf(uint8_t* buf, uint8_t data_len) {
#ifdef MCU_HAS_I2C
    return mcu_i2c_send(PCA9685_I2C_ADDRESS, buf, data_len, false, 1000);
#else
    //return softi2c_send(null, PCA9685_I2C_ADDRESS, buf, data_len, false, 1000);
    return false;
#endif
}

uint8_t read8(uint8_t addr) {
  uint8_t buffer_write[1] = {addr};
  uint8_t buffer_read[1] = {0};
  write_buf(buffer_write, 1);
#ifdef MCU_HAS_I2C
  mcu_i2c_receive(PCA9685_I2C_ADDRESS, buffer_read, 1, 1000);
#else
  softi2c_receive(null, PCA9685_I2C_ADDRESS, buffer_read, 2, false, 1000);
#endif
  return buffer_read[0];
}

// #ifndef mcu_i2c_send
// 	// master sends command to slave
// 	uint8_t mcu_i2c_send(uint8_t address, uint8_t *data, uint8_t datalen, bool release, uint32_t ms_timeout);
// #endif
// #ifndef mcu_i2c_receive
// 	// master receive response from slave
// 	uint8_t mcu_i2c_receive(uint8_t address, uint8_t *data, uint8_t datalen, uint32_t ms_timeout);
// #endif
//void mcu_i2c_config(uint32_t frequency);

	// uint8_t softi2c_send(softi2c_port_t *port, uint8_t address, uint8_t *data, uint8_t len, bool release, uint32_t ms_timeout);
	// uint8_t softi2c_receive(softi2c_port_t *port, uint8_t address, uint8_t *data, uint8_t len, uint32_t ms_timeout);
	// void softi2c_config(softi2c_port_t *port, uint32_t frequency);

void write8(uint8_t addr, uint8_t d) {
  uint8_t buffer[2] = {addr, d};
  write_buf(buffer, 2);
}

void pca9685_reset() {
  write8(PCA9685_MODE1, MODE1_RESTART);
  mcu_delay_us(10 * 1000);
}

void pca9685_setExtClk(uint8_t prescale) {
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep, turn off internal oscillator

  // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
  // use the external clock.
  write8(PCA9685_MODE1, (newmode |= MODE1_EXTCLK));

  write8(PCA9685_PRESCALE, prescale); // set the prescaler

  mcu_delay_us(5 * 1000);
  // clear the SLEEP bit to start
  write8(PCA9685_MODE1, (newmode & ~MODE1_SLEEP) | MODE1_RESTART | MODE1_AI);
}

void pca9685_setPWMFreq(float freq) {
  // Range output modulation frequency is dependant on oscillator
  if (freq < 1)
    freq = 1;
  if (freq > 3500)
    freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

  float prescaleval = ((_oscillator_freq / (freq * 4096.0)) + 0.5) - 1;
  if (prescaleval < PCA9685_PRESCALE_MIN)
    prescaleval = PCA9685_PRESCALE_MIN;
  if (prescaleval > PCA9685_PRESCALE_MAX)
    prescaleval = PCA9685_PRESCALE_MAX;
  uint8_t prescale = (uint8_t)prescaleval;

  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode);                             // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  mcu_delay_us(5 * 1000);
  // This sets the MODE1 register to turn on auto increment.
  write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
}

bool pca9685_begin(uint8_t prescale) {
  // if (i2c_dev)
  //   delete i2c_dev;
  // i2c_dev = new Adafruit_I2CDevice(_i2caddr, _i2c);
  // if (!i2c_dev->begin())
  //   return false;
  pca9685_reset();

  if (prescale) {
    pca9685_setExtClk(prescale);
  } else {
    // set a default frequency
    pca9685_setPWMFreq(1000);
  }

  return true;
}

uint8_t pca9685_setPWM(uint8_t num, uint16_t on, uint16_t off) {
  uint8_t buffer[5];
  buffer[0] = PCA9685_LED0_ON_L + 4 * num;
  buffer[1] = on;
  buffer[2] = on >> 8;
  buffer[3] = off;
  buffer[4] = off >> 8;

  if (write_buf(buffer, 5)) {
    return 0;
  } else {
    return 1;
  }
}
