#include <Arduino.h>
#include<WiFi.h>
const char*ssid="ESP32AP";
const char*password="12345678";

void setup() {
  Serial.begin(9600);
  WiFi.softAP(ssid,password,1,0,4);//通道，是否隐藏，最大连接数
  Serial.print("\n WiFi接入点的IP：");
  Serial.println(WiFi.softAPIP());
}
 
void loop() {
  
}
