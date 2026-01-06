#include <Adafruit_GPS.h>
#include <HardwareSerial.h>

#define GPSSerial Serial2
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false

uint32_t timer = millis();


void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 17, 16);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);
}

void loop() // run over and over again
{
  GPS.read();
  
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) 
      return; 
  }

  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
    }
  }
}