#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoBLE.h>
#include "ThingSpeak.h"
const unsigned long channel_id = 1632294;
const char write_api_key[] = "TH5F3F0TNTNCK8ZG";
const char ssid[] = "TALKTALKB1CED7";
const char pass[] = "KGH8HD6X";
WiFiClient client;
BLEService doorService("180F");
BLEIntCharacteristic doorCheck("2A19", BLERead | BLENotify);
long previousMillis = 0;

void setup() {
  
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(5000);
  }

  ThingSpeak.begin(client);

  if (!BLE.begin()) {
    while (1);
  }

  BLE.setLocalName("DoorDevice");
  BLE.setAdvertisedService(doorService);
  doorService.addCharacteristic(doorCheck);
  BLE.addService(doorService);
  doorCheck.writeValue(0);
  BLE.advertise();
  
}

void loop() {

  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
      long currentMillis = millis();
      if (currentMillis - previousMillis >= 20000) {
        previousMillis = currentMillis;
        int digitalVal = digitalRead(2);
        if(HIGH == digitalVal)
        {
          doorCheck.writeValue(0);
          doorCheck.broadcast();
          ThingSpeak.setField(1, 0);
          ThingSpeak.writeFields(channel_id, write_api_key);
        }
        else
        {
          doorCheck.writeValue(1);
          doorCheck.broadcast();
          ThingSpeak.setField(1, 1);
          ThingSpeak.writeFields(channel_id, write_api_key);
        }
        
      }
    }
  }
  else{
    long currentMillis = millis();
    if (currentMillis - previousMillis >= 20000) {
       previousMillis = currentMillis;
       int digitalVal = digitalRead(2);
       if(HIGH == digitalVal)
       {
         ThingSpeak.setField(1, 0);
         ThingSpeak.writeFields(channel_id, write_api_key);
       }
       else
       {
         ThingSpeak.setField(1, 1);
         ThingSpeak.writeFields(channel_id, write_api_key);
       }      
    }
  } 
}
