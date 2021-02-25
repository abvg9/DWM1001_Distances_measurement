#include "common.h"

bool err_check(const int err_code) {
  return err_code == DWM_OK;
}

bool blink_led(uint32_t data) {

  blink_led_struct* led_information = (blink_led_struct*)data;

  if(!err_check(dwm_gpio_cfg_output(led_information->pin, true))) {
    return false;
  }

  int i;
  bool endless_loop = led_information->loops < 0;

  for(i = 0; i < led_information->loops || endless_loop; ++i) {

    if(!err_check(dwm_gpio_value_set(led_information->pin, false))) {
      return false;
    }

    dwm_thread_delay(ONE_SECOND*led_information->seconds);

    if(!err_check(dwm_gpio_value_set(led_information->pin, true))) {
      return false;
    }

    dwm_thread_delay(ONE_SECOND*led_information->seconds);

  }

  return true;
}