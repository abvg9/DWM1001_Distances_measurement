#include "node.h"

int binary_search_neighbor(uint16_t node_id, int left_limit, int right_limit) {

  if(neighbors.cnt == 0) {
    return -1;
  }

  int medium = 0;

  while(left_limit <= right_limit) {

    medium = left_limit + (right_limit - left_limit) / 2;

    if(neighbors.nodes[medium].id == node_id) {
      return medium;
    }

    if(neighbors.nodes[medium].id < node_id) {
      left_limit = medium + 1;
    } else {
      right_limit = medium - 1;
    }

  }

  return -1;

}

void binary_store_neighbor(uint16_t node_id) {

  if(neighbors.cnt < DWM_RANGING_ANCHOR_CNT_MAX
    && binary_search_neighbor(node_id, 0, neighbors.cnt-1) == -1) {

    node new_node = {node_id, 0};
    int i;

    for(i = neighbors.cnt-1; (i >= 0) && (neighbors.nodes[i].id > node_id); i--) {
      neighbors.nodes[i + 1] = neighbors.nodes[i];
    }

    neighbors.nodes[i + 1] = new_node;
    neighbors.cnt++;

  }

}

void blink_led(int pin_led, int loops, float seconds) {

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

dwm_pos_t create_position(int32_t x, int32_t y, int32_t z, uint8_t quality_factor) {

  dwm_pos_t position;
  position.x = x;
  position.y = y;
  position.z = z;
  position.qf = quality_factor;

  return position;
}

void dwm_event_callback(dwm_evt_t *p_evt) {

  switch (p_evt->header.id) {
    case DWM_EVT_LOC_READY:
      blink_led(LED_BLUE, 1, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
    case DWM_EVT_UWBMAC_JOINED_CHANGED:
      blink_led(LED_BLUE, 2, 1);
      blink_led(LED_GREEN, 1, 1);
    break; 
    case DWM_EVT_BH_INITIALIZED_CHANGED:
      blink_led(LED_BLUE, 3, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
    case DWM_EVT_UWB_SCAN_READY:
      blink_led(LED_BLUE, 4, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
    case DWM_EVT_USR_DATA_READY:
      blink_led(LED_BLUE, 5, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
    case DWM_EVT_USR_DATA_SENT:
      blink_led(LED_BLUE, 6, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
    default:
      blink_led(LED_BLUE, 7, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
  }

  // Send a broadcast message informing that the node is ready.
  uint8_t len, message[DWM_USR_DATA_LEN_MAX];
  len = DWM_USR_DATA_LEN_MAX;

  message[0] = 1;

  if(!err_check(dwm_usr_data_write(message, len, true))) {
  }

}

void dwm_anchor_scan_thread(uint32_t data) {

  // Initialize neighbors list.
  uint16_t* id = NULL;

  err_check(dwm_panid_get(id));

  node n = {(uint16_t)*id, 0};
  neighbors.nodes[0] = n;
  neighbors.cnt = 1;

  dwm_anchor_list_t anchors_list;
  anchors_list.cnt = 0;

  while(anchors_list.cnt != NET_NUM_NODES-1) {

    if(err_check(dwm_anchor_list_get(&anchors_list))) {

      if(anchors_list.cnt > 0) {

        int i;
        for(i = 0; i < anchors_list.cnt; ++i) {
          binary_store_neighbor(anchors_list.v[i].node_id);
        }
      }
    }

  }

}

void dwm_event_thread(uint32_t data) {

  /* Register event callback */
  dwm_evt_listener_register(DWM_EVT_LOC_READY | DWM_EVT_USR_DATA_READY | 
    DWM_EVT_USR_DATA_SENT | DWM_EVT_UWB_SCAN_READY | DWM_EVT_BH_INITIALIZED_CHANGED
    | DWM_EVT_UWBMAC_JOINED_CHANGED, NULL);

  dwm_evt_t evt;

  while (true) {

    if(err_check(dwm_evt_wait(&evt))) {
      dwm_event_callback(&evt);
    }
  }

}

bool err_check(int err_code) {

  if(err_code != DWM_OK) {
    return false;
  }

  return true;
}

bool set_node_as_anchor(void) {

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }
  
  if(cfg.mode != DWM_MODE_ANCHOR) {

    dwm_cfg_anchor_t anchor_cfg;

    anchor_cfg.initiator = true;
    anchor_cfg.bridge = false;
    anchor_cfg.common.ble_en = false;
    anchor_cfg.common.led_en = false;
    anchor_cfg.common.enc_en = false;
    anchor_cfg.common.fw_update_en = true;
    anchor_cfg.common.uwb_mode = DWM_UWB_MODE_ACTIVE;

    if(!err_check(dwm_cfg_anchor_set(&anchor_cfg))) {
      return false;
    }

     // Set position.
    dwm_pos_t pos_get;

    if(!err_check(dwm_pos_get(&pos_get))) {
      return false;
    }

    if(pos_get.x != 0 || pos_get.y != 0 || pos_get.z != 0 || pos_get.qf != 100) {

      dwm_pos_t pos_set = create_position(0, 0, 0, 100);
      if(!err_check(dwm_pos_set(&pos_set))) {
        return false;
      }
    }

    dwm_reset();

  }

  return true;
}

bool set_node_as_tag(void) {

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }
  
  if(cfg.mode != DWM_MODE_TAG) {

    // Update rate set to 1 second, stationary update rate set to 5 seconds.
    if(!err_check(dwm_upd_rate_set(10, 10))) {
      return false;
    }

    // Sensitivity for switching between stationary and normal update rate.
    if(!err_check(dwm_stnry_cfg_set(DWM_STNRY_SENSITIVITY_NORMAL))) {
      return false;
    }

    //dwm_uwb_cfg_t uwb_cfg;
    //uwb_cfg.pg_delay = PG_DELAY;
    //uwb_cfg.tx_power = TX_POWER;

    // Set uwb configuration.
    //if(!err_check(dwm_uwb_cfg_set(&uwb_cfg))) {
      //return false;
    //}
  
    dwm_cfg_tag_t tag_cfg;

    tag_cfg.stnry_en = true;
    tag_cfg.meas_mode = DWM_MEAS_MODE_TWR;
    tag_cfg.low_power_en = false;
    tag_cfg.loc_engine_en = true;
    tag_cfg.common.ble_en = false;
    tag_cfg.common.enc_en = false;
    tag_cfg.common.fw_update_en = false;
    tag_cfg.common.led_en = false;
    tag_cfg.common.uwb_mode = DWM_UWB_MODE_ACTIVE;

    if(!err_check(dwm_cfg_tag_set(&tag_cfg))) {
      return false;
    }

    dwm_reset();

  }
  
  return true;
}



