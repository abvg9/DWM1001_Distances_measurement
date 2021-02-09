#ifndef NODE
#define NODE

#include "dwm.h"
#include <stdio.h>

/* Structs */
typedef struct {
  uint16_t id;
  uint32_t distance;
} node;

typedef struct {
  int cnt;
  node nodes[DWM_RANGING_ANCHOR_CNT_MAX];
} rangin_neigbors;

/* VARIABLES */
rangin_neigbors neighbors;
#define NET_NUM_NODES 2 // Without counting this node.
#define PG_DELAY 197
#define TX_POWER 0xD0252525

/* I2C */
#define LIS2DX_SLAVE_ADDR       0x19   /* I2C slave address */

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

/* Variables */
dwm_distance_t neighbours;

/* Functions */

int binary_search_neighbor(uint16_t node_id, int left_limit, int right_limit);

void binary_store_neighbor(uint16_t node_id);

void blink_led(int pin_led, int loops, float seconds);

dwm_pos_t create_position(int32_t x, int32_t y, int32_t z, uint8_t quality_factor);

void dwm_anchor_scan_thread(uint32_t data);

void dwm_event_callback(dwm_evt_t *p_evt);

void dwm_event_thread(uint32_t data);

bool err_check(int err_code);

bool set_node_as_anchor(void);

bool set_node_as_tag(void);

#endif //NODE