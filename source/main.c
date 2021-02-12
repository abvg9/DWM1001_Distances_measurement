#include "node.h"
#include "nvm.h"

bool first_run = true;
rangin_neighbors neighbors;

// ORDENES QUE VENDRÁN DE LA CONTROLADORA
#define FLUSH_MEMORY true
bool IS_INITIATOR=true;

int dwm_user_start(void) {

  // while(controller send a message)
  // swtich case with the commands.

  if(FLUSH_MEMORY) {

    set_zeros_nvm();

  } else {

    // Eneable location engine.
    dwm_le_compile();

    uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX];
    uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

    if(err_check(dwm_nvm_usr_data_get(buf, &len))) {

      if(!check_nvm_validity(buf)) {

        if(!clean_memory(buf)) {
          return -1;
        }

      } else {
        first_run = false;
        neighbors = load_neighbors();
      }

    } else {
      return -1;
    }

    if(!set_node_mode(first_run)) {
      return false;
    }

    if(first_run || neighbors.cnt == 0) {

      // Scan thread
      uint8_t scan_hndl;

      if(!err_check(dwm_thread_create(THREAD_PRIO, dwm_anchor_scan_thread, (void*)NULL,
                    "Scan_anchors", THREAD_STACK_SIZE, &scan_hndl))) {
        return -1;
      }

      if(!err_check(dwm_thread_resume(scan_hndl))) {
        return -1;
      }

    } else {

      // Event thread
      uint8_t event_hndl;

      if(!err_check(dwm_thread_create(THREAD_PRIO, dwm_event_thread, (void*)NULL,
                      "Event_handler", THREAD_STACK_SIZE, &event_hndl))) {
        return -1;
      }

      if(!err_check(dwm_thread_resume(event_hndl))) {
        return -1;
      }
    }
  }

  return 0;

}