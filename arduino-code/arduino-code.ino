#include <DHT.h>

//#define DHTTYPE DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const int HEAT_INDEX_START_FAN = 75; // degrees
const int INCREASE_SPEED_AT_TEMP = 10; // interval of 10 degrees

const int motorPin = 9;
 
void setup() {
  Serial.begin(9600);
  
  pinMode(motorPin, OUTPUT);
  dht.begin();

  Serial.println("Enter speed (0 to 255) or type 'AUTO' for temperature control:");
}


void loop() {
  int speed;

  //String detection_message = Serial.readStringUntil('\n');
  //detection_message.trim();
  //if(detection_message == "PERSON DETECTED") {
    float temp = dht.readTemperature(true); // Fahrenheit
    float humidity = dht.readHumidity();
    if (isnan(temp) || isnan(humidity)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      //return;
    }
    float heat_index = dht.computeHeatIndex(temp, humidity);
    //float heat_index = 76;
    if (Serial.available()) {
      if (heat_index >= HEAT_INDEX_START_FAN) {
        int high_temp = HEAT_INDEX_START_FAN + INCREASE_SPEED_AT_TEMP * 2;
        Serial.println(high_temp);
        Serial.println("hello");

        if (heat_index >= high_temp) {
          speed = 255;
        } else if (heat_index >= (high_temp - INCREASE_SPEED_AT_TEMP)) {
          speed = 180;
        } else {
          Serial.print((high_temp - INCREASE_SPEED_AT_TEMP));
          speed = 105;
        }
      }
    speed = constrain(speed, 0, 255);
    analogWrite(motorPin, speed);
  }



    Serial.print(heat_index);
    Serial.print(" °C, Speed: ");
    Serial.println(speed);

    delay(2000);
  //}
} 