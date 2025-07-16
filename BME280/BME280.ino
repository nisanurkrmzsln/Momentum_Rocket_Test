#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEA_LEVEL_PRESSURE 102550 // Bu değeri sensörün ortam basıncına göre ayarlayabilirsiniz

Adafruit_BME280 bme; 

float altitude, pressure, temperature, humidity, first_alt, max_alt = 0;
float kalman_new = 0, cov_new = 0, kalman_gain = 0, kalman_calculated = 0, kalman_old = 0 , cov_old = 0; //Kalman için 
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
    else if(max_alt-15>altitude){
      Serial.println("Alçalma Başladı.");
      dragParachute();
    }
  }
  else if(dragParachuteOpen && !mainParachuteOpen){
    if(400<altitude && altitude<600){
      mainParachute();
    }
  }
  printParameters();
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
  altitude=kalman_filter(altitude);
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

void printParameters() {
  Serial.print("Altitude : "); Serial.print(altitude); Serial.print("Max Altitude : "); Serial.println(max_alt); 
  Serial.print("Pressure : "); Serial.println(pressure);
  Serial.print("Temp : "); Serial.println(temperature);
  Serial.print("Humidity : "); Serial.println(humidity);
}
float kalman_filter(float input) {
 
  kalman_new = kalman_old; 
  cov_new = cov_old + 0.50;
  kalman_gain = cov_new / (cov_new + 0.9);

  kalman_calculated = kalman_new + (kalman_gain * (input - kalman_new));

  cov_new = (1 - kalman_gain) * cov_old;
  cov_old = cov_new;

  kalman_old = kalman_calculated;
  return kalman_calculated;
}






