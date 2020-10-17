#define NBR_COLLECTED_SENSORS 3
// {"Z":"Dock","S":"P_bmp180","V":997.00,"R":""}

struct sensor_entry {
    String zone;
    String sensor;
    float value;  
};

typedef struct sensor_entry sensor_info;
extern float sensor_value[];

sensor_info collect_sens[NBR_COLLECTED_SENSORS] = {
    {"Tupa","Temp",24.2},
    {"OD_1","Temp",22.1},
    {"Dock","T_Water",16.0}
};

void test_sens_db(void){
    Serial.println("Test sens_db");
    for( int i = 0; i < NBR_COLLECTED_SENSORS; i++){          
        Serial.print(collect_sens[i].zone);
        Serial.print(" - ");
        Serial.print(collect_sens[i].sensor);
        Serial.print(" - ");
        Serial.print(collect_sens[i].value);
        Serial.println();
    }
}
#define MSG_ATTRIBUTES 4
void parse_msg(char *rad_msg){
   int attr_pos[MSG_ATTRIBUTES];
   int attr_end[MSG_ATTRIBUTES];
   String attributes[MSG_ATTRIBUTES];
   String msg_str;
   uint8_t i;

   
   msg_str = rad_msg;
   Serial.print("String= "); Serial.println(msg_str);
   attr_pos[0] = msg_str.indexOf("{\"Z\":") + 5;
   attr_pos[1] = msg_str.indexOf(",\"S\":") + 5;
   attr_pos[2] = msg_str.indexOf(",\"V\":") + 5;
   attr_pos[3] = msg_str.indexOf(",\"R\":") + 5;
   boolean attr_found = true;
   Serial.println("Search Attributes");
   for(int i = 0; i<MSG_ATTRIBUTES;i++){
      if(attr_pos[i] < 0){
        attr_found = false;
        Serial.println("Attribute[i] not found");
      } 
   }
   if (attr_found){
     for(int i = 0; i < MSG_ATTRIBUTES;i++){
       int end_pos = -1;
       Serial.println(i);
       if (msg_str.charAt(attr_pos[i]) == '\"'){
         attr_pos[i]++;
         end_pos = msg_str.indexOf('\"',attr_pos[i]);
       }  
       else {  //not a string value
         end_pos = msg_str.indexOf(',',attr_pos[i]);
         if (end_pos < 0) end_pos = msg_str.indexOf('}',attr_pos[i]);
       }
       if (end_pos < attr_pos[i]){
         attr_found = false;
         Serial.println("missing double qoute");
       } else {
         attributes[i] = msg_str.substring(attr_pos[i],end_pos);
         Serial.print(i); Serial.print(" -- ");
         Serial.println(attributes[i]);
       }
     }
     
   }
   else {
     Serial.println("Missing JSON tags");
   }   

   int match_indx = -1;
   if (attr_found){
       for (int i = 0; (i < NBR_COLLECTED_SENSORS) && (match_indx < 0); i++){
           if ((attributes[0] == collect_sens[i].zone) &&
               (attributes[1] == collect_sens[i].sensor))
           {
               match_indx = i;
           }
       }
       if (match_indx >= 0)
       {
           Serial.print("Sensor match: ");
           Serial.print(collect_sens[match_indx].zone); Serial.print(" ");
           Serial.print(collect_sens[match_indx].sensor); Serial.print(" = ");
           Serial.print(attributes[2]);
           if  (match_indx < 3) {
               sensor_value[match_indx] = attributes[2].toFloat();
           }    
       }
     for (i = 0; i < 3; i++)
     {
        attributes[i].toUpperCase();
        attributes[i].toCharArray(disp_buf[RADIO_MSG_STATE][i],5);
     }
     disp_set_state(RADIO_MSG_STATE);
     disp_set_time_out(5);
     //disp_buf[d_st][0]

   }
   //Serial.println(msg_str.indexOf("{\"Z\":"));
   //Serial.println(msg_str.indexOf("\"S\":"));
   //Serial.println(msg_str.indexOf("\"V\":"));
   //Serial.println(msg_str.indexOf("\"R\":"));
   //Serial.println(msg_str.indexOf("Dock"));
   //Serial.println(msg_str.indexOf("OD_1"));
   
   //NBR_COLLECTED_SENSORS
}
