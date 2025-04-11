#include "../cnc.h"
#include "pca9685.h"
#include "softi2c.h"

uint32_t _oscillator_freq = FREQUENCY_OSCILLATOR;

bool write_buf(uint8_t* buf, uint8_t data_len, bool release) {
  // DBGMSG("pca9685 write_buf(), size=%d", data_len);

#ifdef MCU_HAS_I2C
    return mcu_i2c_send(PCA9685_I2C_ADDRESS, buf, data_len, release, 0);
#else
    //return softi2c_send(null, PCA9685_I2C_ADDRESS, buf, data_len, false, 1000);
    return false;
#endif
}

uint8_t read8(uint8_t addr) {
  // DBGMSG("pca9685 read8(), addr=%d", addr);
  uint8_t buffer_write[] = {addr};
  uint8_t buffer_read[] = {0};
  write_buf(buffer_write, 1, true);
#ifdef MCU_HAS_I2C
  mcu_i2c_receive(PCA9685_I2C_ADDRESS, buffer_read, 1, 0);
#else
  softi2c_receive(null, PCA9685_I2C_ADDRESS, buffer_read, 2, false, 1000);
#endif
  // DBGMSG("pca9685 read8(), buffer_read[0]=%d", buffer_read[0]);
  return buffer_read[0];
}


void write8(uint8_t addr, uint8_t d, bool release) {
  // DBGMSG("pca9685 write8(), addr=%d, d=%d", addr, d);
  uint8_t buffer[2] = {addr, d};
  write_buf(buffer, 2, release);
}

void pca9685_reset() {
  DBGMSG("pca9685 reset()");
  write8(PCA9685_MODE1, MODE1_RESTART, true);
  cnc_delay_ms(10);
}

void pca9685_setExtClk(uint8_t prescale) {
  DBGMSG("pca9685 pca9685_setExtClk(%d)", prescale);
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode, false); // go to sleep, turn off internal oscillator

  // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
  // use the external clock.
  write8(PCA9685_MODE1, (newmode |= MODE1_EXTCLK), false);

  write8(PCA9685_PRESCALE, prescale, false); // set the prescaler

  cnc_delay_ms(5);
  // clear the SLEEP bit to start
  write8(PCA9685_MODE1, (newmode & ~MODE1_SLEEP) | MODE1_RESTART | MODE1_AI, true);
}

void pca9685_wakeup() {
  DBGMSG("pca9685 pca9685_wakeup()");
  uint8_t sleep = read8(PCA9685_MODE1);
  uint8_t wakeup = sleep & ~MODE1_SLEEP; // set sleep bit low
  write8(PCA9685_MODE1, wakeup, true);
}

void pca9685_setPWMFreq(float freq) {
  DBGMSG("pca9685 pca9685_setPWMFreq(%f)", freq);
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

  DBGMSG("pca9685 pca9685_setPWMFreq(), prescale=%d", prescale);

  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep

  DBGMSG("pca9685 pca9685_setPWMFreq(), oldmode=%d, newmode=%d", oldmode, newmode);

  write8(PCA9685_MODE1, newmode, false);     // go to sleep
  write8(PCA9685_PRESCALE, prescale, false); // set the prescaler

  write8(PCA9685_MODE1, oldmode, false);
  cnc_delay_ms(5);
  // This sets the MODE1 register to turn on auto increment.
  write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI, true);

  // DBGMSG("pca9685 pca9685_setPWMFreq(), mode now=%d", read8(PCA9685_MODE1));
  // DBGMSG("pca9685 pca9685_setPWMFreq(), prescale now=%d", read8(PCA9685_PRESCALE));
}

bool pca9685_begin(uint8_t prescale) {
  DBGMSG("pca9685 begin(), prescale=%d", prescale);
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

  // DBGMSG("pca9685 pca9685_setPWM(%d, %d, %d), buf=%d %d %d %d %d", num, on, off, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);

  if (write_buf(buffer, 5, true)) {
    return 0;
  } else {
    return 1;
  }
}
