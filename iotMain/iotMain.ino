#include <Wire.h>     //lcd
#include "rgb_lcd.h"  //lcd
#include <Keypad.h>   //keypad

rgb_lcd lcd; //lcd

#define ROW 4     //keypad
#define COLUMN 3  //keypad
#define BUZZ 32   //buzzer

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
  delay(1000);
  lcd.clear();
}

void loop() 
{
  keyPad();
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

void checkCode()  //checks the code is correct 
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