#include <NTPClient.h>
#include <ArduinoJson.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid =  "OPPO A3s";
const char *pass =  "pramuka22";

#define FIREBASE_HOST "db-iot-e8e04-default-rtdb.firebaseio.com/"                    
#define FIREBASE_AUTH "PKoCmZfK1SpLsfdfEvSk4O3nKdrQNEx6ischXGhm"  

// NTP Config 
#define NTP_OFFSET   7 * 60 * 60   // In seconds
#define NTP_SERVER "id.pool.ntp.org"

// Firebase
FirebaseData firebaseData;

// Wifi Client
WiFiClient client;

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET);

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("-");
  }
   timeClient.begin();
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.print(WiFi.localIP());
   Serial.println("");
   
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
   Firebase.reconnectWiFi(true);
   
}

void loop() {
  
  timeClient.update();
  
  // Get timestamp
  unsigned long epochTime = timeClient.getEpochTime();
  
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  
  // Get Date
  String tanggal = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  
  if (Serial.available()) {
    
    // deserializeJson data dari serial
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc,Serial);
    if (error) {
      Serial.println(error.f_str());
      return;
    }
    
    // Parsing Data
    float jarakPandang = doc["jarakPandang"];
    float ppm_co = doc["ppm_co"];
    float ppm_co2 = doc["ppm_co2"];
    float ppm_so2 = doc["ppm_so2"];
    
    // convert data to json and save to firebase
    FirebaseJson store_firebase;
    store_firebase.set("tanggal",tanggal);
    store_firebase.set("jarak_pandang",jarakPandang);
    store_firebase.set("ppm_co",ppm_co);
    store_firebase.set("ppm_co2",ppm_co2);
    store_firebase.set("ppm_so2",ppm_so2);
    
    // Do Store Data
    if (Firebase.set(firebaseData,String(epochTime),store_firebase)) {
       Serial.println("PATH: " + firebaseData.dataPath());
       Serial.println("PASSED");
    }else{
       Serial.println("FAILED");
       Serial.println("REASON: " + firebaseData.errorReason());
    }
    
  }
  
}
