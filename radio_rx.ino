
#include <Arduino.h>
#include <Wire.h>
#include <rfm69_support.h>
// RH_RF69_MAX_MESSAGE_LEN
char rx_buf[RH_RF69_MAX_MESSAGE_LEN];

void radio_rx_handler(void) {
    uint8_t len = radio_read_msg(rx_buf, RH_RF69_MAX_MESSAGE_LEN);
    if(len>0) {
        Serial.println(rx_buf);
        parse_msg(rx_buf);
    }
}

/*
 * {"Z":"Dock","S":"P_bmp180","V":1005.00,"R":""}
 * {"Z":"OD_1","S":"Light1","V":0.01,"R":""}

 * 
 */
