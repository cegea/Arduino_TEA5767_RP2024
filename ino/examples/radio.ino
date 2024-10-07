/**
 ********************************************************************************
 * @file    radio.ino
 * @author  Carlos Egea
 * @date    06/06/2023
 * @brief   Example for I2C TEA5767 radio module.
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include <tea5767_i2c.h>

tea5767_i2c radio(EU_BAND);

void setup(){    
    radio.begin();
    Serial.begin(9600);
    radio.tea5767_setStation(92.4);
}
    
void loop(){
  float freq;

  while(Serial.available()){
    freq = Serial.readStringUntil('\n').toFloat();
    radio.tea5767_setStation(freq);
    Serial.print("\nFrequency: ");
    Serial.println(radio.tea5767_getStation());
  }
}
