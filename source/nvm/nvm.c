#include "nvm.h"

bool check_nvm_boolean_variable(nvm_memory_position mp, uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  bool value = true;
  int i;

  for(i = 0; i < NVM_VARIABLE_SIZE && value; ++i) {
    value = (nvm[mp + i] == 0);
  }

  return value;

}

bool clean_memory(uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX]) {

  uint8_t clean_nvm[DWM_NVM_USR_DATA_LEN_MAX] = {0};

  int i;

  for(i = 0; i < NVM_VARIABLE_SIZE; ++i) {
    clean_nvm[valid_NVM + i] = NVM_TRUE;
  }

  for(i = 0; i < NVM_VARIABLE_SIZE; ++i) {
    clean_nvm[mode + i] = DWM_MODE_TAG;
  }

  clean_nvm[initiator] = NVM_FALSE;
  clean_nvm[number_of_scanned_neighbors] = 0;
  clean_nvm[tag_index] = 0;
  clean_nvm[initiator_index] = 1;

  buf = clean_nvm;

  return dwm_nvm_usr_data_set(clean_nvm, DWM_NVM_USR_DATA_LEN_MAX);
}

rangin_neighbors load_neighbors(void) {

  uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  rangin_neighbors neighbors;
  neighbors.cnt = 0;

  if(!err_check(dwm_nvm_usr_data_get(buf, &len))) {

    int i;
    int j;

    neighbors.cnt = buf[number_of_scanned_neighbors];

    // An id of a node occupies 16 bits, and the NVM
    // positions are of 8 bits, so to load
    // an id, we need to load to positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt; i+=2, ++j) {
      neighbors.node_ids[j] = 
        ((uint16_t) buf[neighbors_start_address + i + 1] << 8) | buf[neighbors_start_address + i];
    }
  }

  return neighbors;
}

bool set_nvm_boolean_variable(nvm_memory_position mp, uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX], bool value) {
 
  int val = value ? NVM_TRUE : NVM_FALSE;

  int i;
  for(i = 0; i < NVM_VARIABLE_SIZE && value; ++i) {
    nvm[mp + i] = val;
  }

  return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
}

bool store_neighbors(rangin_neighbors neighbors) {

  uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(!err_check(dwm_nvm_usr_data_get(buf, &len))) {

    int i;
    int j;

    buf[number_of_scanned_neighbors] = neighbors.cnt;

    // An id of a node occupies 16 bits, and the NVM
    // positions are of 8 bits, so to load
    // an id, we need to load to positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt; i+=2, ++j) {
      buf[neighbors_start_address + i] = (uint8_t)((neighbors.node_ids[j] & 0XFF00) >> 8);
      buf[neighbors_start_address + i + 1] = (uint8_t)(neighbors.node_ids[j] & 0X00FF);
    }

    return err_check(dwm_nvm_usr_data_set(buf, DWM_NVM_USR_DATA_LEN_MAX));
  }

  return false;
}