#include "nvm.h"

bool check_nvm_validity(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  bool value = true;
  int i;

  for(i = 0; i < NVM_VALID_VARIABLE_SIZE && value; ++i) {
    value = (nvm[valid_NVM + i] == VALID_VALUE);
  }

  return value;

}

bool clean_memory(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  // Put zeros in all the positions of the NVM.
  set_zeros_nvm();

  // Make the NVM valid.
  int i;
  for(i = 0; i < NVM_VALID_VARIABLE_SIZE; ++i) {
    nvm[valid_NVM + i] = VALID_VALUE;
  }

  nvm[number_of_scanned_neighbors] = 0;
  nvm[tag_index] = 0;

  return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
}

uint8_t get_nvm_uint8_variable(nvm_memory_position mp) {

  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(!err_check(dwm_nvm_usr_data_get(nvm, &len))) {
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

    int i, j, k;

    neighbors.cnt = nvm[number_of_scanned_neighbors];

    // Invalid memory read.
    if(neighbors.cnt > NET_NUM_NODES || neighbors.cnt < 0) {
      neighbors.cnt = 0;
    }

    // An id of a node occupies 64 bits, and the NVM
    // positions are of 8 bits, so to load
    // an id, we need to load to positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt; i+=sizeof(uint64_t)/sizeof(uint8_t), ++j) {
      for(k = 1; k <= sizeof(uint64_t)/sizeof(uint8_t); ++k) {
        neighbors.node_ids[j] |= (uint64_t) nvm[neighbors_start_address + i + k - 1] << sizeof(uint8_t)*k;
      }

    }
  }

  return neighbors;
}

bool set_nvm_uint8_variable(nvm_memory_position mp, int value) {
  
  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(!err_check(dwm_nvm_usr_data_get(nvm, &len))) {
    nvm[mp] = value;
  } else {
    return false;
  }

  return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
}

void set_zeros_nvm(void) {

  uint8_t clean_nvm[DWM_NVM_USR_DATA_LEN_MAX];
  
  int i;
  for(i = 0; i < DWM_NVM_USR_DATA_LEN_MAX; ++i) {
    clean_nvm[i] = 0x00;
  }

  dwm_nvm_usr_data_set(clean_nvm, DWM_NVM_USR_DATA_LEN_MAX);

}

bool store_neighbors(rangin_neighbors neighbors) {

  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(!err_check(dwm_nvm_usr_data_get(nvm, &len))) {

    int i, j, k;
    uint64_t masc = 0XFF00000000000000;

    nvm[number_of_scanned_neighbors] = neighbors.cnt;

    // An id of a node occupies 64 bits, and the NVM
    // positions are of 8 bits, so to load
    // an id, we need to load to positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt; i+=sizeof(uint64_t)/sizeof(uint8_t), ++j) {
      for(k = 0; masc > 0; ++k) {
        nvm[neighbors_start_address + i + k] = (uint8_t)((neighbors.node_ids[j] & masc) >> sizeof(uint8_t));
        masc = masc >> sizeof(uint8_t);
      }
      masc = 0XFF00000000000000;
    }

    return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
  }

  return false;
}