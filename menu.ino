
#define MENU_TAG_LEN       4
#define MENU_DISP_LEN      5
#define MENU_STACK_DEPTH   8
#define ROT_DIR           -1

#include "rotenc.h"

enum enDigitalPins
{
  // Rotary encoder and push button input lines
  dpInEncoderA=5,
  dpInEncoderB=6,
  dpInEncoderC=9,
};

 
typedef void (*pMenuFunc)(int);

struct MenuEntryStruct {
  char tag[MENU_TAG_LEN];
  char down[MENU_TAG_LEN];
  char name[5];
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


struct MenuEntryStruct menu[] = {
  //  tag   down name        func param
    {"Top","MH1","MH1*", &nop,69},
    {"Top","MH2","MH2*", &nop,42},
    {"Top","TUP","Tupa", &nop,42},
    {"---","---","    ", &nop, 0},
    {"MH1","---","MH11", &nop,69},
    {"MH1","---","MH12", &nop,69},
    {"MH1","---","MH13", &nop,69},
    {"---","---","    ", &nop, 0},
    {"MH2","---","MH21", &nop,69},
    {"MH2","---","MH22", &nop,69},
    {"---","---","    ", &nop, 0},
    {"TUP","---","Tup1", &nop,69},
    {"TUP","---","Tup2", &nop,69},
    {"---","---","    ", &nop, 0},
    {"!!!","---","    ", &nop, 0}    
};

void menu_init(void){
    Serial.println("menu_init");
    menu_stack[0] = 0;
    menu_cursor = 0;
    menu_handle.first = 0;
    menu_handle.last = 2;
    menu_handle.nbr = 3;
    menu_handle.total_nbr = sizeof(menu)/sizeof(menu[0]);
    
    menu_handle.indx = 0;
    menu_handle.indx = menu_handle.last_indx;
    
    rotenc_init(dpInEncoderA, dpInEncoderB, dpInEncoderC);
    set_pos_indx(0);
    // int position = rotenc_get_pos();
    //uint8_t pressed = rotenc_rd_pressed();


    // menu[0].func(menu[0].param);
}

boolean menu_task(void){
  boolean update_disp = false;
  menu_handle.indx = rotenc_get_pos();
  if ( menu_handle.indx != menu_handle.last_indx){
    menu_handle.last_indx = menu_handle.indx;
    update_disp = true;
    Serial.print(menu_handle.indx);Serial.print(", ");
    Serial.print(menu[menu_handle.indx].tag);Serial.print(", ");
    Serial.print(menu[menu_handle.indx].down);
    Serial.println();
  }
  switch(rotenc_rd_pressed()){
    case 1:
      Serial.println(menu[menu_handle.indx].down);
      goto_tag(menu[menu_handle.indx].down);
      Serial.println(menu_handle.indx);
      update_disp = true;
      break;
    case 2: 
      return_tag();
      Serial.println(menu_handle.indx);
      update_disp = true;
      break;  
  }
  return update_disp;

}

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


char *get_menu_name(void )
{
  return menu[menu_handle.indx].name;  
}


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

void nop(int x)
{
    Serial.println(x);
}
