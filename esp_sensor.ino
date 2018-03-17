#include "DHT.h" 
#include <Wire.h>
#include <BH1750.h>

#define DHTPIN0 D1
#define DHTPIN1 D2
BH1750 lightMeter;
#define DHTTYPE DHT22         // DHT 22  (AM2302)

unsigned long last_sensor_time = 0;
unsigned long sensor_time = 10000;
DHT dht0(DHTPIN0, DHTTYPE); 
DHT dht1(DHTPIN1, DHTTYPE); 

void setup() {
   Serial.begin(9600); //setup baurate
   Wire.begin(D4,D5);
   lightMeter.begin();
   dht0.begin();
   dht1.begin();
}

void loop() {
  
    delay(2000);
    uint16_t lux = lightMeter.readLightLevel();
    int h0= dht0.readHumidity();
    int t0= dht0.readTemperature();
    int h1= dht1.readHumidity();
    int t1= dht1.readTemperature();
     delay(10000);
    if (isnan(h0) || isnan(t0) || isnan(h1) || isnan(t1)) {
      Serial.println("Failed");
      return;
    }  
    Serial.print(lux);
    Serial.print(",");
    Serial.print(t0);
    Serial.print(",");
    Serial.print(h0);
    Serial.print(",");
    Serial.print(t1);
    Serial.print(",");
    Serial.print(h1);
    Serial.print("\n");
    delay(10000);
}
