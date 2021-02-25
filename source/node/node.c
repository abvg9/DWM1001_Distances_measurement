#include "node.h"

/*******************************
 * DEFAULT NODE CONFIGURATIONS *
 *******************************/
const dwm_cfg_common_t default_common_cfg = {DWM_UWB_MODE_ACTIVE, false, false, false, false};
const dwm_cfg_tag_t default_tag_cfg = {{}, false, false, false, DWM_MEAS_MODE_TWR};
const dwm_cfg_anchor_t default_anchor_cfg = {{}, false, false};

extern rangin_neighbors neighbors;

void scan_neighbors_thread(uint32_t data) {

  dwm_anchor_list_t anchors_list;

  do {

    if(err_check(dwm_anchor_list_get(&anchors_list))) {

      int i;
      for(i = 0; i < anchors_list.cnt; ++i) {
        store_neighbor(anchors_list.v[i].node_id);
      }

    }

  } while(neighbors.cnt != NET_NUM_NODES-1);

  uint64_t node_id;
  dwm_node_id_get(&node_id);

  // Get the last 16 bits, the node id.
  node_id &= 0xffff;

  store_neighbor(node_id);
  set_nvm_uint8_variable(my_neighbor_index, is_there_neighbor(node_id));

  store_neighbors(neighbors);

  dwm_reset();

}

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

bool set_node_as_anchor(bool is_initiator) {

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }

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

  if(!err_check(dwm_gpio_cfg_output(red1_led, true))) {
    return false;
  }

  if(!err_check(dwm_gpio_value_set(red1_led, !is_initiator))) {
      return false;
  }

  return true;
}

bool set_node_as_tag(void) {

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }
  
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

dwm_mode_t select_node_mode(uint8_t index) {

  if(index > DWM_RANGING_ANCHOR_CNT_MAX || index == get_nvm_uint8_variable(initiator_index)) {

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

void get_anchor_distances_thread(uint32_t data) {

  dwm_loc_data_t loc;

  do {
    dwm_loc_get(&loc);
  } while(loc.anchors.dist.cnt != NET_NUM_NODES-1);


  /* TODO */
  // ENVIAMOS DISTANCIAS A LA CONTROLADORA.
  int i;
  for(i = 0; i < loc.anchors.dist.cnt; ++i) {
    printf("Node id: 0x%04X\n", (unsigned int)loc.anchors.dist.addr[i]);
    printf("Distance: %lu mm\n", loc.anchors.dist.dist[i]);
  }

  update_state();
}

void update_state(void) {

  uint8_t i_index = get_nvm_uint8_variable(initiator_index) + 1;
  uint8_t t_index = get_nvm_uint8_variable(tag_index) + 1;

  set_nvm_uint8_variable(initiator_index, i_index%neighbors.cnt);
  set_nvm_uint8_variable(tag_index, t_index%neighbors.cnt);

  dwm_reset();
}

void wait_tag_thread(uint32_t data) {

  dwm_anchor_list_t anchors_list;

  uint16_t tag_id = neighbors.node_ids[get_nvm_uint8_variable(tag_index)];
  //uint16_t my_id = neighbors.node_ids[get_nvm_uint8_variable(my_neighbor_index)];

  bool tag_no_ended = true;

  int i = -1;
  do {
    if(err_check(dwm_anchor_list_get(&anchors_list))) {
      i++;
      tag_no_ended = (anchors_list.v[i].node_id != tag_id);
    }
  } while(i < anchors_list.cnt && tag_no_ended);

  update_state();
}