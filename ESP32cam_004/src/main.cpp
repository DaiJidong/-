#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 5  //D1

char ssid[] = "芜湖";    // your network SSID (name)
char pass[] = "1357986420";    // your network password (use for WPA, or use as key for WEP)


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[]    = "iot-06z00dr148llov7.mqtt.iothub.aliyuncs.com";
int        port        = 1883;

const char inTopic[]   = "/sys/k295cDCNxDF/esp32cam_dev/thing/service/property/set";
const char outTopic[]  = "/sys/k295cDCNxDF/esp32cam_dev/thing/event/property/post";

const long interval = 10000;
unsigned long previousMillis = 0;

int count = 0;

String inputString ="";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(4,OUTPUT);

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  mqttClient.setId("k295cDCNxDF.esp32cam_dev|securemode=2,signmethod=hmacsha256,timestamp=1737130765768|");                    //mqtt 连接客户端id
  mqttClient.setUsernamePassword("esp32cam_dev&k295cDCNxDF", "e9d53e6ea21607bfc616d930e563a36bb56cd9b63b63d50767bee5d0f03da82a");        //mqtt 连接用户名、密码

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  //mqttClient.onMessage(onMqttMessage());

  Serial.print("Subscribing to topic: ");
  Serial.println(inTopic);
  Serial.println();

  int subscribeQos = 1;

  mqttClient.subscribe(inTopic, subscribeQos);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(inTopic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(inTopic);
  Serial.println();
}

void loop() {

  mqttClient.poll();


  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    String payload;

    DHT.read(DHT11_PIN);

    //{"params":{"temp":1.22,"humi":22},"version":"1.0.0"}
  
  

    DynamicJsonDocument json_msg(512);
    DynamicJsonDocument json_data(512);

    json_data["temp"] = DHT.temperature;
    json_data["humi"] = DHT.humidity;

    json_msg["params"] = json_data;
    json_msg["version"] = "1.0.0";


    serializeJson(json_msg,payload);
    payload = "{\"params\":{\"temp\":55,\"humi\":66},\"version\":\"1.0.0\"}";
    Serial.print("Sending message to topic: ");
    Serial.println(outTopic);
    Serial.println(payload);


    bool retained = false;
    int qos = 1;
    bool dup = false;

    mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
    mqttClient.print(payload);
    mqttClient.endMessage();

    Serial.println();

    count++;
  }
}

//{"deviceType":"CustomCategory","iotId":"fwJnX0wS4z4MbrzkyRlt000000","requestId":"1689847083906","checkFailedData":{},"productKey":"a1gfbOdKycU","gmtCreate":1689847087015,"deviceName":"app_dev","items":{"led":{"value":0,"time":1689847086996}}}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', duplicate = ");
  Serial.print(mqttClient.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(mqttClient.messageQoS());
  Serial.print(", retained = ");
  Serial.print(mqttClient.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    char inChar =(char)mqttClient.read();
    inputString +=inChar;
    if(inputString.length()==messageSize) {

      DynamicJsonDocument json_msg(1024);
      DynamicJsonDocument json_item(1024);
      DynamicJsonDocument json_value(1024);

      deserializeJson(json_msg,inputString);

      String items = json_msg["items"];

      deserializeJson(json_item,items);
      String led = json_item["led"];

      deserializeJson(json_value,led);
      bool value = json_value["value"];

      if(value ==0) {
        //关
        Serial.println("off");
        digitalWrite(4,HIGH);
      } else {
        //开
        Serial.println("on");
        digitalWrite(4,LOW);
      }
      inputString="";
    }


  }
  Serial.println();

  Serial.println();
}