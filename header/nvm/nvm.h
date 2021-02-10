#ifndef NVM
#define NVM

#include "common.h"

/* Definitions */
#define NVM_VALIDATION_NUMBER 0XFF
#define NVM_VALIDATION_CNT 3

/* Enums */
typedef enum {
  node_type = 0,
  nvm_validation_number = 1,
  first_neighbor_id_1 = 4
} nvm_memory_position;

/* Functions */

bool first_board_start(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

bool nvm_is_valid(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

bool nvm_load_data(void);

bool nvm_store_data(dwm_mode_t node_mode);

bool nvm_validate(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

#endif //NVM