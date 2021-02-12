#include "node.h"
#include "nvm.h"

bool first_run = true;
uint16_t* node_id = NULL;

int dwm_user_start(void) {

  //llena_de_unos();
  // AQUI SE DEBE LLAMAR A UNA FUNCIÓN QUE LEA SI
  // QUE DEBE EJECUTAR, SI LIMPIEZA DE MEMORIA
  // O EL PROGRAMA PRINCIPAL.

  uint8_t buf[DWM_NVM_USR_DATA_LEN_MAX];
  uint8_t len = DWM_NVM_USR_DATA_LEN_MAX;

  if(err_check(dwm_nvm_usr_data_get(buf, &len))) {

    if(!check_nvm_validity(buf)) {

      if(!clean_memory(buf)) {
        return -1;
      }

    } else {
      first_run = false;
    }

  } else {
    return -1;
  }

  if(!err_check(dwm_panid_get(node_id))) {
    return -1;
  }

  if(*node_id == get_nvm_uint8_variable(tag_index)) {

    if(!set_node_as_tag()) {
      return -1;
    }

  } else {

    if(*node_id == get_nvm_uint8_variable(initiator_index)) {

      if(!set_node_as_anchor(true)) {
        return -1;
      }

    } else {

      if(!set_node_as_anchor(false)) {
        return -1;
      }

    }

    // Scan thread
    uint8_t scan_hndl;

    if(!err_check(dwm_thread_create(THREAD_PRIO, dwm_anchor_scan_thread, (void*)NULL,
                  "Scan_anchors", THREAD_STACK_SIZE, &scan_hndl))) {
      return -1;
    }

    if(!err_check(dwm_thread_resume(scan_hndl))) {
      return -1;
    }

  }

  // Event thread

  uint8_t event_hndl;

  if(!err_check(dwm_thread_create(THREAD_PRIO, dwm_event_thread, (void*)NULL,
                  "Event_handler", THREAD_STACK_SIZE, &event_hndl))) {
    return -1;
  }

  if(!err_check(dwm_thread_resume(event_hndl))) {
    return -1;
  }

  return 0;

}