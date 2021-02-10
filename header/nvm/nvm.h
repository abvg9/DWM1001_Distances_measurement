#ifndef NVM
#define NVM

#include "common.h"

/***************
 * DEFINITIONS *
 ***************/

#define NVM_VARIABLE_SIZE 2

/********************************
 * NVM POSIBLE VARIABLES VALUES *
 ********************************/

#define NVM_TRUE 0X00

/*********
 * ENUMS *
 *********/
typedef enum {
  valid_NVM = 0,
  last_mode = valid_NVM + NVM_VARIABLE_SIZE,
  number_of_neighbors = last_mode + NVM_VARIABLE_SIZE,
  neighbors_start_address = number_of_neighbors + 1
} nvm_memory_position;

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Set zeros in all the positions of the NVM.
 *
 * @return bool.
 * @retval true Success.
 * @retval false Can not write in nvm.
 */
bool clean_memory(void);

/**
 * @brief Check if some boolean variable of the NVM is true or false.
 *
 * @param[in] nvm: The non volatile memory of the board.
 * @param[in] mp: Position of the variable in the nvm.
 *
 * @return bool
 * @retval true If variable is true.
 * @retval false If variable is false.
 */
bool check_nvm_boolean_variable(nvm_memory_position mp, uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

/**
 * @brief Load and return a list of the neighbors stored in the nvm.
 *
 * @return rangin_neighbors
 * @retval List of the neighbors stored in the nvm.
 */
rangin_neighbors load_neighbors(void);

/**
 * @brief Store a list of neighbors in the nvm.
 *
 * @param[in] neighbors: Id neighbors to store.
 *
 * @return bool
 * @retval true If neighbors
 * @retval false If variable is false.
 */
bool store_neighbors(rangin_neighbors neighbors);

#endif //NVM