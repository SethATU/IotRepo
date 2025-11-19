#include <Wire.h>     //lcd
#include "rgb_lcd.h"  //lcd
#include <Keypad.h>   //keypad
//#include <webpage.h>  //webpage

#define ROW 4     //keypad
#define COLUMN 3  //keypad
#define BUZZ 32   //buzzer

rgb_lcd lcd; //lcd

const int TRIG = 14;  //distance
const int ECHO = 34;  //distance

long dura;                      //distance
float dist;                     //distance
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
  pinMode(TRIG, OUTPUT);  //distance
  pinMode(ECHO, INPUT);   //distance

  lcd.print("Hello ESP32!"); //lcd, test code
  delay(1000);
  lcd.clear();
}

void loop() 
{
  keyPad();
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

void distance() //distance function
{
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