#ifndef COMMON
#define COMMON

#include "dwm.h"

/*******
 * NET *
 *******/
#define NET_NUM_NODES 2

/********
 * LEDS *
 ********/
#define RED_LED    	 DWM_GPIO_IDX_22
#define RED1_LED         DWM_GPIO_IDX_14
#define BLUE_LED   	 DWM_GPIO_IDX_31
#define GREEN_LED  	 DWM_GPIO_IDX_30
#define LED_ON(led_pin)  dwm_gpio_value_set(led_pin, true)
#define LED_OFF(led_pin) dwm_gpio_value_set(led_pin, false)

/***********
 * THREADS *
 ***********/
#define THREAD_PRIO       20
#define THREAD_STACK_SIZE (3 * 1024)

/* Relationship between seconds and system ticks */
#define ONE_SECOND 100

/************
 * NEIGHBORS *
 ************/

typedef struct {
  int cnt;
  uint64_t node_ids[DWM_RANGING_ANCHOR_CNT_MAX];
} rangin_neighbors;

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Check if a dwm api function return an error code or not.
 *
 * @param[in] err_code: Return value of a dwm pi function.
 *
 * @return bool
 * @retval true If the return value is not an error code.
 * @retval false If the return value is an error code.
 */
bool err_check(int err_code);

/**
 * @brief Makes a led blink.
 *
 * @note This function only works if called in the context of a thread.
 * @note If you look at the implementation of the function, a library function
 *       called dwm_thread_delay is called. This receives an integer and 
 *       indicates how many ticks we want to remain idle. For this reason,
 *       when you want to call the blink_led function, be careful with the seconds
 *       that are passed to it, since c, when casting from float to integer could
 *       round to 0 and we could not see the led blink.
 *
 * @param[in] pin_led: Led gpio pin.
 * @param[in] loops: Number of times the led will blink.
 * @param[in] seconds: Number of seconds that the led will be in the on or off state.
 *
 */
void blink_led_thread(int pin_led, int loops, float seconds);

#endif // COMMON