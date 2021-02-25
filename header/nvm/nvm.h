#ifndef NVM
#define NVM

#include "common.h"

/***************
 * DEFINITIONS *
 ***************/
#define NVM_VALID_VARIABLE_SIZE 6
#define VALID_VALUE 0XAB

/*********
 * ENUMS *
 *********/
typedef enum {
  valid_nvm = 0, // Indicates if the NVM has a valid configuration.
  number_of_scanned_neighbors = NVM_VALID_VARIABLE_SIZE, // Number of sanned neighbors.
  tag_index = number_of_scanned_neighbors + 1, // Index of node must be a tag.
  my_neighbor_index = tag_index + 1, // Index of the node in the neighbors list.
  initiator_index = my_neighbor_index + 1, // Index of node must be the initiator.
  neighbors_start_address = initiator_index + 1 // Initial address of the neighbors.
} nvm_memory_position;

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Check if the nvm is valid.
 *
 * @param[in] nvm: The non volatile memory of the board.
 *
 * @return bool
 * @retval true If nvm is valid.
 * @retval false If nvm is not valid.
 */
bool check_nvm_validity(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

/**
 * @brief Prepare nvm to have the correct format.
 *
 * @param[out] nvm: The dirty non volatile memory.
 *
 * @return bool.
 * @retval true Success.
 * @retval false Can not write in nvm.
 */
bool clean_memory(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

/**
 * @brief Set zeros to all the positions fo the nvm.
 *
 * @param[out] nvm: The dirty non volatile memory.
 *
 * @return bool.
 * @retval true Success.
 * @retval false Can not write in nvm.
 */
bool flush_nvm(uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX]);

/**
 * @brief Get a uint8 value of the nvm.
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
 * @brief Set a value to a uint8 variable of the nvm.
 *
 * @param[in] mp: Position of the variable in the nvm.
 * @param[in] value: Value to set.
 *
 * @return bool
 * @retval true If variable was modified correctly.
 * @retval false If variable was not modified correctly.
 */
bool set_nvm_uint8_variable(nvm_memory_position mp, uint8_t value);

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