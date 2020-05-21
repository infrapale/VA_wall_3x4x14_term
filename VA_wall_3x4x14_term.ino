/*
 * Villa Astrid Wall Terminal 
 * 3x4 character 14segment display elements
 * RFM69 434MHz Radio module
 * Tom Höglund 2020
 * 
 * https://github.com/infrapale/VA_wall_3x4x14_term
 */

#include <stdio.h>
#include <string.h>
#include <avr/dtostrf.h>
#include <Wire.h>

#include <rfm69_support.h>
#include "light_msg.h"
#include "disp4x14.h"
#include "akbd.h"
#include "TaHa.h" 

/*
 * RFM69 Definitions for the M0+RFM69 Feather
 */
#define FREQUENCY     RF69_434MHZ
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
#define RFM69_FREQ    434.0 
#define RFM69_TX_IVAL_100ms  20;

   
#define KBD_NBR_KEYS       12
#define BTN_NBR_BTNS       3


int16_t dec_div[] = {1,10,100,1000,10000};

char buf[16];
akbd btn4(A0);
akbd kbd3x4(A1);
uint16_t kbd_values[KBD_NBR_KEYS] = {
  52,92,132,198,312,413,550,699,788,871,936,967
};
uint16_t btn_values[BTN_NBR_BTNS] = {707,917,420};


// Task handler definitions
TaHa taha_kbd_scan;
TaHa taha_btn_scan;
TaHa taha_menu;
TaHa radio_send_handle;
TaHa display_handle;
/**
 * @brief  Scan Analog Keyboard, pressed keys are stored in object buffer
 * @param  -
 * @retval -
 */
void scan_kbd (void)
{
  kbd3x4.scan();
}
/**
 * @brief Scan 3 (4) analog keys and store pressed keys in object buffer
 * @param
 * @retval
 */
void scan_btn(void)
{
    btn4.scan();
}

void /**
 * @brief  Control menu using a rotaty encoder
 *         if menu is changed display menu position
 * @param  -
 * @retval -
 */
menu_handler(void)
{
   char btn_on_off_ret = btn4.read();  
   //Serial.println(btn_on_off);
   boolean show_menu = menu_task(btn_on_off_ret);
   if (show_menu){
      disp_set_state(MENU_STATE);
      disp_set_text(MENU_STATE, 0, get_menu_name());
      disp_set_text(MENU_STATE, 1, "On  ");
      disp_set_text(MENU_STATE, 2, "Off ");
      // Serial.println("menu_handler test");
      disp_set_time_out(10);   
   }
}


/**
 * @brief Arduino setup function
 * @param -
 * @retval -
 */
void setup() {
  delay(3000);
  Serial.begin(9600);

  menu_init();
  radio_init(RFM69_CS,RFM69_INT,RFM69_RST, RFM69_FREQ);
  radio_send_msg("Telmac Wall Terminal 14-segment");
  disp4x14_init();
  init_light_msg();
  
  kbd3x4.set_nbr_keys(KBD_NBR_KEYS);
  btn4.set_nbr_keys(BTN_NBR_BTNS);
  for(uint8_t i = 0;i < KBD_NBR_KEYS; i++){ 
      kbd3x4.set_aval(i, kbd_values[i]);
  }
  for(uint8_t i = 0;i < BTN_NBR_BTNS; i++){ 
      btn4.set_aval(i, btn_values[i]);
  }
 
  /*
   * Define task execution
   */
   
  taha_kbd_scan.set_interval(10,RUN_RECURRING, scan_kbd);
  taha_btn_scan.set_interval(10,RUN_RECURRING, scan_btn);
  taha_menu.set_interval(100,RUN_RECURRING, menu_handler);
  radio_send_handle.set_interval(500,RUN_RECURRING, radio_tx_handler);
  display_handle.set_interval(DISP_UPD_IVAL ,RUN_RECURRING, disp_machine);
 
}
/**
 * @brief Run tasks  via scheduler 
 * @param -
 * @retval -
 */
void loop() {
  char btn;
  
  taha_kbd_scan.run();
  taha_btn_scan.run();
  taha_menu.run();
  radio_send_handle.run();
  display_handle.run();
  
  btn = kbd3x4.read();
  if (btn) {
    //Serial.println(btn);uint16_t aval = kbd3x4.rd_analog(); Serial.println(aval);
    act_on_kbd3x4(btn);
  }
}
