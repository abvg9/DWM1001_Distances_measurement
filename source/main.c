#include "node.h"
#include "nvm.h"

int dwm_user_start(void) {

  // Inicialmente ponemos el nodo como anchor, para que empiecen
  // a escanearse entre ellos.
  if(!set_node_as_anchor()) {
    return -1;
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

  // Event thread
  uint8_t event_hndl;

  if(!err_check(dwm_thread_create(THREAD_PRIO, dwm_event_thread, (void*)NULL,
                  "Event_handler", THREAD_STACK_SIZE, &event_hndl))) {
    return -1;
  }

  // Start the thread 
  if(!err_check(dwm_thread_resume(event_hndl))) {
    return -1;
  }

  return 0;

}