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
#define LED_RED    	 DWM_GPIO_IDX_22
#define LED_RED1    	 DWM_GPIO_IDX_14
#define LED_BLUE   	 DWM_GPIO_IDX_31
#define LED_GREEN  	 DWM_GPIO_IDX_30
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
  uint16_t node_ids[DWM_RANGING_ANCHOR_CNT_MAX];
} rangin_neighbors;

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Check if a dwm api function return a error code or not.
 *
 * @param[in] err_code: Return value of a dwm pi function.
 *
 * @return bool
 * @retval true If the return value is not an error code.
 * @retval false If the return value is an error code.
 */
bool err_check(int err_code);

#endif // COMMON