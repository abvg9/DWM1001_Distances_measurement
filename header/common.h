#ifndef COMMON
#define COMMON

#include "dwm.h"

/* Net */
#define NET_NUM_NODES 2

/* Leds */
#define LED_RED    	 DWM_GPIO_IDX_22
#define LED_RED1    	 DWM_GPIO_IDX_14
#define LED_BLUE   	 DWM_GPIO_IDX_31
#define LED_GREEN  	 DWM_GPIO_IDX_30
#define LED_ON(led_pin)  dwm_gpio_value_set(led_pin, true)
#define LED_OFF(led_pin) dwm_gpio_value_set(led_pin, false)

/* Thread priority */
#ifndef THREAD_PRIO
#define THREAD_PRIO	20
#endif /* THREAD_APP_PRIO */

/* Thread stack size */
#ifndef THREAD_STACK_SIZE
#define THREAD_STACK_SIZE	(3 * 1024)
#endif /* THREAD_APP_STACK_SIZE */

/* Relationship between seconds and system ticks */
#define ONE_SECOND 100

/* Neigbors */
typedef struct {
  int cnt;
  uint16_t node_ids[DWM_RANGING_ANCHOR_CNT_MAX];
} rangin_neigbors;

rangin_neigbors neighbors;

#endif // COMMON