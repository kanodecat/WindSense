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
  int speed;

  String detection_message = Serial.readStringUntil('\n');
  detection_message.trim();
  //if(detection_message == "PERSON DETECTED") {
    //float temp = dht.readTemperature(true); // Fahrenheit
    //float humidity = dht.readHumidity();
    //float heat_index = dht.computeHeatIndex(temp, humidity);

    /*
    if (Serial.available()) {
      if (heat_index >= HEAT_INDEX_START_FAN) {
        int high_temp = HEAT_INDEX_START_FAN + INCREASE_SPEED_AT_TEMP * 2;
        if (heat_index >= high_temp) {
          speed = 255;
        } else if (heat_index >= (high_temp - INCREASE_SPEED_AT_TEMP)) {
          speed = 180;
        } else {
          speed = 105;
        }
      }
    */
    speed = 255;
    speed = constrain(speed, 0, 255);
    analogWrite(motorPin, speed);
    //}

    delay(2000);
  //}
} 