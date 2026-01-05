/*
 * This Arduino Nano 33 IoT code was developed by newbiely.com
 *
 * This Arduino Nano 33 IoT code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/arduino-nano-iot/arduino-nano-33-iot-dht11
 */

#include <DHT.h>
#define DHT11_PIN  2 // The Arduino Nano 33 IoT pin D31 connected to DHT11 sensor

DHT dht11(DHT11_PIN, DHT11);

void setup() {
  Serial.begin(9600);
  dht11.begin(); // initialize the DHT11 sensor
}

void loop() {
  // read humidity
  float humi  = dht11.readHumidity();
  // read temperature in Celsius
  float temperature_C = dht11.readTemperature();
  // read temperature in Fahrenheit
  float temperature_F = dht11.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(temperature_C) || isnan(temperature_F) || isnan(humi)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(temperature_C);
    Serial.print("°C  ~  ");
    Serial.print(temperature_F);
    Serial.println("°F");
  }

  // wait a 2 seconds between readings
  delay(2000);
}
