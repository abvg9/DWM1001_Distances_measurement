#include "node.h"

/*******************************
 * DEFAULT NODE CONFIGURATIONS *
 *******************************/
const dwm_cfg_common_t default_common_cfg = {DWM_UWB_MODE_ACTIVE, false, false, false, false};
const dwm_cfg_tag_t default_tag_cfg = {{}, false, false, false, DWM_MEAS_MODE_TWR};
const dwm_cfg_anchor_t default_anchor_cfg = {{}, false, false};

extern rangin_neighbors neighbors;

bool check_configuration(dwm_mode_t expected_mode, dwm_cfg_t cfg) {

  if(expected_mode != cfg.mode) {
    return false;
  }

  if(expected_mode == DWM_MODE_TAG) {

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

  } else {

    if(default_anchor_cfg.bridge != cfg.bridge) {
      return false;
    }

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

bool send_message(message_type m) {
  dwm_pos_t message = {m , 0, 0, 0};
  return err_check(dwm_pos_set(&message));
}

bool cmp_positions(dwm_pos_t a, dwm_pos_t b) {
  
  if(a.x != b.x) {
    return false;
  }

  if(a.y != b.y) {
    return false;
  }

  if(a.z != b.z) {
    return false;
  }

  if(a.qf != b.qf) {
    return false;
  }

  return true;
}

dwm_pos_t create_position(int32_t x, int32_t y, int32_t z, uint8_t qf) {

  dwm_pos_t pos = {x, y, z, qf};

  return pos;
}

void get_anchor_distances_thread(uint32_t data) {

  dwm_loc_data_t loc;
  blink_led_struct no_got_distances_led = {red1_led, 1, 0.1f};

  do {
    blink_led((uint32_t)&no_got_distances_led);
    dwm_loc_get(&loc);
  } while(loc.anchors.dist.cnt != NET_NUM_NODES-1);

  int i;
  for(i = 0; i < loc.anchors.dist.cnt; ++i) {
    printf("Node id: 0x%04X\n", (unsigned int)loc.anchors.dist.addr[i]);
    printf("Distance: %lu mm\n", loc.anchors.dist.dist[i]);
  }

  blink_led_struct got_distances_led = {blue_led, 1, 0.1f};
  blink_led((uint32_t)&got_distances_led);

  update_state();
}

bool is_anchor_scan_finished(dwm_anchor_list_t anchors_list) {

  int i = 0;
  bool finished = true;

  if(anchors_list.cnt != NET_NUM_NODES-1) {
    return false;
  }

  while(i < anchors_list.cnt && finished) {
    finished = anchors_list.v[i].x == net_nodes_finded;
    i++;
  }

  return finished;
}

int is_there_neighbor(uint16_t node_id) {

  if(neighbors.cnt == 0) {
    return -1;
  }

  int medium = 0, left_limit = 0, right_limit = neighbors.cnt-1;

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

void scan_neighbors_thread(uint32_t data) {

  dwm_anchor_list_t anchors_list;
  blink_led_struct no_anchors_found_led = {red1_led, 1, 0.1f};
  blink_led_struct anchors_found_led = {green_led, 0, 0.1f};
  int i;

  do {

    if(err_check(dwm_anchor_list_get(&anchors_list))) {

      if(anchors_list.cnt > 0) {

        anchors_found_led.loops = anchors_list.cnt;
        blink_led((uint32_t)&anchors_found_led);
        blink_led((uint32_t)&no_anchors_found_led);
 
        for(i = 0; i < anchors_list.cnt; ++i) {
          store_neighbor(anchors_list.v[i].node_id);
        }

      } else {
        blink_led((uint32_t)&no_anchors_found_led);
      }

    }

    if(neighbors.cnt != NET_NUM_NODES-1) {
      send_message(net_nodes_finded);
    }

  } while(!is_anchor_scan_finished(anchors_list));

  uint64_t node_id;
  dwm_node_id_get(&node_id);

  // Get the last 16 bits, the node id.
  node_id &= 0xffff;

  store_neighbor(node_id);
  set_nvm_uint8_variable(my_neighbor_index, is_there_neighbor(node_id));

  store_neighbors(neighbors);

  dwm_reset();

}

dwm_mode_t select_node_mode(uint8_t index) {

  if(index >= DWM_RANGING_ANCHOR_CNT_MAX || index == get_nvm_uint8_variable(initiator_index)) {

    if(set_node_as_anchor(true)) {
      return DWM_MODE_ANCHOR;
    }

  } else if(index == get_nvm_uint8_variable(tag_index)) {

    if(set_node_as_tag()) {
      return DWM_MODE_TAG;
    }

  } else {

    if(set_node_as_anchor(false)) {
      return DWM_MODE_ANCHOR;
    }

  }

  return -1;

}

bool set_node_as_anchor(bool is_initiator) {

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }

  send_message(clean_message_buffer);

  if(!check_configuration(DWM_MODE_ANCHOR, cfg) || cfg.initiator != is_initiator) {

    dwm_cfg_anchor_t anchor_cfg = default_anchor_cfg;
    anchor_cfg.common = default_common_cfg;
    anchor_cfg.initiator = is_initiator;

    if(!err_check(dwm_cfg_anchor_set(&anchor_cfg))) {
      return false;
    }

    // To apply changes we need to reset the board.
    dwm_reset();

  }

  if(!err_check(dwm_gpio_cfg_output(blue_led, true))) {
    return false;
  }

  if(!err_check(dwm_gpio_value_set(blue_led, false))) {
    return false;
  }

  return true;
}

bool set_node_as_tag(void) {

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }
  
  set_nvm_uint8_variable(was_a_tag_in_last_state, true);

  if(!check_configuration(DWM_MODE_TAG, cfg)) {

    dwm_cfg_tag_t tag_cfg = default_tag_cfg;
    tag_cfg.common = default_common_cfg;

    if(!err_check(dwm_cfg_tag_set(&tag_cfg))) {
      return false;
    }

    // To apply changes we need to reset the board.
    dwm_reset();

  }

  if(!err_check(dwm_gpio_cfg_output(green_led, true))) {
    return false;
  }

  if(!err_check(dwm_gpio_value_set(green_led, false))) {
    return false;
  }
  
  return true;
}

bool set_position(dwm_pos_t pos) {
  return err_check(dwm_pos_set(&pos));
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

void update_state(void) {

  uint8_t i_index = (get_nvm_uint8_variable(initiator_index) + 1) % neighbors.cnt;
  uint8_t t_index = (get_nvm_uint8_variable(tag_index) + 1) % neighbors.cnt;

  set_nvm_uint8_variable(initiator_index, i_index);
  set_nvm_uint8_variable(tag_index, t_index);

  dwm_reset();
}

void wait_tag_thread(uint32_t data) {

  dwm_anchor_list_t anchors_list;
  uint16_t tag_id = neighbors.node_ids[get_nvm_uint8_variable(tag_index)];
  bool tag_ended = false;
  blink_led_struct no_tag_founded_led = {red1_led, 1, 0.1f};
  blink_led_struct tag_founded_led = {green_led, 1, 0.1f};
  int i;

  // If the node was a tag in the last state, it must wait 5 seconds.
  if(get_nvm_uint8_variable(was_a_tag_in_last_state)) {
    set_nvm_uint8_variable(was_a_tag_in_last_state, false);
    send_message(tag_got_distances);
  }

  // Wait until tag get distances.
  do {
    if(err_check(dwm_anchor_list_get(&anchors_list))) {

      for(i = 0; i < anchors_list.cnt && !tag_ended; ++i) {

        tag_ended = (anchors_list.v[i].node_id == tag_id) && 
                    (anchors_list.v[i].x == tag_got_distances);
        
      }

      if(tag_no_ended) {
        blink_led((uint32_t)&no_tag_founded_led);
      } else {
        blink_led((uint32_t)&tag_founded_led);
      }
   
    }
  } while(!tag_ended);


  update_state();
}