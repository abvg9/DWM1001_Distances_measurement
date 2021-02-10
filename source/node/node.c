#include "node.h"

/*******************************
 * DEFAULT NODE CONFIGURATIONS *
 *******************************/
const dwm_cfg_common_t default_common_cfg = {DWM_UWB_MODE_ACTIVE, true, false, false, false};
const dwm_cfg_tag_t default_tag_cfg = {{}, true, false, true, DWM_MEAS_MODE_TWR};
const dwm_cfg_anchor_t default_anchor_cfg = {{}, false, false};

int binary_search_neighbor(uint16_t node_id, int left_limit, int right_limit) {

  if(neighbors.cnt == 0) {
    return -1;
  }

  int medium = 0;

  while(left_limit <= right_limit) {

    medium = left_limit + (right_limit - left_limit) / 2;

    if(neighbors.node_ids[medium] == node_id) {
      return medium;
    }

    if(neighbors.node_ids[medium] < node_id) {
      left_limit = medium + 1;
    } else {
      right_limit = medium - 1;
    }

  }

  return -1;

}

void binary_store_neighbor(uint16_t node_id) {

  if(neighbors.cnt == 0) {
    neighbors.node_ids[0] = node_id;
  } else if(neighbors.cnt < DWM_RANGING_ANCHOR_CNT_MAX
    && binary_search_neighbor(node_id, 0, neighbors.cnt-1) == -1) {

    int i;

    for(i = neighbors.cnt-1; (i >= 0) && (neighbors.node_ids[i] > node_id); i--) {
      neighbors.node_ids[i + 1] = neighbors.node_ids[i];
    }

    neighbors.node_ids[i + 1] = node_id;
  }

  neighbors.cnt++;

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

bool check_configuration(dwm_mode_t expected_mode, dwm_cfg_t cfg) {

  if(expected_mode != cfg.mode) {
    return false;
  }

  switch(expected_mode) {

    case DWM_MODE_TAG:

      if(default_tag_cfg.stnry_en != cfg.stnry_en) {
        return false;
      }

      if(default_tag_cfg.meas_mode != cfg.meas_mode) {
        return false;
      }

      if(default_tag_cfg.low_power_en != cfg.low_power_en) {
        return false;
      }

      if(default_tag_cfg.loc_engine_en != cfg.loc_engine_en) {
        return false;
      }

      break;
    
    case DWM_MODE_ANCHOR:

      if(default_anchor_cfg.initiator != cfg.initiator) {
        return false;
      }

      if(default_anchor_cfg.bridge != cfg.bridge) {
        return false;
      }

    break;   

  }

  // Compare common.
  if(cfg.common.ble_en != default_common_cfg.ble_en) {
    return false;
  }

  if(cfg.common.enc_en != default_common_cfg.enc_en) {
    return false;
  }

  if(cfg.common.fw_update_en != default_common_cfg.fw_update_en) {
    return false;
  }

  if(cfg.common.led_en != default_common_cfg.led_en) {
    return false;
  }

  if(cfg.common.uwb_mode != default_common_cfg.uwb_mode) {
    return false;
  }

  return true;
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
    case DWM_EVT_USR_DATA_READY:
      blink_led(LED_BLUE, 4, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
    case DWM_EVT_USR_DATA_SENT:
      blink_led(LED_BLUE, 5, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
    default:
      blink_led(LED_BLUE, 6, 1);
      blink_led(LED_GREEN, 1, 1);
    break;
  }

}

void dwm_anchor_scan_thread(uint32_t data) {

  // Initialize neighbors list.
  uint16_t* id = NULL;

  err_check(dwm_panid_get(id));
  binary_store_neighbor(*id);

  dwm_anchor_list_t anchors_list;
  anchors_list.cnt = 0;

  while(neighbors.cnt != NET_NUM_NODES) {

    if(err_check(dwm_anchor_list_get(&anchors_list))) {

      if(anchors_list.cnt > 0) {

        int i;
        for(i = 0; i < anchors_list.cnt; ++i) {
          binary_store_neighbor(anchors_list.v[i].node_id);
        }
      }
    }

  }

  // Si soy el primero, seré el tag.
  if(neighbors.node_ids[0] == *id) {
    
    // HAY QUE GUARDAR COSAS EN LA NVM PARA:
    // -1 CUANDO VUELVA AL DWM_USER_START NO VUELVA A SER ANCHOR
    // -2 GUARDAR LA LISTA DE VECINOS ENCONTRADOS.
    if(nvm_store_data(DWM_MODE_ANCHOR)) {
      set_node_as_tag();
    }

  }

}

void dwm_event_thread(uint32_t data) {

  /* Register event callback */
  dwm_evt_listener_register(DWM_EVT_LOC_READY | DWM_EVT_USR_DATA_READY | 
    DWM_EVT_USR_DATA_SENT | DWM_EVT_BH_INITIALIZED_CHANGED
    | DWM_EVT_UWBMAC_JOINED_CHANGED, NULL);

  dwm_evt_t evt;

  while (true) {

    if(err_check(dwm_evt_wait(&evt))) {
      dwm_event_callback(&evt);
    }
  }

}

bool set_node_as_anchor(void) {

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }
  
  if(!check_configuration(DWM_MODE_ANCHOR, cfg)) {

    dwm_cfg_anchor_t anchor_cfg = default_anchor_cfg;
    anchor_cfg.common = default_common_cfg;

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
  
  if(!check_configuration(DWM_MODE_TAG, cfg)) {

    // Update rate set to 1 second, stationary update rate set to 5 seconds.
    if(!err_check(dwm_upd_rate_set(10, 10))) {
      return false;
    }

    // Sensitivity for switching between stationary and normal update rate.
    if(!err_check(dwm_stnry_cfg_set(DWM_STNRY_SENSITIVITY_NORMAL))) {
      return false;
    }
 
    dwm_cfg_tag_t tag_cfg = default_tag_cfg;
    tag_cfg.common = default_common_cfg;

    if(!err_check(dwm_cfg_tag_set(&tag_cfg))) {
      return false;
    }

    dwm_reset();

  }

  // AQUI DEBERIAMOS AVISAR AL SIGUIENTE NODO DE QUE EL ES EL INITIATOR
  
  return true;
}



