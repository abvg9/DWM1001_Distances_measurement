#ifndef COMMON
#define COMMON

#include "dwm.h"
#include <stdio.h>

/*******
 * NET *
 *******/
#define NET_NUM_NODES 3

/********
 * LEDS *
 ********/
typedef enum {
  blue_led = DWM_GPIO_IDX_31,
  green_led = DWM_GPIO_IDX_30,
  red_led = DWM_GPIO_IDX_22,
  red1_led = DWM_GPIO_IDX_14,
} led_pin;

/***********
 * THREADS *
 ***********/
#define THREAD_PRIO       20
#define THREAD_STACK_SIZE 3*1024
#define ONE_SECOND 100 // Relationship between seconds and system ticks.

/***********
 * STRUCTS *
 ***********/

typedef struct {
  int cnt;
  uint64_t node_ids[DWM_RANGING_ANCHOR_CNT_MAX];
} rangin_neighbors;

typedef struct {
  led_pin pin;
  int loops;
  float seconds;
} blink_led_struct;

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Makes a led blink.
 *
 * @note This function only works if it is called in the context of a thread.
 * @note If you look at the implementation of the function, a library function
 *       called dwm_thread_delay is called. This receives an integer and 
 *       indicates how many ticks we want to remain idle. For this reason,
 *       when you want to call the blink_led function, be careful with the seconds
 *       that are passed to it, since c, when casting from float to integer could
 *       round to 0 and you could not see the led blink.
 *
 * @param[in] data: This contains the pointer to the next variables.
 * @param[in] pin_led: Led gpio pin.
 * @param[in] loops: Number of times the led will blink. If loops is a negative value
 *                   this loop will be endless.
 * @param[in] seconds: Number of seconds that the led will be in the on or off state.
 *
 * @return bool
 * @retval true If the led blink.
 * @retval false If dwm_gpio_value_set return an error code.
 */
bool blink_led(uint32_t data);

/**
 * @brief Check if a dwm api function return an error code or not.
 *
 * @param[in] err_code: Return value of a dwm function.
 *
 * @return bool
 * @retval true If the return value is not an error code.
 * @retval false If the return value is an error code.
 */
bool err_check(const int err_code);

#endif // COMMON