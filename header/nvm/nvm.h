#ifndef NVM
#define NVM

#include "common.h"

/***************
 * DEFINITIONS *
 ***************/

#define NVM_VARIABLE_SIZE 2

/****************************************
 * NVM POSIBLE BOOLEAN VARIABLES VALUES *
 ****************************************/
#define NVM_TRUE 0X00
#define NVM_FALSE 0X01

/*********
 * ENUMS *
 *********/
typedef enum {
  valid_NVM = 0, // Indicates if the nvm has a valid configuration.
  mode = valid_NVM + NVM_VARIABLE_SIZE, // Indicates in wich mode the node should operate.
  initiator = mode + NVM_VARIABLE_SIZE, // Indicate if the anchor must be initiator.
  number_of_scanned_neighbors = initiator + NVM_VARIABLE_SIZE, // Number of sanned neighbors.
  tag_index = number_of_scanned_neighbors + 1, // Index of node must be a tag.
  initiator_index = tag_index + 1, // Index of the node must be a initiator
  neighbors_start_address = number_of_scanned_neighbors + 1 // Initial address of the neighbors.
} nvm_memory_position;

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Set zeros in all the positions of the NVM.
 *
 * @param[out] buf: The non volatile memory cleaned and ready to use.
 *
 * @return bool.
 * @retval true Success.
 * @retval false Can not write in nvm.
 */
bool clean_memory(uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX]);

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
 * @brief Set a value to a boolean variable of the NVM.
 *
 * @param[in] nvm: The non volatile memory of the board.
 * @param[in] mp: Position of the variable in the nvm.
 * @param[in] value: Value to set.
 *
 * @return bool
 * @retval true If variable was modified correctly.
 * @retval false If variable was not modified correctly.
 */
bool set_nvm_boolean_variable(nvm_memory_position mp, uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX], bool value);

/**
 * @brief Store a list of neighbors in the nvm.
 *
 * @param[in] neighbors: Ids of the neighbors to store.
 *
 * @return bool
 * @retval true If neighbors list was stored correctly.
 * @retval false If neighbors list was not stored correctly.
 */
bool store_neighbors(rangin_neighbors neighbors);

#endif //NVM