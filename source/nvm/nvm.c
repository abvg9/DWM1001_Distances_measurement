#include "nvm.h"

bool first_board_start(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  bool is_the_first_start = true;
  int i;

  for(i = 0; i < DWM_NVM_USR_DATA_LEN_MAX && is_the_first_start; ++i) {
    is_the_first_start *= (nvm[i] == 0);
  }

  if(!is_the_first_start) {
    uint8_t new_nvm[DWM_NVM_USR_DATA_LEN_MAX] = {0};
    dwm_nvm_usr_data_set(new_nvm, DWM_NVM_USR_DATA_LEN_MAX);
  }

  return is_the_first_start;

}

bool nvm_load_data(void) {

  uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;
  
  if(!err_check(dwm_nvm_usr_data_get(buf, &len))) {
    return false;
  }

  dwm_cfg_t cfg;

  if(!err_check(dwm_cfg_get(&cfg))) {
    return false;
  }

  // Significa que es el primer inicio de la placa y debemos
  if(first_board_start(buf)) {

  }

  if(nvm_is_valid(buf)) {

    if(buf[node_type] != cfg.mode) {

      int i;
      int j;
      // Una direccion es de 16 bits, y la nvm esta compusta por fragmentos
      // de 8 bits.
      for(i = 0, j = 0; i < NET_NUM_NODES*2; i+=2, ++j) {
        neighbors.node_ids[j] = 
          ((uint16_t) buf[first_neighbor_id_1 + i + 1] << 8) | buf[first_neighbor_id_1 + i];
      }

    }

  } else {
    return nvm_validate(buf);
  }

  return true;
}

bool nvm_store_data(dwm_mode_t node_mode) {

  uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX];
  buf[node_type] = node_mode;

  int i;
  int j;
  // Una direccion es de 16 bits, y la nvm esta compusta por fragmentos
  // de 8 bits.
  for(i = 0, j = 0; i < NET_NUM_NODES*2; i+=2, ++j) {
    buf[first_neighbor_id_1 + i] = (uint8_t)((neighbors.node_ids[j] & 0XFF00) >> 8);
    buf[first_neighbor_id_1 + i + 1] = (uint8_t)(neighbors.node_ids[j] & 0X00FF);
  }

  return err_check(dwm_nvm_usr_data_set(buf, DWM_NVM_USR_DATA_LEN_MAX));
}

bool nvm_validate(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {
  
  int i;
  for(i = 0; i < NVM_VALIDATION_CNT; ++i) {
    nvm[nvm_validation_number + i] = NVM_VALIDATION_NUMBER;
  }
  
  return err_check(dwm_nvm_usr_data_set(nvm, DWM_NVM_USR_DATA_LEN_MAX));
}

bool nvm_is_valid(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]) {

  bool nvm_is_valid = true;
  int i;
  for(i = 0; i < NVM_VALIDATION_CNT && nvm_is_valid; ++i) {
    nvm_is_valid *= (nvm[nvm_validation_number + i] == NVM_VALIDATION_NUMBER);
  }

  return nvm_is_valid;
}