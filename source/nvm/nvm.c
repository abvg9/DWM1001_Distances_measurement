#include "nvm.h"

bool check_nvm_validity(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  bool value = true;
  int i;

  for(i = 0; i < NVM_VALID_VARIABLE_SIZE && value; ++i) {
    value = (nvm[valid_nvm + i] == VALID_VALUE);
  }

  return value;

}

bool clean_memory(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  // Put zeros in all the positions of the nvm.
  flush_nvm(nvm);

  // Make the nvm valid.
  int i;
  for(i = 0; i < NVM_VALID_VARIABLE_SIZE; ++i) {
    nvm[valid_nvm + i] = VALID_VALUE;
  }

  nvm[initiator_index] = 1;
  nvm[number_of_scanned_neighbors] = 0;
  nvm[tag_index] = 0;
  nvm[my_neighbor_index] = 0xFF;

  return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
}

bool flush_nvm(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  int i;
  for(i = 0; i < DWM_NVM_USR_DATA_LEN_MAX; ++i) {
    nvm[i] = 0x00;
  }

  return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));

}

uint8_t get_nvm_uint8_variable(nvm_memory_position mp) {

  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(err_check(dwm_nvm_usr_data_get(nvm, &len))) {
    return nvm[mp];
  }

  return -1;
}

rangin_neighbors load_neighbors(void) {

  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  rangin_neighbors neighbors;
  neighbors.cnt = 0;

  if(err_check(dwm_nvm_usr_data_get(nvm, &len))) {

    int i, j;

    neighbors.cnt = nvm[number_of_scanned_neighbors];

    // Invalid memory read.
    if(neighbors.cnt > NET_NUM_NODES || neighbors.cnt < 0) {
      neighbors.cnt = 0;
    }

    // An id of a node occupies 16 bits, and the nvm
    // positions are of 8 bits, so to load
    // an id, we need to load two positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt*2; i+=2, ++j) {
        neighbors.node_ids[j] = 
          ((uint64_t) nvm[neighbors_start_address + i] << 8) | nvm[neighbors_start_address + i + 1];
    }
  }

  return neighbors;
}

bool set_nvm_uint8_variable(nvm_memory_position mp, uint8_t value) {
  
  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(err_check(dwm_nvm_usr_data_get(nvm, &len))) {
    nvm[mp] = value;
  } else {
    return false;
  }

  return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
}

bool store_neighbors(rangin_neighbors neighbors) {

  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(err_check(dwm_nvm_usr_data_get(nvm, &len))) {

    int i, j;

    nvm[number_of_scanned_neighbors] = neighbors.cnt;

    // An id of a node occupies 16 bits, and the nvm
    // positions are of 8 bits, so to load
    // an id, we need to load two positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt*2; i+=2, ++j) {
      nvm[neighbors_start_address + i] = (uint8_t)((neighbors.node_ids[j] & 0XFF00) >> 8);
      nvm[neighbors_start_address + i + 1] = (uint8_t)(neighbors.node_ids[j] & 0X00FF);
    }

    return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
  }

  return false;
}