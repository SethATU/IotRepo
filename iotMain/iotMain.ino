#include <Wire.h>     //lcd
#include "rgb_lcd.h"  //lcd
#include <Keypad.h>   //keypad

rgb_lcd lcd; //lcd

#define ROW 4     //keypad
#define COLUMN 3  //keypad

char keys[ROW][COLUMN] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW] = {2, 0, 4, 16}; //keypad
byte pin_column[COLUMN] = {17, 5, 18};  //keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW, COLUMN);  //keypad

void setup() {
  Serial.begin(115200);

  Wire.begin(19, 23); //lcd
  lcd.begin(16, 2);   //lcd
  lcd.print("Hello ESP32!"); //lcd, test code
}

void loop() {
  keyPad();
}

void keyPad() {
  char key = keypad.getKey();

  if (key) {
    Serial.println(key); //keypad, test code
  }
}
