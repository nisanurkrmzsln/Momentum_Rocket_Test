#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEA_LEVEL_PRESSURE 102550

Adafruit_BME280 bme; 

float altitude, pressure, temperature, humidity, first_alt, max_alt = 0; // for BME280 
bool dragParachuteOpen = false, mainParachuteOpen = false;

typedef struct {
  float altitude;
  float pressure;
  float temperature;
  float humidity;
} Signal;
Signal data;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if(!bme.begin()) {
    Serial.println("BME280 connection failed...!");
    while(1) {
      delay(10);
    }
  }
  else {
    first_alt = bme.readAltitude(SEA_LEVEL_PRESSURE);
  }
  
}

void loop() {
  read_altitude();
  read_pressure();
  read_temperature();
  read_humidity();

  if(!dragParachuteOpen){
    if(altitude>max_alt){
      max_alt=altitude;
    }
    else if(max_alt>altitude){
      Serial.println("Alçalma Başladı.");
      dragParachute();
    }
  }
  else if(dragParachuteOpen=true && !mainParachuteOpen){
    if(400<altitude && altitude<600){
      mainParachute();
    }
  }
}

void dragParachute(){
  Serial.println(F("Suruklenme Parasutu Acildi."));
  dragParachuteOpen = true;
}

void mainParachute(){
  Serial.println(F("Ana Parasut Acildi."));
  mainParachuteOpen= true;
}

// ------ BME280 functions ----- 
void read_altitude() {
  altitude = bme.readAltitude(SEA_LEVEL_PRESSURE);
  altitude = altitude - first_alt;
  data.altitude=altitude;
}
void read_pressure(){
  pressure=bme.readPressure();
  data.pressure=pressure;
}
void read_temperature(){
  temperature=bme.readTemperature();
  data.temperature=temperature;
}
void read_humidity(){
  humidity=bme.readHumidity();
  data.humidity=humidity;
}





