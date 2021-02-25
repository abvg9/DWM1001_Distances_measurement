#ifndef NODE
#define NODE

#include "nvm.h"

/*************
 * FUNCTIONS *
 *************/

/**
 * @brief Check if a given configuration is equal to the default configuration.
 *
 * @param[in] expected_mode: Expected mode of the node (DWM_MODE_ANCHOR or DWM_MODE_TAG).
 * @param[in] cfg: Current configuration of the node.
 *
 * @return bool
 * @retval true If the given configuration is equal to the default configuration.
 * @retval false If the given configuration is not equal to the default configuration.
 */
bool check_configuration(dwm_mode_t expected_mode, dwm_cfg_t cfg);

/**
 * @brief Loop until all the anchor of the net are finded.
 *
 * @param[in] data: Pointer to the parameters of the function
 *
 */
void get_anchor_distances_thread(uint32_t data);

/**
 * @brief Search if an id is in the list of neighbors.
 *
 * @param[in] node_id: Id to search.
 *
 * @return int
 * @retval >= 0 If node_id is inside neighbors list.
 * @retval -1  If node_id is not inside neighbors list.
 */
int is_there_neighbor(uint16_t node_id);

/**
 * @brief Loop until all the nodes of the net are finded and
 *        store them in the nvm.
 *
 * @param[in] data: Pointer to the parameters of the function.
 *
 */
void scan_neighbors_thread(uint32_t data);

/**
 * @brief Select the node mode based on the node index in the
 *        neighbors list.
 *
 * @param[in] index: Index of the node in the neigbors list.
 *
 * @return dwm_mode_t
 * @retval The mode must have the node (DWM_MODE_ANCHOR or DWM_MODE_TAG).
 */
dwm_mode_t select_node_mode(uint8_t index);

/**
 * @brief Set anchor configuration to the node.
 *
 * @param[in] is_initiator: Indicate if the anchor it must be the initiator.
 *
 * @return bool
 * @retval true If the node could be configured as anchor.
 * @retval false If the node could not be configured as anchor.
 */
bool set_node_as_anchor(bool is_initiator);

/**
 * @brief Set anchor configuration to the tag.
 *
 * @return bool
 * @retval true If the node could be configured as tag.
 * @retval false If the node could not be configured as tag.
 */
bool set_node_as_tag(void);

/**
 * @brief Store id in the list of neighbors.
 *
 * @param[in] node_id: Id to store.
 *
 */
void store_neighbor(uint64_t node_id);

/**
 * @brief Update nvm to the next state of the network.
 *
 */
void update_state(void);

void wait_tag_thread(uint32_t data);

#endif //NODE