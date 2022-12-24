#include "ROBLEX.h"

void  ROBLEX::ReadApp(String cmd) {
    int index [20];
    index[0] = -1;
    String value [20];
    bool last = false;
    int i = 1;
    while (!last) {
      index[i] = cmd.indexOf(',', index[i - 1] + 1);
      ROBLEX::AppValue[i - 1] = cmd.substring(index[i - 1] + 1, index[i]);
      if ( index[i] < 0) {
        last = true;
      }
      i++;
    }
  }
