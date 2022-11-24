#include "UbidotsEsp32Mqtt.h"   
#define WIFI_SSID "studyroom"          
#define WIFI_PASS "studyroom"
#define UBIDOTS_TOKEN "BBFF-tA6sBL2UsR56MktBzHeEMHgJ5WKtaK" 
#define DEVICE_LABEL "coding" 
#define distance_LABEL "distance"

const int trig = 25;
const int echo = 26;

const int PUBLISH_FREQUENCY = 3000;

unsigned long timer; 

Ubidots ubidots(UBIDOTS_TOKEN);

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");


  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  Serial.begin(115200);  
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, distance_LABEL);
  }

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);
  long distance = (duration*340)/1/10000;

  if (abs(millis() - timer) > PUBLISH_FREQUENCY) {
    ubidots.add(distance_LABEL, distance);
    ubidots.publish(DEVICE_LABEL);

    if (distance <= 20) {
      Serial.println("X");
    }

    else {
      Serial.println("O");
    }

    timer = millis();
  }
  
  ubidots.loop();
}
