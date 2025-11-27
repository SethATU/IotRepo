#include <Wire.h>     //lcd
#include "rgb_lcd.h"  //lcd
#include <Keypad.h>   //keypad
#include <SPI.h>      //rfid
#include <MFRC522.h>  //rfid
//#include <webpage.h>  //webpage

#define ROW 4     //keypad
#define COLUMN 3  //keypad
#define BUZZ 32   //buzzer
#define SS_RFID   21 //rfid
#define RST_RFID  22 //rfid
#define SCK_RFID  25 //rfid
#define MOSI_RFID 26 //rfid  
#define MISO_RFID 27 //rfid

rgb_lcd lcd; //lcd

const int TRIG = 14;  //distance
const int ECHO = 34;  //distance

long dura;                      //distance
float dist;                     //distance
int passCode[4] = {1, 2, 3, 4}; //keypad
int passCodeEntered[4];         //keypad
int passCodeWrong = 0;          //keypad
int keyIndex = 0;               //keypad
int atempt = 3;                 //keypad
bool prompt = true;             //keypad
char keys[ROW][COLUMN] =  {     //keypad
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
int alarmStatus = 0;  //alarm - 0-off / 1-on

MFRC522 rfid(SS_RFID, RST_RFID);  //rfid

byte pin_rows[ROW] = {2, 0, 4, 16};             //keypad
byte pin_column[COLUMN] = {17, 5, 18};          //keypad
byte unlockCard[4] = {0x65, 0x74, 0x4D, 0x05};  //rfid - hex code for saved card

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW, COLUMN);  //keypad

void setup() {
  Serial.begin(115200);

  Wire.begin(19, 23); //lcd
  lcd.begin(16, 2);   //lcd
  SPI.begin(SCK_RFID, MISO_RFID, MOSI_RFID, SS_RFID); //rfid

  pinMode(BUZZ, OUTPUT);  //buzzer 
  pinMode(TRIG, OUTPUT);  //distance
  pinMode(ECHO, INPUT);   //distance

  rfid.PCD_Init();  //rfid

  lcd.print("Loading");
  for (int l = 0; l < 9; l++)
  {
    lcd.print(".");
    delay(1000);
  }
}

void loop() {
  keyPad();
}

void keyPad() { //keypad function
  card = 0;
  char key = keypad.getKey();

  if (keyIndex == 0 && prompt) {
    lcd.clear();

    if (alarmStatus == 0) { 
      lcd.print("Alarm Disabled."); 
    }
    else { 
      lcd.print("Alarm Active."); 
    }

    delay(2500);
    lcd.setCursor(0, 1);
    lcd.print("Enter Code: ");  
    prompt = false;
  }
  
  if(key) {
    if (key >= '0' && key <= '9') {
      lcd.print(key); //keypad, print text on lcdscreen
      passCodeEntered[keyIndex] = key - '0';
      keyIndex++;
    }
    if (key == '#')
    {
      rfidFunction();
      return;
    }

    if (keyIndex == 4) {
      checkCode();
      keyIndex = 0;
      prompt = true;
      delay(1500);
      lcd.clear();
    }
  }
}

void checkCode() {  //checks the pass code is correct 
  passCodeWrong = 0;

  for(int i = 0; i < 4; i++) {
    if (passCodeEntered[i] != passCode[i]) {
      passCodeWrong++;
    }
  }

  if (passCodeWrong > 0 && alarmStatus == 1)
  {
    atempt--;
  }

  if (passCodeWrong == 0) {
    delay(1000);
    lcd.clear();
    lcd.print("Correct.");
    delay(5000);
    atempt = 3;
    if (alarmStatus == 1)
    {
      alarmStatus = 0;
    }
    else 
    {
      alarmStatus = 1;
    } 
  }
  else if (passCodeWrong > 0 && atempt > 0 && alarmStatus == 0) {
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print("Try Again.");
    delay(5000);
    alarmStatus = 0;
  }
  else if (passCodeWrong > 0 && atempt == 0) {
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print("Sounding Alarm!");
    delay(5000);    
    buzzer();
    alarmStatus = 1;
  }
  else
  {
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print(atempt);
    lcd.print(" Attempts Left");
    delay(5000);
  }
}

void buzzer() { //buzzer function
  for(int i = 0; i < 40; i++) {
    digitalWrite(BUZZ, HIGH);
    delay(250);
    digitalWrite(BUZZ, LOW);
    delay(250);
  }
  lcd.clear();
  atempt = 1;
}

void distance() { //distance function
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  dura = pulseIn(ECHO, HIGH);
  dist = (dura * 0.0343) / 2; //convert to cm

  Serial.print("Dist:"); 
  Serial.print(dist);
  Serial.print("Cm\n");
  
  delay(1000);
}

void rfidFunction() {
  lcd.clear();
  lcd.print("Scan Card.");
  lcd.setCursor(0, 1);

  while (!rfid.PICC_IsNewCardPresent()) { delay(10); } 
  while (!rfid.PICC_ReadCardSerial()) { delay(10); }

  if (memcmp(rfid.uid.uidByte, unlockCard, 4) == 0) {
    lcd.print("Card Matches");
    if (alarmStatus == 1)
    {
      alarmStatus = 0;
    }
    else 
    {
      alarmStatus = 1;
    } 
  }
  else {
    lcd.print("Crad does not match");
  }
  
  delay(5000);
  lcd.clear();
  keyIndex = 0;
  prompt = true;

  return;
}