#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

void setup() {
  Wire.begin(19, 23);
  lcd.begin(16, 2);
  lcd.print("Hello ESP32!");
}

void loop() {

}
