#include "common.h"

bool err_check(int err_code) {

  if(err_code != DWM_OK) {
    return false;
  }

  return true;
}

void blink_led_thread(int pin_led, int loops, float seconds) {

    // Set as output and turn off the led.
    dwm_gpio_cfg_output(pin_led, true);

    int i;

    for(i = 0; i < loops; ++i) {

      dwm_gpio_value_set(pin_led, false);
      dwm_thread_delay(ONE_SECOND*seconds);

      dwm_gpio_value_set(pin_led, true);
      dwm_thread_delay(ONE_SECOND*seconds);
    }

}