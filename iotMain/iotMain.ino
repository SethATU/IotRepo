<<<<<<< Updated upstream
#include <Wire.h>     //lcd
#include "rgb_lcd.h"  //lcd
#include <Keypad.h>   //keypad

rgb_lcd lcd; //lcd
=======
#include <Wire.h>      //lcd
#include "rgb_lcd.h"   //lcd
#include <Keypad.h>    //keypad
#include <SPI.h>       //rfid
#include <MFRC552.h>   //rfid
//#include <webpage.h> //webpage
>>>>>>> Stashed changes

#define ROW       4  //keypad
#define COLUMN    3  //keypad
#define BUZZ      32 //buzzer
#define SS_RFID   21 //rfid
#define RDT_RFID  22 //rfid
#define SCK_RFID  25 //rfid
#define MOSI_RFID 26 //rfid
#define MISO_RFID 27 //rfid

int passCode[4] = {1, 2, 3, 4}; //keypad
int passCodeEntered[4];         //keypad
int passCodeWrong = 0;          //keypad
int keyIndex = 0;               //keypad
bool prompt = true;             //keypad
char keys[ROW][COLUMN] =  {     //keypad
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
int alarmStatus = 0;  //alarm

MFRC522 rfid(SS_RFID, RST_RFID);  //rfid

byte pin_rows[ROW] = {2, 0, 4, 16}; //keypad
byte pin_column[COLUMN] = {17, 5, 18};  //keypad
byte nuidPICC[4]; //rfid

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW, COLUMN);  //keypad

void setup() 
{
  Serial.begin(115200);

  Wire.begin(19, 23); //lcd
  lcd.begin(16, 2);   //lcd
  SPI.begin(SCK_RFID, MISO_RFID, MOSI_RFID, SS_RFID); //rfid

  pinMode(BUZZ, OUTPUT);  //buzzer 

  rfid.PCD_Init();  

  //lcd.print("Hello ESP32!"); //lcd, test code
  //delay(1000);
  //lcd.clear();
}

void loop() {
  keyPad();
<<<<<<< Updated upstream
=======

  if(alarmStatus == 0) {
    lcd.clear();
    lcd.print("Alarm: Unarmed");
    lcd.setCursor(0, 1);
    
  }
  
  
  //alarm status - unarmed 
  //show status then option to enter code to arm alarm 
    //correct code - alarm armed 
    //wrong code - try again
  //tap card - press * to arm # to cancile 

  //alarm ststus - armed
  //show status then option to enter code to disarm arm alarm
    //pin wrong 3x alarm goes off
    //pin correct alarm unarmed 
  //tap card to dissarm alarm 

  //alarm status - aremed 
  //if distance is < 50cm
    //start 30 second timer and then buzzer goes off 
>>>>>>> Stashed changes
}

void keyPad() //keypad function
{
  char key = keypad.getKey();

  if (keyIndex == 0 && prompt)
  {
    lcd.clear();
    lcd.print("Enter Code:");  
    lcd.setCursor(0, 1);
    prompt = false;
  }
  
  if(key)
  {
    if (key >= '0' && key <= '9')
    {
      lcd.print(key); //keypad, print text on lcdscreen
      passCodeEntered[keyIndex] = key - '0';
      keyIndex++;
    }

    if (keyIndex == 4)
    {
      checkCode();
      keyIndex = 0;
      prompt = true;
      delay(1500);
      lcd.clear();
    }
  }
}

void checkCode()  //checks the pass code is correct 
{
  passCodeWrong = 0;

  for(int i = 0; i < 4; i++)
  {
    if (passCodeEntered[i] != passCode[i])
    {
      passCodeWrong++;
    }
  }

  if (passCodeWrong == 0)
  {
    delay(1000);
    lcd.clear();
    lcd.print("Correct");
  }
  else
  {
    buzzer();
  }
}

void buzzer() //buzzer function
{
  delay(1000);
  lcd.clear();
  lcd.print("Incorrect");    //lcd
  
  for(int i = 0; i < 40; i++)
  {
    digitalWrite(BUZZ, HIGH);
    delay(250);
    digitalWrite(BUZZ, LOW);
    delay(250);
  }
}