#ifndef NODE
#define NODE

#include "common.h"
#include <stdio.h>

rangin_neighbors neighbors;

/*************
 * FUNCTIONS *
 *************/

int binary_search_neighbor(uint16_t node_id, int left_limit, int right_limit);

void binary_store_neighbor(uint16_t node_id);

void blink_led(int pin_led, int loops, float seconds);

bool check_configuration(dwm_mode_t expected_mode, dwm_cfg_t cfg);

dwm_pos_t create_position(int32_t x, int32_t y, int32_t z, uint8_t quality_factor);

void dwm_anchor_scan_thread(uint32_t data);

void dwm_event_callback(dwm_evt_t *p_evt);

void dwm_event_thread(uint32_t data);

bool set_node_as_anchor(void);

bool set_node_as_tag(void);

#endif //NODE