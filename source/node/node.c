#include "node.h"

/*******************************
 * DEFAULT NODE CONFIGURATIONS *
 *******************************/
const dwm_cfg_common_t default_common_cfg = {DWM_UWB_MODE_ACTIVE, false, false, false, false};
const dwm_cfg_tag_t default_tag_cfg = {{}, false, false, false, DWM_MEAS_MODE_TWR};
const dwm_cfg_anchor_t default_anchor_cfg = {{}, false, true};

extern rangin_neighbors neighbors;

void anchor_scan_thread(uint32_t data) {

  dwm_anchor_list_t anchors_list;
  anchors_list.cnt = 0;

  while(neighbors.cnt != NET_NUM_NODES -1) {

    if(err_check(dwm_anchor_list_get(&anchors_list))) {

      if(anchors_list.cnt > 0) {

        blink_led_thread(BLUE_LED, anchors_list.cnt, 1);
        blink_led_thread(GREEN_LED, anchors_list.cnt, 1);

        int i;
        for(i = 0; i < anchors_list.cnt; ++i) {
          store_neighbor(anchors_list.v[i].node_id);
        }
      }else {
        blink_led_thread(RED1_LED, 1, 1);
      }
    }

  }

  uint64_t node_id;
  dwm_node_id_get(&node_id);

  // Get the last 16 bits, the node id.
  node_id &= 0X000000000000FFFF;

  store_neighbor(node_id);

  store_neighbors(neighbors);
  dwm_reset();

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

bool cmp_neighbors_lists(rangin_neighbors loaded_neighbors) {

  if(loaded_neighbors.cnt != neighbors.cnt) {
    return false;
  }

  int i;
  bool equal = true;

  for(i = 0; i < loaded_neighbors.cnt && equal; ++i) {
    equal = (loaded_neighbors.node_ids[i] == neighbors.node_ids[i]);
  }

  return equal;
}

dwm_pos_t create_position(int32_t x, int32_t y, int32_t z, uint8_t quality_factor) {

  dwm_pos_t position;
  position.x = x;
  position.y = y;
  position.z = z;
  position.qf = quality_factor;

  return position;
}

void message_event_callback(dwm_evt_t *p_evt) {

  switch (p_evt->header.id) {

    case DWM_EVT_USR_DATA_READY:
      blink_led_thread(BLUE_LED, 1, 3);
      blink_led_thread(GREEN_LED, 1, 3);
      // Si salta este sería la clave.
      // p_evt->usr_data[i]
      // dwm_reset();
    break;

    case DWM_EVT_USR_DATA_SENT:
      blink_led_thread(BLUE_LED, 2, 3);
      blink_led_thread(GREEN_LED, 1, 3);
    break;

    default:
      blink_led_thread(BLUE_LED, 3, 3);
      blink_led_thread(GREEN_LED, 1, 3);
    break;
  }

}

void message_handler_thread(uint32_t data) {

  /* Register event callback */
  dwm_evt_listener_register(DWM_EVT_USR_DATA_READY | DWM_EVT_USR_DATA_SENT, NULL);

  dwm_evt_t evt;

  while (true) {

    if(err_check(dwm_evt_wait(&evt))) {
      message_event_callback(&evt);
    }
  }

}

int is_there_neighbor(uint16_t node_id) {

  if(neighbors.cnt == 0) {
    return -1;
  }

  int medium = 0, left_limit = 0, right_limit = 0;

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

    // To apply changes we need to reset the board.
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
    /*
    if(!err_check(dwm_upd_rate_set(10, 10))) {
      return false;
    }

    // Sensitivity for switching between stationary and normal update rate.
    if(!err_check(dwm_stnry_cfg_set(DWM_STNRY_SENSITIVITY_NORMAL))) {
      return false;
    }
    */
 
    dwm_cfg_tag_t tag_cfg = default_tag_cfg;
    tag_cfg.common = default_common_cfg;

    if(!err_check(dwm_cfg_tag_set(&tag_cfg))) {
      return false;
    }

    // To apply changes we need to reset the board.
    dwm_reset();

  }
  
  return true;
}

dwm_mode_t set_node_mode(bool first_run) {

  uint64_t node_id;
  dwm_node_id_get(&node_id);

  int index = is_there_neighbor(node_id);

  if(index == get_nvm_uint8_variable(tag_index) && !first_run) {

    if(set_node_as_tag()) {
      return DWM_MODE_TAG;
    }
    
  } else {

    if(set_node_as_anchor()) {
      return DWM_MODE_ANCHOR;
    }
  }

  return -1;

}

void store_neighbor(uint64_t node_id) {

  if(neighbors.cnt == 0) {

    neighbors.node_ids[0] = node_id;
    neighbors.cnt++;

  } else if(neighbors.cnt < DWM_RANGING_ANCHOR_CNT_MAX && 
            is_there_neighbor(node_id) == -1) {

    int i;

    for(i = neighbors.cnt-1; (i >= 0) && (neighbors.node_ids[i] > node_id); i--) {
      neighbors.node_ids[i + 1] = neighbors.node_ids[i];
    }

    neighbors.node_ids[i + 1] = node_id;
    neighbors.cnt++;
  }

}

void tag_scan_thread(uint32_t data) {

  dwm_loc_data_t loc;
  dwm_loc_get(&loc);

  while(loc.anchors.dist.cnt != NET_NUM_NODES-1) {
    int rc = dwm_loc_get(&loc);
  }

  int patata = 0;
  patata++;
  // AHORA SOLO QUEDA ENVIAR LAS DISTANCIAS Y REINICIARTE.
  //dwm_reset();

}