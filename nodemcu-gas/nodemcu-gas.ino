#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>


const char *ssid =  "Home 3R";     // replace with your wifi ssid and wpa2 key
const char *pass =  "jahe merah";

#define FIREBASE_HOST "db-iot-e8e04-default-rtdb.firebaseio.com/"                    
#define FIREBASE_AUTH "PKoCmZfK1SpLsfdfEvSk4O3nKdrQNEx6ischXGhm"   

FirebaseData firebaseData;
WiFiClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
 WiFi.begin(ssid, pass); 
 while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");

 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
 Firebase.reconnectWiFi(true);
}

void loop() {
  if (Serial.available()) {
    Serial.write(Serial.read());
  }
  String x = (String)Serial.read();
  
  float t = 1;
  if (Firebase.setString(firebaseData, "/DataSensors/datas", x))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}
