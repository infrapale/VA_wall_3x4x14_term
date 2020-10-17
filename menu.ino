/*
 * Menu structure controlled by a rotary encoder
 */

#define MENU_TAG_LEN       4
#define MENU_DISP_LEN      5
#define MENU_STACK_DEPTH   8
#define ROT_DIR           -1
#define MAX_BTN            4

#include "rotenc.h"      // library class
#include "rot_enc.h"     // private enhancement
#include "light_msg.h"
#include "Pin_Button.h"

enum enDigitalPins
{
  // Rotary encoder and push button input lines
  dpInEncoderA=5,
  dpInEncoderB=6,
  dpInEncoderC=9,
  dpInButtnOn=10,
  dpInButtnOff=11,
  dpInButtnRet=12,
  dpInButtnFree=13
};

PinBtn butn[MAX_BTN];
 
typedef void (*pMenuFunc)(int);

struct MenuEntryStruct {
  char tag[MENU_TAG_LEN];
  char down[MENU_TAG_LEN];
  char name[5];
  char zone[4];
  char relay[6];
  pMenuFunc func;
  int param;
};

typedef struct MenuPtrStruct {
    uint16_t first;
    uint16_t last;
    uint16_t nbr;
    uint16_t total_nbr;
    uint16_t indx;
    uint16_t last_indx;
    
};

uint16_t menu_stack[MENU_STACK_DEPTH];
uint8_t menu_stack_ptr = 0;
uint16_t menu_cursor;
static MenuPtrStruct menu_handle;

/**
 * Menu Structure
 * - function callback is not yet in use
 */
struct MenuEntryStruct menu[] = {
  //  tag   down name    zone relay     func param
    {"Top","K  ","K*  ", "---", "-----", &nop,69},
    {"Top","MH1","MH1*", "---", "-----", &nop,69},
    {"Top","MH2","MH2*", "---", "-----", &nop,42},
    {"Top","TUP","TUP*", "---", "-----", &nop,42},
    {"Top","WC ","WC *", "---", "-----", &nop,42},
    {"Top","KHH","KHH*", "---", "-----", &nop,42},
    {"Top","ULK","ULK*", "---", "-----",  &nop,42},
    {"K  ","---","K_1 ", "MH1", "RKOK1", &nop,42},
    {"K  ","---","K_2 ", "MH1", "RKOK2", &nop,42},
    {"K  ","---","K_3 ", "MH1", "RKOK3", &nop,42},
    {"K  ","---","POY1", "MH1", "RKOK4", &nop,42},
    {"K  ","---","POY2", "MH1", "RKOK5", &nop,42},
    {"MH1","---","MH11", "MH1", "RMH11", &nop,69},
    {"MH1","---","MH12", "MH1", "RMH12", &nop,69},
    {"MH1","---","MH13", "MH1", "RMH13", &nop,69},
    {"MH2","---","MH21", "MH2", "RMH21", &nop,69},
    {"MH2","---","MH22", "MH2", "RMH22", &nop,69},
    {"TUP","---","TUP1", "TK1", "RTUP1", &nop,69},
    {"TUP","---","TUP2", "TK1", "RTUP1", &nop,69},
    {"TUP","---","PARV", "TK1", "RPARV", &nop,69},
    {"TUP","---","ET  ", "MH2", "RET_1", &nop,69},
    {"TUP","---","TK  ", "MH2", "RTK_1", &nop,69},    
    {"WC ","---","WC_K", "TK1", "RWC_1", &nop, 0},
    {"WC ","---","WC_L", "MH2", "RWC_2", &nop, 0},
    {"KHH","---","KHH1", "TK1", "RKHH1", &nop, 0},
    {"KHH","---","KHH2", "MH2", "RKHH2", &nop, 0},
    {"KHH","---","PESH", "MH2", "RPSH1", &nop, 0},
    {"KHH","---","SAUN", "TK1", "RSAUN", &nop, 0},
    {"ULK","---","PIHA", "TK1", "RPIHA", &nop, 0},
    {"ULK","---","TERA", "TK1", "RTERA", &nop, 0},
    {"ULK","---","POLK", "TK1", "RPOLK", &nop, 0},
    {"!!!","---","    ", "---", "-----", &nop, 0}    
};

/**
 * @brief  Initilaize menu structure
 * @param -
 * @retval -
 */
void menu_init(void){
    Serial.println("menu_init");
    rotenc_init(dpInEncoderA, dpInEncoderB, dpInEncoderC);
    rotenc_set_step(0, 0, 10, 1);
    butn[0].Init(dpInButtnOn,'1');
    butn[1].Init(dpInButtnOff,'0');
    butn[2].Init(dpInButtnRet,'R');
    butn[3].Init(dpInButtnFree,'X');
    menu_stack[0] = 0; 
    menu[0].func(menu[0].param);
    menu_stack[0] = 0;
    menu_cursor = 0;
    menu_handle.first = 0;
    menu_handle.last = 2;
    menu_handle.nbr = 3;
    menu_handle.total_nbr = sizeof(menu)/sizeof(menu[0]);
    
    menu_handle.indx = 0;
    menu_handle.indx = menu_handle.last_indx;
    
    init_rot_enc();
    set_pos_indx(0);
    // int position = rotenc_get_pos();
    //uint8_t pressed = rotenc_rd_pressed();


    // menu[0].func(menu[0].param);
}


void init_rot_enc(void){

}

/**
 * @brief  Menu task called by the scheduler
 * @param
 * @retval
 */
boolean menu_task(char on_off_ret){
  boolean update_disp = false;
  menu_handle.indx = rotenc_get_pos();
  if ( menu_handle.indx != menu_handle.last_indx){
    menu_handle.last_indx = menu_handle.indx;
    update_disp = true;
    Serial.print("menu_task: ");
    Serial.print(menu_handle.indx);Serial.print(", ");
    Serial.print(menu[menu_handle.indx].tag);Serial.print(", ");
    Serial.print(menu[menu_handle.indx].down);
    Serial.println();
  }
  switch(rotenc_rd_pressed()){
    case 1:
      Serial.println(menu[menu_handle.indx].down);
      if(strcmp(menu[menu_handle.indx].down,"---") != 0){ 
        goto_tag(menu[menu_handle.indx].down);
        Serial.println(menu_handle.indx);
        update_disp = true;
      }  
      break;
    case 2: 
      return_tag();
      Serial.println(menu_handle.indx);
      update_disp = true;
      break;  
  }
  switch(on_off_ret){
    case '0':   // OFF
      switch_lights("0");
      update_disp = true;
      break;
    case '1':   // ON
      switch_lights("1");
      update_disp = true;
      break;  
  }
  return update_disp;
}

void switch_lights(char *func){
   char tag[MENU_TAG_LEN];
   char down[MENU_TAG_LEN];
   uint16_t i;
   
   strcpy(tag,menu[menu_handle.indx].down);
   //strcpy(down,menu[menu_handle.indx].down);
   Serial.println("switch_lights:");
   Serial.print("tag: "); Serial.println(tag);
   //Serial.print("down: ");Serial.println(down);   
   if (strcmp(tag, "---") == 0) {
      // not a group code
      Serial.println("individual control:");
      add_code(menu[menu_handle.indx].zone,menu[menu_handle.indx].relay,func);
   }
   else {
       Serial.println("iterative control:");
       Serial.print(menu_handle.indx); Serial.print("-");
       Serial.print(menu_handle.total_nbr); Serial.print("-");
       for (i = menu_handle.indx; i <= menu_handle.total_nbr; i++){
           if ( strcmp(menu[i].tag, tag) == 0 ){
               Serial.print("iterating: ");
               add_code(menu[i].zone,menu[i].relay,func);
               Serial.print(i); Serial.print("-");
               Serial.print(menu[i].zone); Serial.print("-");
               Serial.print(menu[i].relay); Serial.print("-");
               Serial.print(func); Serial.println();
                
           }
       }
       
   }
}

/**
 * @brief
 * @param
 * @retval
 */
void set_pos_indx(uint16_t pindx){
  if (pindx < menu_handle.total_nbr){
    menu_handle.indx = pindx;
    menu_handle.nbr = 1;
    uint16_t i = menu_handle.indx;
    menu_handle.first = menu_handle.indx;
    while( i > 0 ){
      i--;
      if ( strcmp(menu[menu_handle.indx].tag,menu[i].tag) == 0){
        menu_handle.nbr++;
        menu_handle.first = i;
        Serial.print("menu_handle.first= "); Serial.println(menu_handle.first);
      }
      else {
        break;
      }
    }
    
    i = menu_handle.indx;
    menu_handle.last = menu_handle.indx;
    
    while( i< menu_handle.total_nbr - 1 ){
      i++;
      if ( strcmp(menu[menu_handle.indx].tag,menu[i].tag) == 0){
        menu_handle.nbr++;
        menu_handle.last = i;
        Serial.print("menu_handle.last= "); Serial.println(menu_handle.last);
      }
      else {
        break;
      }
    }
    
    rotenc_set_step(menu_handle.indx, menu_handle.first, menu_handle.last,  ROT_DIR);   
    Serial.print(menu_handle.indx);Serial.print(", ");
    Serial.print(menu_handle.first);Serial.print(", ");
    Serial.print(menu_handle.last);Serial.print(", ");
    Serial.print(menu_handle.total_nbr);
    Serial.println();
    
  }
}

/**
 * @brief
 * @param
 * @retval
 */
char *get_menu_name(void )
{
  return menu[menu_handle.indx].name;  
}

/**
 * @brief
 * @param
 * @retval
 */
void goto_tag(char *tag_x){
  uint16_t i = 0;  //look from start
   bool  done = false;
   while (! done){
     if (strcmp(menu[i].tag, tag_x) == 0){
        done = true;
        Serial.print("goto_tag: ");
        Serial.println(menu_stack_ptr);

        if(menu_stack_ptr < MENU_STACK_DEPTH-1){
          menu_stack[menu_stack_ptr] = menu_handle.indx;
          menu_stack_ptr++;
          set_pos_indx(i);
        }
        else {
          Serial.print("menu stack is full");
        }
     }
     else {
       if (i < menu_handle.total_nbr-1) {
         i++;
       }
       else {
         done = true;
       }
     }   
   }
}

/**
 * @brief
 * @param
 * @retval
 */
void return_tag(){
    Serial.print("return_tag: ");
    Serial.print(menu_stack_ptr);
    Serial.println(menu_stack[menu_stack_ptr]);
    
    if(menu_stack_ptr > 0){
        menu_stack_ptr--;
        menu_handle.indx = menu_stack[menu_stack_ptr];  
        set_pos_indx(menu_handle.indx);      
    }  
    else {
      Serial.println("Menu stack is empty");
    }
}

/**
 * @brief
 * @param
 * @retval
 */
uint16_t find_menu_tag( struct MenuEntryStruct *menu, struct MenuPtrStruct *m_handle, char *tag){
    uint8_t ph = 0;  // find_fist, find_last
    uint16_t i = 0;
    bool  done = false;
    char tags[3][4] = {"123", "---", "!!!"};
    strcpy(tags[0],tag);
    //Serial.println(tag);
    m_handle->first = -1;
    m_handle->last = -1;
    m_handle->nbr = 0;
    while (! done){
        //Serial.print("i="); Serial.println(i);
        uint8_t tag_indx = 0;
        //Serial.print("menu="); Serial.println(menu[i].tag);
        for (tag_indx = 0; tag_indx < 3; tag_indx++){
            if ( strcmp(tags[tag_indx],menu[i].tag) ==0) {
                break; 
            } 
        }
        //Serial.print("tag_indx="); Serial.println(tag_indx);
        //Serial.print("Ph=");Serial.println(ph);
        switch(ph){
        case 0:  // not yet found
            switch(tag_indx){
                case 0:  // == tag
                    ph++; 
                    m_handle->first = i;
                    break;
                case 1:  // == "---"
                    break;
                case 2:  // == "!!!"
                    done = true;
                     m_handle->nbr = 0;
                    break;
            }
            break;
        case 1:  // reading till end
            switch(tag_indx){
                case 0:  // == tag
                    break;
                case 1:  // == "---"
                    if (m_handle->first >= 0){
                        m_handle->last = i-1;
                        m_handle->nbr = m_handle->last - m_handle->first + 1 ; 
                        m_handle->indx = m_handle->first;
                    }    
                    else {
                        m_handle->nbr = 0;  
                    }    
                    done = true;
                    break;
                case 2:  // == "!!!"
                    m_handle->nbr = 0;  
                    done = true;
                    break;
            }
        }
        i++;
                        
    } 
    
    return(m_handle->nbr);
}
/**
 * @brief
 * @param
 * @retval
 */
void nop(int x)
{
    Serial.println(x);
}
