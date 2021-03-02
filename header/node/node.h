#ifndef NODE
#define NODE

#include "nvm.h"

/************
 * MESSAGES *
 ************/
typedef enum {
  clean_message_buffer,
  net_nodes_finded,
  tag_got_distances
} message_type;

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
 * @brief Send a message that other nodes can read.
 *
 * @param[in] m: Type of message to send.
 *
 * @return bool
 * @retval true If the message could be sent.
 * @retval false If the message could not be sent.
 */
bool send_message(message_type m);

/**
 * @brief Loop until all the anchors of the net are finded.
 *
 * @param[in] data: Pointer to the parameters of the function
 */
void get_anchor_distances_thread(uint32_t data);

/**
 * @brief Check if all the scanned anchors scanned the other anchors.
 *
 * @param[in] dwm_anchor_list_t: List of the scanned anchors.
 *
 * @return bool
 * @retval true If all the scanned anchors scanned the other anchors.
 * @retval false If not all the scanned anchors scanned the other anchors.
 */
bool is_anchor_scan_finished(dwm_anchor_list_t anchors_list);

/**
 * @brief Check if all idle timer of a thread is expired.
 *
 * @return bool
 * @retval true If idle timer was expired.
 * @retval false If idle timer was not expired.
 */
bool is_idle_time_expired(void);

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
 * @brief Reset the board to the initial state.
 *
 * @return bool
 * @retval true If the node could be reset.
 * @retval false If the node could not be reset.
 */
bool reset_board(void);

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
 * @brief Set position to the node.
 *
 * @return bool
 * @retval true If the position can be set.
 * @retval false If the position can not be set.
 */
bool set_position(dwm_pos_t pos);

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

/**
 * @brief Loop until the tag of the net are finded.
 *
 * @param[in] data: Pointer to the parameters of the function
 */
void wait_tag_thread(uint32_t data);

#endif //NODE