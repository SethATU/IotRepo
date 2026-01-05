#include <Wire.h>   
#include "rgb_lcd.h"
#include <Keypad.h>
#include <SPI.h>    
#include <MFRC522.h>

#define ROW 4   
#define COLUMN 3
#define SS_RFID 21
#define RST_RFID 22
#define SCK_RFID 25
#define MOSI_RFID 26
#define MISO_RFID 27

rgb_lcd lcd;
            
char keys[ROW][COLUMN] =  {    
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte pin_rows[ROW] = {2, 0, 4, 16};   
byte pin_column[COLUMN] = {17, 5, 18};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW, COLUMN);
byte unlockCard[4] = {0x65, 0x74, 0x4D, 0x05};
byte unlockFob[4] = {0x26, 0xF4, 0xAF, 0x01}; 
MFRC522 rfid(SS_RFID, RST_RFID);

int code[4] = {1, 2, 3, 4};
int check[4];
bool prompt = true;
int incorrect = 0;
int alarmState = 0;
int codeIndex = 0;
int atempt = 3;

void setup() {
  Serial.begin(115200);
  Wire.begin(19, 23);
  lcd.begin(16, 2); 
  SPI.begin(SCK_RFID, MISO_RFID, MOSI_RFID, SS_RFID);

  rfid.PCD_Init();

  lcd.print("Loading"); 
  for (int l = 0; l < 9; l++) {
    lcd.print(".");
    delay(250);
  }
  lcd.clear();
}

void loop() {
  keypadRead();
}

void keypadRead() {
  char key = keypad.getKey();
  
  if(codeIndex == 0 && prompt) {
    if(alarmState == 0) { lcd.print("Alarm Disabled #"); }
    else { lcd.print("Alarm Active   #"); }

    delay(250);
    lcd.setCursor(0, 1);
    lcd.print("Enter Code: ");  
    prompt = false;
  }

  if(key) {  
    if(key >= '0' && key <= '9') {
      lcd.print(key);
      check[codeIndex] = key - '0';
      codeIndex++;
    }
    if(codeIndex == 4) {
      codeIndex = 0;
      prompt = true;
      delay(250);
      codeCheck();
    }
  }
}

void codeCheck() {
  incorrect = 0;
  lcd.clear();

  for(int j = 0; j < 4; j++) {
    if(check[j] != code[j]) {
      incorrect++;
    }
  }
  if(incorrect > 0 && alarmState == 1) { atempt--; }

  if(incorrect == 0) {
    rfidRead();
    return;
  }
  else if(incorrect > 0 && alarmState == 0) {
    lcd.print("Incorrect");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");
    delay(2000);
  }
  else if(incorrect > 0 && alarmState == 1 && atempt > 0) {
    lcd.print("Incorrect");
    lcd.setCursor(0, 1);
    lcd.print("Attempts Left: ");
    lcd.print(atempt);
    delay(2000);
  }
  else {
    lcd.print("Incorrect");
    lcd.setCursor(0, 1);
    lcd.print("Sounding Alarm");
    delay(2000);
  }
  lcd.clear();
}

void rfidRead() {
  lcd.print("2FA / Scan Card:");
  lcd.setCursor(0, 1);
  
  while (!rfid.PICC_IsNewCardPresent()) { delay(10); }
  while (!rfid.PICC_ReadCardSerial()) { delay(10); }

  bool isCard = (memcmp(rfid.uid.uidByte, unlockCard, 4) == 0); 
  bool isFob = (memcmp(rfid.uid.uidByte, unlockFob, 4) == 0);

  if(isCard || isFob) { 
    lcd.print("Valid");
    if(alarmState == 1) { alarmState = 0; }
    else { alarmState = 1; }
  }
  else { 
    lcd.print("Not valid");
  }

  delay(2000);
  lcd.clear();
  codeIndex = 0;
  prompt = true;  
  return;
}