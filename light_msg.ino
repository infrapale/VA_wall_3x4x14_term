#include "light_msg.h"
#include "disp4x14.h"

#define CODE_LEN      6
#define ZONE_LEN      4
#define FUNC_LEN      4
#define MSG_BUF_LEN 32
#define MSG_BUF_LEN_MASK 0b00011111;

struct msg_buf_struct {
  char code[CODE_LEN];  // ring buffer
  char zone[ZONE_LEN];  // ring buffer
  char func[FUNC_LEN];
} msg_buf[MSG_BUF_LEN];  

byte buf_wr_indx;
byte buf_rd_indx;


void init_light_msg(void){
     // clear code and zone buffers
    for(uint8_t i=0;i<MSG_BUF_LEN; i++){
        msg_buf[i].zone[0] = 0;
        msg_buf[i].code[0] = 0;
        msg_buf[i].func[0] = 0;
    }
    buf_wr_indx = 0;
    buf_rd_indx = 0;

}

void act_on_kbd3x4(char btn){
    switch(btn){
       case '0': add_code("MH2","RWC_2","T"); 
                 break;        
       case '1': add_code("MH2","RET_1","T"); 
                 break;
       case '2': add_code("TK1","RPOLK","T"); 
                 break;
       case '3': add_code("MH2","RMH21","T"); 
                 add_code("MH2","RMH22","T"); 
                 break;     
       case '4': add_code("TK1","RPARV","T"); 
                 break;   
       case '5': add_code("MH1","RMH11","T"); 
                 add_code("MH1","RMH12","T"); 
                 add_code("MH1","RMH13","T"); 
                 break;   
       case '6': add_code("MH2","RKHH2","T"); 
                 add_code("MH2","RPSH1","T");
                 break;   
       case '7': add_code("TK1","RTUP1","T"); 
                 add_code("TK1","RTUP2","T");
                 break;   
       case '8': add_code("MH1","RKOK1","T"); 
                 add_code("MH1","RKOK2","T"); 
                 add_code("MH1","RKOK3","T"); 
                 add_code("MH1","RKOK4","T"); 
                 add_code("MH1","RKOK5","T"); 
                  break;   
       case '9': add_code("MH1","xxxxx","1"); break;   
       case '*': add_code("MH1","*.OFF","0"); 
                 add_code("MH2","*.OFF","0");
                 add_code("TK1","*.OFF","0");
                 break;   
       case '#': add_code("MH1","RKOK3","T"); 
                 add_code("MH1","RKOK4","T"); 
                 add_code("MH1","RKOK5","T"); 
                 break;   
    }  
  
}


void add_code(const char *new_zone, const char *new_code, const char *new_func){
    int i;
    for(i = 0; i < CODE_LEN; i++) {
        if (new_code[i] != 0) { 
            msg_buf[buf_wr_indx].code[i] = new_code[i];
        } 
        else {
            msg_buf[buf_wr_indx].code[i] =0;
        }   
    }
    for(i = 0; i < ZONE_LEN; i++) {
        if (new_code[i] != 0) { 
             msg_buf[buf_wr_indx].zone[i] = new_zone[i];
        } 
        else {
            msg_buf[buf_wr_indx].zone[i] =0;
        }   
    }
    for(i = 0; i < FUNC_LEN; i++) {
        if (new_func[i] != 0) { 
             msg_buf[buf_wr_indx].func[i] = new_func[i];
        } 
        else {
            msg_buf[buf_wr_indx].func[i] =0;
        }   
    }
    buf_wr_indx = ++buf_wr_indx & MSG_BUF_LEN_MASK;   
}


void radiate_msg( const char *zone, const char *relay_addr, char *func ) {
    char rf69_packet[RH_RF69_MAX_MESSAGE_LEN];
    if (json_char_array(rf69_packet, RH_RF69_MAX_MESSAGE_LEN, zone, relay_addr, func, "") > 0)
    {
      radio_send_msg(rf69_packet);
      Serial.println(rf69_packet);
      disp_set_buf( KBD_3X4, 0, zone);
      disp_set_buf( KBD_3X4, 1, relay_addr+1);
      disp_set_buf( KBD_3X4, 2, func);
      disp_set_state(KBD_3X4);
      disp_set_time_out(5);
     
    }
    else 
    {
      Serial.println("RFM69 message length was exceeded"); 
    }
}

void radio_tx_hanndler(void){
    if (msg_buf[buf_rd_indx].code[0] != 0){
        radiate_msg( msg_buf[buf_rd_indx].zone, msg_buf[buf_rd_indx].code, msg_buf[buf_rd_indx].func);
        Serial.print(msg_buf[buf_rd_indx].zone); 
        Serial.println(msg_buf[buf_rd_indx].code);
        msg_buf[buf_rd_indx].zone[0] = 0;
        msg_buf[buf_rd_indx].code[0] = 0;
        msg_buf[buf_rd_indx].code[0] = 0;
        buf_rd_indx = ++buf_rd_indx & MSG_BUF_LEN_MASK; 
        radio_send_handle.delay_task(2000);
    }
}
