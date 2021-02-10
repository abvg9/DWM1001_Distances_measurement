#include "common.h"

bool err_check(int err_code) {

  if(err_code != DWM_OK) {
    return false;
  }

  return true;
}