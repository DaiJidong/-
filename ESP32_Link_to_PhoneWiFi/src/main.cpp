#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(9600);
  WiFi.begin("芜湖","1357986420");
  while(WiFi.status()!=WL_CONNECTED)
    {
      delay(500);
    }
  Serial.print("IP地址是：");
  Serial.println(WiFi.localIP());
}

void loop() {

}


