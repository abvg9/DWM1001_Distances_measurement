#ifndef NODE
#define NODE

#include "nvm.h"
#include <stdio.h>

/*************
 * FUNCTIONS *
 *************/

void anchor_scan_thread(uint32_t data);

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
 * @brief Create a dwm_pos_t based on receibed parameters.
 *
 * @param[in] x: Position in axis x (mm).
 * @param[in] y: Position in axis y (mm).
 * @param[in] z: Position in axis z (mm).
 * @param[in] quality_factor: Value that determines the quality of the measurements.
 *
 * @return dwm_pos_t
 */
dwm_pos_t create_position(int32_t x, int32_t y, int32_t z, uint8_t quality_factor);

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

void message_event_callback(dwm_evt_t *p_evt);

void message_handler_thread(uint32_t data);

/**
 * @brief Set anchor configuration to the node.
 *
 * @return bool
 * @retval true If the node could be configured as anchor.
 * @retval false If the node could not be configured as anchor.
 */
bool set_node_as_anchor(bool isInitiator);

/**
 * @brief Set anchor configuration to the tag.
 *
 * @return bool
 * @retval true If the node could be configured as tag.
 * @retval false If the node could not be configured as tag.
 */
bool set_node_as_tag(void);

dwm_mode_t set_node_mode(uint8_t index);

/**
 * @brief Store id in the list of neighbors.
 *
 * @param[in] node_id: Id to store.
 *
 */
void store_neighbor(uint64_t node_id);

void tag_scan_thread(uint32_t data);

#endif //NODE