#include <Wire.h>     //lcd
#include "rgb_lcd.h"  //lcd
#include <Keypad.h>   //keypad
//#include <webpage.h>  //webpage

rgb_lcd lcd; //lcd

#define ROW 4     //keypad
#define COLUMN 3  //keypad
#define BUZZ 32   //buzzer

int passCode[4] = {1, 2, 3, 4}; //keypad
int passCodeEntered[4];         //keypad
int passCodeWrong = 0;          //keypad
char keys[ROW][COLUMN] =  {     //keypad
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW] = {2, 0, 4, 16}; //keypad
byte pin_column[COLUMN] = {17, 5, 18};  //keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW, COLUMN);  //keypad

void setup() 
{
  Serial.begin(115200);

  Wire.begin(19, 23); //lcd
  lcd.begin(16, 2);   //lcd

  pinMode(BUZZ, OUTPUT);  //buzzer 

  lcd.print("Hello ESP32!"); //lcd, test code
}

void loop() 
{
  lcd.clear();
  keyPad();
}

void keyPad() //keypad function
{
  int keyPress = 0;
  char key = keypad.getKey();

  lcd.println("Enter Code\n");
  if(key)
  {
    Serial.print(key); //keypad, test code
    lcd.print(key); //keypad, print text on lcdscreen

    passCodeEntered[keyPress] = key - '0';
    keyPress++;
  }

  if (keyPress == 4)
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
      lcd.clear();
      lcd.print("Correct");
      Serial.println("Correct");
    }
    else 
    {
      buzzer();
    }

    keyPress = 0;
    lcd.clear();
  }
}

void buzzer() //buzzer function
{
  if(passCodeWrong > 0)
  {
    lcd.clear();
    lcd.print("Incorrect pass code! Alarm on");    //lcd
    Serial.print("Incorrect pass code! Alarm on"); //lcd, test code
    for(int i = 0; i < 40; i++)
    {
      digitalWrite(BUZZ, HIGH);
      delay(250);
      digitalWrite(BUZZ, LOW);
      delay(250);
    }
  }
}