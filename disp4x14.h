#ifndef __DISP4X14_H__
#define __DISP4X14_H__
#include "Adafruit_LEDBackpack.h"

enum disp_states{
  START_UP,
  SENSORS,
  KBD_3X4,
  LAST_STATE
};

void disp4x14_init(void);
void disp4x14_str(int8_t element, const char *c_arr);
void disp4x14_float(int8_t element, float fval, uint8_t decimals);
void disp_set_time_out(uint16_t ta);
void disp_set_state(enum disp_states d_st);
void disp_machine(void);
void disp_set_buf( enum disp_states d_st, uint8_t element, char *p);
#endif
