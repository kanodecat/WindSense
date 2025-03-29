#include <DHT.h>

//#define DHTTYPE DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const int HEAT_INDEX_START_FAN = 76; // degrees
const int INCREASE_SPEED_AT_TEMP = 10; // interval of 10 degrees

const int motorPin = 9;
 
void setup() {
  Serial.begin(9600);
  
  pinMode(motorPin, OUTPUT);
  dht.begin();

  Serial.println("Enter speed (0 to 255) or type 'AUTO' for temperature control:");
}


void loop() {
  float temp = dht.readTemperature(true); // Fahrenheit
  float humidity = dht.readHumidity();
  if (isnan(temp) || isnan(humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float heat_index = dht.computeHeatIndex(temp, humidity);

  int speed = 0;
  if (Serial.available()) {
    //speed = constrain(speed, 0, 255);
    //analogWrite(motorPin, speed);

    if (heat_index >= HEAT_INDEX_START_FAN) {
      int high_temp = HEAT_INDEX_START_FAN + INCREASE_SPEED_AT_TEMP * 3;
      if (heat_index >= high_temp) {
        speed = 255;
      } else if (heat_index >= high_temp - INCREASE_SPEED_AT_TEMP) {
        speed = 180;
      } else {
        speed = 105;
      }
    }
    //speed = map(heat_index, 75, 105, 0, 255); // sets 0% 33% 66% 100%
    speed = constrain(speed, 0, 255);
    analogWrite(motorPin, speed);
  }



  Serial.print(temp);
  Serial.print(" °C, Speed: ");
  Serial.println(speed);

  delay(2000);
} 