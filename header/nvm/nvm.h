#ifndef NVM
#define NVM

#include "common.h"

/***************
 * DEFINITIONS *
 ***************/
#define NVM_VALID_VARIABLE_SIZE 3
#define VALID_VALUE 0XFF

/*********
 * ENUMS *
 *********/
typedef enum {
  valid_NVM = 0, // Indicates if the nvm has a valid configuration.
  number_of_scanned_neighbors = valid_NVM + NVM_VALID_VARIABLE_SIZE, // Number of sanned neighbors.
  tag_index = number_of_scanned_neighbors + 1, // Index of node must be a tag.
  initiator_index = tag_index + 1, // Index of the node must be a initiator
  neighbors_start_address = initiator_index + 1 // Initial address of the neighbors.
} nvm_memory_position;

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Check if the NVM is valid.
 *
 * @param[in] nvm: The non volatile memory of the board.
 *
 * @return bool
 * @retval true If NVM is valid.
 * @retval false If NVM is not valid.
 */
bool check_nvm_validity(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

/**
 * @brief Prepare NVM to have the correct format.
 *
 * @param[out] nvm: The non volatile memory cleaned and ready to use.
 *
 * @return bool.
 * @retval true Success.
 * @retval false Can not write in nvm.
 */
bool clean_memory(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

/**
 * @brief Get a uint8 value of the NVM.
 *
 * @param[in] mp: Position of the variable in the nvm.
 *
 * @return uint8_t
 * @retval Value of the variable.
 */
uint8_t get_nvm_uint8_variable(nvm_memory_position mp);

/**
 * @brief Load and return a list of the neighbors stored in the nvm.
 *
 * @return rangin_neighbors
 * @retval List of the neighbors stored in the nvm.
 */
rangin_neighbors load_neighbors(void);

/**
 * @brief Set a value to a uint8 variable of the NVM.
 *
 * @param[in] mp: Position of the variable in the nvm.
 * @param[in] value: Value to set.
 *
 * @return bool
 * @retval true If variable was modified correctly.
 * @retval false If variable was not modified correctly.
 */
bool set_nvm_uint8_variable(nvm_memory_position mp, int value);

void set_zeros_nvm(void);

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