/*String JsonRelayString(const char *zone, const char *addr, const char *value, const char *remark ){
    String JsonString; 
    JsonString = "{\"Z\":\"";
    JsonString += zone;
    JsonString += "\",";
    JsonString += "\"S\":\"";
    JsonString += addr;  
    JsonString += "\",";
    JsonString += "\"V\":\"";
    JsonString += value;
    JsonString += "\",";
    JsonString += "\"R\":\"";
    JsonString += remark;
    JsonString += "\"}";

    return(JsonString);
}
*/

uint8_t json_char_array(char *p, const uint8_t max_len, const char *zone, const char *addr, const char *value, const char *remark) {
     if (max_len >= strlen(zone)+strlen(addr)+strlen(value)+strlen(remark)){
       strcpy(p,"{\"Z\":\"");
       strcat(p, zone);
       strcat(p,"\",");
       strcat(p,"\"S\":\"");
       strcat(p,addr);
       strcat(p, "\",");
       strcat(p,"\"V\":\"");
       strcat(p,value);
       strcat(p,"\",");
       strcat(p,"\"R\":\"");
       strcat(p,remark);
       strcat(p,"\"}");
       return strlen(p);
     }
     else
     {
       return 0;
     }
  
}
