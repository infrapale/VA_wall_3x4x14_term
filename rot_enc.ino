#include "rotenc.h"
#include "Pin_Button.h"

// Digital pin definitions
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
  char name[10];
  pMenuFunc func;
  int param;
};

struct MenuPtrStruct {
    uint16_t first;
    uint16_t last;
    uint16_t nbr;
    uint16_t indx;
    
} menu_handle;

uint16_t menu_stack[MENU_STACK_DEPTH];
uint8_t menu_stack_ptr = 0;

void init_rot_enc(void){
    rotenc_init(dpInEncoderA, dpInEncoderB, dpInEncoderC);
    rotenc_set_step(0, 0, 10, 1);
    butn[0].Init(dpInButtnOn,'1');
    butn[1].Init(dpInButtnOff,'0');
    butn[2].Init(dpInButtnRet,'R');
    butn[3].Init(dpInButtnFree,'X');
    menu_stack[0] = 0;
    
    menu[0].func(menu[0].param);

}

uint8_t push_menu_stack(uint16_t m_indx){
    if(menu_stack_ptr < MENU_STACK_DEPTH-1){
        menu_stack[menu_stack_ptr] = m_indx;
        menu_stack_ptr++;
        return(menu_stack_ptr);
    } else
    {
        return(-1);
    }   
}
/*
 * pop_menu_stack
 * !! menu_stack[0] = <topmost menu index = 0>
 */
uint8_t pop_menu_stack(void){
    if(menu_stack_ptr > 0){
        menu_stack_ptr--;
    }  
    return(menu_stack[menu_stack_ptr]);
}

void nop(int x)
{
    Serial.println(x);
}

struct MenuEntryStruct menu[] = {
  //  tag   down name        func param
    {"Top","MH1","MH1      ", &nop,69},
    {"Top","MH2","MH2      ", &nop,42},
    {"Top","TUP","Tupa     ", &nop,42},
    {"---","---","         ", &nop, 0},
    {"MH1","---","MH1 1    ", &nop,69},
    {"MH1","---","MH1 2    ", &nop,69},
    {"MH1","---","MH1 3    ", &nop,69},
    {"---","---","         ", &nop, 0},
    {"MH2","---","MH2 1    ", &nop,69},
    {"MH2","---","MH2 2    ", &nop,69},
    {"---","---","         ", &nop, 0},
    {"TUP","---","Tupa 1   ", &nop,69},
    {"TUP","---","Tupa 2   ", &nop,69},
    {"---","---","         ", &nop, 0},
    {"!!!","---","         ", &nop, 0}    
};

/**
* Real-time functions
*/
TaHa taha_btn_scan;

int position;

uint16_t find_menu_tag( MenuEntryStruct *menu, MenuPtrStruct *m_handle, char *tag){
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

void scan_btn(void)
{
    for(int i= 0;i<MAX_BTN; i++){
        butn[i].Scan();
    }    
}
