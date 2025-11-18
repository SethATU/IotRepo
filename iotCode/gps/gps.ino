#include <HardwareSerial.h>

HardwareSerial GPSSerial(1);

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 36, 13); // RX=36, TX=13
  Serial.println("GPS test start");
}

void loop() {
  while (GPSSerial.available()) {
    char c = GPSSerial.read();
    Serial.write(c);
  }
}
