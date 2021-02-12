#include "nvm.h"

void llena_de_unos(void) {

  uint8_t clean_nvm[DWM_NVM_USR_DATA_LEN_MAX];
  
  int i;
  for(i = 0; i < DWM_NVM_USR_DATA_LEN_MAX; ++i) {
    clean_nvm[i] = 0xFF;
  }

  dwm_nvm_usr_data_set(clean_nvm, DWM_NVM_USR_DATA_LEN_MAX);

}

bool check_nvm_validity(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  bool value = true;
  int i;

  for(i = 0; i < NVM_VALID_VARIABLE_SIZE && value; ++i) {
    value = (nvm[valid_NVM + i] == VALID_VALUE);
  }

  return value;

}

bool clean_memory(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  // Put zeros in all the positions.
  int i;
  for(i = 0; i < DWM_NVM_USR_DATA_LEN_MAX; ++i) {
    nvm[i] = 0x00;
  }

  // Make the NVM valid.
  for(i = 0; i < NVM_VALID_VARIABLE_SIZE; ++i) {
    nvm[valid_NVM + i] = true;
  }

  nvm[number_of_scanned_neighbors] = 0;
  nvm[tag_index] = 0;
  nvm[initiator_index] = 1;

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

    int i;
    int j;

    neighbors.cnt = nvm[number_of_scanned_neighbors];

    // An id of a node occupies 16 bits, and the NVM
    // positions are of 8 bits, so to load
    // an id, we need to load to positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt; i+=2, ++j) {
      neighbors.node_ids[j] = 
        ((uint16_t) nvm[neighbors_start_address + i + 1] << 8) | nvm[neighbors_start_address + i];
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

bool store_neighbors(rangin_neighbors neighbors) {

  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(!err_check(dwm_nvm_usr_data_get(nvm, &len))) {

    int i;
    int j;

    nvm[number_of_scanned_neighbors] = neighbors.cnt;

    // An id of a node occupies 16 bits, and the NVM
    // positions are of 8 bits, so to load
    // an id, we need to load to positions of the nvm.
    for(i = 0, j = 0; i < neighbors.cnt; i+=2, ++j) {
      nvm[neighbors_start_address + i] = (uint8_t)((neighbors.node_ids[j] & 0XFF00) >> 8);
      nvm[neighbors_start_address + i + 1] = (uint8_t)(neighbors.node_ids[j] & 0X00FF);
    }

    return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
  }

  return false;
}