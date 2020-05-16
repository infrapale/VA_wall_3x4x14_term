#define ALPHA_DIGITS   4 
#define ALPHA_ELEMENTS 3 
#include <stdio.h>
#include <string.h>
#include <avr/dtostrf.h>
#include <Arduino.h>


#include "disp4x14.h"

uint8_t alpha_i2c_addr[ALPHA_ELEMENTS] ={0x70,0x71,0x72};

struct alpha4_struct {
   Adafruit_AlphaNum4 alpha4;
   char buff[ALPHA_DIGITS];
   uint8_t dot_pos;
};

static uint16_t time_out;
char disp_buf[LAST_STATE][ALPHA_ELEMENTS][5];
enum disp_states disp_state;



//Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
alpha4_struct disp[ALPHA_ELEMENTS];

void disp4x14_init(void){
  
  for (uint8_t element = 0; element < ALPHA_ELEMENTS; element++){
    disp[element].alpha4.begin(alpha_i2c_addr[element]); 
    //Serial.print("14 Segment test element: ");
    //Serial.println(alpha_i2c_addr[element]);
      
    disp[element].alpha4.clear();
  } 
  time_out = 10;
  disp_state = START_UP;
  strcpy(disp_buf[START_UP][0],"-VA-");
  strcpy(disp_buf[START_UP][1],"KURU");
  strcpy(disp_buf[START_UP][2],"2020");

}

void disp4x14_str(int8_t element, const char *c_arr){
  disp[element].alpha4.clear();
  //disp[element].alpha4.setBrightness(50);
  uint8_t i = 0;
  //Serial.println(c_arr);
  while((i < ALPHA_DIGITS) && (c_arr[i] != 0))
  {
      disp[element].alpha4.writeDigitAscii(i,c_arr[i] ,false);   
      i++;   
  }
  disp[element].alpha4.writeDisplay();
} 

void disp4x14_float(int8_t element, float fval, uint8_t decimals){
  char buf[12];
  dtostrf(fval, 2, decimals, buf);
  uint8_t buf_indx = 0;
  uint8_t disp_indx = 0;
  uint8_t dec_pos = 99;
  bool    put_dot;
  char    *pch;
  pch=strstr(buf,".");
  if (pch!=NULL) {
    dec_pos = (uint8_t)(pch - buf - 1);
  }
  // Serial.println(buf);
  if (buf[buf_indx]=='-'){
    disp[element].alpha4.writeDigitAscii(disp_indx, '-',false);
    buf_indx++;
  }
  else
  {
    disp[element].alpha4.writeDigitAscii(disp_indx, '+',false);
  }
  while((++disp_indx < ALPHA_DIGITS) &&
        (buf_indx < strlen(buf))){
    if (buf_indx == dec_pos) {
      put_dot = true;
    }
    else {
      put_dot = false;
    }
    disp[element].alpha4.writeDigitAscii(disp_indx,buf[buf_indx] ,put_dot);
     buf_indx++;
    if (put_dot) buf_indx++;
  }
  disp[element].alpha4.writeDisplay();
} 

void disp_set_time_out(uint16_t ta){
  time_out = ta;
}

void update_disp(enum disp_states d_st){
  disp4x14_str(0, disp_buf[d_st][0]);
  disp4x14_str(1, disp_buf[d_st][1]);
  disp4x14_str(2, disp_buf[d_st][2]);
}

void disp_machine(void){

  if (time_out > 0) {
    time_out--;
  }

  switch(disp_state){
    case START_UP:
      update_disp(disp_state);
      if (time_out == 0){
        disp_state = SENSORS;
      }
      break;
    case SENSORS:
      disp4x14_float(0, 23.1, 1);
      disp4x14_float(1, 18.4, 1);
      disp4x14_float(2, 17.2, 1);
      break;
    case KBD_3X4:
      update_disp(disp_state);
      if (time_out == 0){
        disp_state = SENSORS;
      }
      break;
  }
}

void disp_set_state(enum disp_states d_st){
  disp_state = d_st;
}

void disp_set_buf( enum disp_states d_st, uint8_t element, const char *p){
  strcpy(disp_buf[d_st][element],p);
  uint8_t i;
  for( i = 0;i< ALPHA_DIGITS;i++) {
    if (i < strlen(p)){
       disp_buf[d_st][element][i] = p[i];
    }
    else {
      disp_buf[d_st][element][i] = ' ';
    }
  }  
}
