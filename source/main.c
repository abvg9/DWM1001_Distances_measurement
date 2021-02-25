#include "node.h"
#include "nvm.h"

rangin_neighbors neighbors;

// ORDENES QUE VENDRÁN DE LA CONTROLADORA
#define FLUSH_MEMORY false
#define PAIND 0xDECA
const bool INITIAL_INITIATOR = true;

int dwm_user_start(void) {

  // while(controller send a message)
  // swtich case with the commands.
  uint8_t nvm[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(!err_check(dwm_nvm_usr_data_get(nvm, &len))) {
    return -1;
  }

  if(FLUSH_MEMORY) {

    flush_nvm(nvm);

  } else {

    if(!err_check(dwm_panid_set(PAIND))) {
      return false;
    }

    if(!check_nvm_validity(nvm)) {

      if(!clean_memory(nvm)) {
        return -1;
      }

    } else {
      neighbors = load_neighbors();
    }

    uint8_t index = get_nvm_uint8_variable(my_neighbor_index);

    dwm_mode_t mode = select_node_mode(index);

    if(mode == -1) {
      return -1;
    }

    if(mode == DWM_MODE_ANCHOR && (neighbors.cnt == 0 || index > DWM_RANGING_ANCHOR_CNT_MAX)) {

      uint8_t scan_neighbors_hndl;

      if(!err_check(dwm_thread_create(THREAD_PRIO, scan_neighbors_thread, (void*)NULL,
                    "scan_neighbors", THREAD_STACK_SIZE, &scan_neighbors_hndl))) {
        return -1;
      }

      if(!err_check(dwm_thread_resume(scan_neighbors_hndl))) {
        return -1;
      }

    } else if(index == get_nvm_uint8_variable(tag_index)) {

      uint8_t get_anchor_distances_hndl;

      if(!err_check(dwm_thread_create(THREAD_PRIO, get_anchor_distances_thread, (void*)NULL,
                      "Get_distances_to_anchors", THREAD_STACK_SIZE, &get_anchor_distances_hndl))) {
        return -1;
      }

      if(!err_check(dwm_thread_resume(get_anchor_distances_hndl))) {
        return -1;
      }

    } else {

      uint8_t wait_tag_hndl;

      if(!err_check(dwm_thread_create(THREAD_PRIO, wait_tag_thread, (void*)NULL,
                    "wait_tag", THREAD_STACK_SIZE, &wait_tag_hndl))) {
        return -1;
      }

      if(!err_check(dwm_thread_resume(wait_tag_hndl))) {
        return -1;
      }

    }

  }

  return 0;

}