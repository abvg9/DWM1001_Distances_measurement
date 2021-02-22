#include "node.h"
#include "nvm.h"

rangin_neighbors neighbors;

// ORDENES QUE VENDRÁN DE LA CONTROLADORA
#define FLUSH_MEMORY false
const uint16_t PANID = 0xDECA;
bool first_run = true;

int dwm_user_start(void) {

  // while(controller send a message)
  // swtich case with the commands.

  if(FLUSH_MEMORY) {
    set_zeros_nvm();
  } else {

    if(!err_check(dwm_panid_set(PANID))) {
      return -1;
    }

    uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX];
    uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

    if(err_check(dwm_nvm_usr_data_get(buf, &len))) {

      if(!check_nvm_validity(buf)) {

        if(!clean_memory(buf)) {
          return -1;
        }

      } else {
        neighbors = load_neighbors();

        if(neighbors.cnt == 0) {
          first_run = true;
        } else {
          first_run = false;
        }
      }

    } else {
      return -1;
    }

    uint8_t index = get_nvm_uint8_variable(my_neighbor_index);

    dwm_mode_t mode = set_node_mode(index, first_run);

    if(mode == -1) {
      return -1;
    }

    if(mode == DWM_MODE_ANCHOR && (neighbors.cnt == 0 || index > DWM_RANGING_ANCHOR_CNT_MAX+1)) {

      // Anchor scan thread
      uint8_t anchor_scan_hndl;

      if(!err_check(dwm_thread_create(THREAD_PRIO, anchor_scan_thread, (void*)NULL,
                    "Scan_anchors", THREAD_STACK_SIZE, &anchor_scan_hndl))) {
        return -1;
      }

      if(!err_check(dwm_thread_resume(anchor_scan_hndl))) {
        return -1;
      }

    } else if(index == get_nvm_uint8_variable(tag_index)) {

      // Tag scan thread
      uint8_t tag_scan_hndl;

      if(!err_check(dwm_thread_create(THREAD_PRIO, tag_scan_thread, (void*)NULL,
                      "Get_distances_to_anchors", THREAD_STACK_SIZE, &tag_scan_hndl))) {
        return -1;
      }

      if(!err_check(dwm_thread_resume(tag_scan_hndl))) {
        return -1;
      }
    }

    // Event thread
    uint8_t message_event_hndl;

    if(!err_check(dwm_thread_create(THREAD_PRIO, message_handler_thread, (void*)NULL,
                      "Message_handler", THREAD_STACK_SIZE, &message_event_hndl))) {
      return -1;
    }

    if(!err_check(dwm_thread_resume(message_event_hndl))) {
      return -1;
    }
  }

  return 0;

}