#include <Arduino.h>
#define LED_pin 4
void setup(){
pinMode(LED_pin,OUTPUT);
}
void loop(){
digitalWrite(LED_pin,HIGH);
delay(1000);
digitalWrite(LED_pin,LOW);
delay(1000);
}