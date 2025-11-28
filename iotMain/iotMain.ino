#include <Wire.h>           //lcd
#include "rgb_lcd.h"        //lcd
#include <Keypad.h>         //keypad
#include <SPI.h>            //rfid
#include <MFRC522.h>        //rfid
#include <webpage.h>        //webpage
#include <camera.h>         //webpage
#include <keypadWeb.h>      //webpage   
#include <location.h>       //webpage
#include <WiFi.h>           //webserver
#include <ESPmDNS.h>        //webserver
#include <WebServer.h>      //webserver

#define ROW 4     //keypad
#define COLUMN 3  //keypad
#define BUZZ 32   //buzzer
#define SS_RFID   21 //rfid
#define RST_RFID  22 //rfid
#define SCK_RFID  25 //rfid
#define MOSI_RFID 26 //rfid  
#define MISO_RFID 27 //rfid

rgb_lcd lcd; //lcd

const char* ssid = "Backup";          //wifi
const char* password = "nonono12345"; //wifi
const int TRIG = 14;  //distance
const int ECHO = 34;  //distance
unsigned long lastDistance = 0; //distance
long dura;                      //distance
float dist;                     //distance
int distanceCheck = 20;         //distance
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
int card = 0;         //card scanned - 0-Not scanned / 1-Matches / 2-No Match
int unlock = 0;       //card scanned - 0-Not scanned / 1-Card / 2-Fob 

MFRC522 rfid(SS_RFID, RST_RFID);  //rfid

byte pin_rows[ROW] = {2, 0, 4, 16};             //keypad
byte pin_column[COLUMN] = {17, 5, 18};          //keypad
byte unlockCard[4] = {0x65, 0x74, 0x4D, 0x05};  //rfid - hex code for saved card
byte unlockFob[4] = {0x26, 0xF4, 0xAF, 0x01};   //rfid - hex code for saved key fob

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW, COLUMN);  //keypad

WebServer server(80); //webserver

void setup() {
  Serial.begin(115200);

  Wire.begin(19, 23); //lcd
  lcd.begin(16, 2);   //lcd
  SPI.begin(SCK_RFID, MISO_RFID, MOSI_RFID, SS_RFID); //rfid

  WiFi.mode(WIFI_STA);        //webserver
  WiFi.begin(ssid, password); //webserver
  Serial.println("");         //webserver

  //all below is related to webserver
  //tells you if its connected or not and shows the IP address
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);                 //html - webpage.h file 
  server.on("/cameraWeb", handleCamera);      //html - camera.h file
  server.on("/locationWeb", handleLocation);  //html - location.h file
  server.on("/keypadWeb", handleKeypad);      //html - keypadWeb.h file
  server.on("/inline", [&server]() {
  server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  pinMode(BUZZ, OUTPUT);  //buzzer 
  pinMode(TRIG, OUTPUT);  //distance
  pinMode(ECHO, INPUT);   //distance

  rfid.PCD_Init();  //rfid

  lcd.print("Loading"); //little loading bar printed to lcd
  for (int l = 0; l < 9; l++) {
    lcd.print(".");
    delay(250);
  }
}

void loop() {
  server.handleClient();
  delay(2);
  keyPad();
  distance();
}

void keyPad() { //keypad function
  char key = keypad.getKey();

  if (keyIndex == 0 && prompt) {
    lcd.clear();

    if (alarmStatus == 0) { 
      lcd.print("Alarm Disabled #"); 
    }
    else { 
      lcd.print("Alarm Active   #"); 
    }

    delay(500);
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
    if (key == '#') {
      delay(500);
      rfidFunction();
      return;
    }

    if (keyIndex == 4) {
      checkCode();
      keyIndex = 0;
      prompt = true;
      delay(500);
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

  if (passCodeWrong > 0 && alarmStatus == 1) {
    atempt--;
  }

  if (passCodeWrong == 0) {
    delay(1000);
    lcd.clear();
    lcd.print("Correct.");
    delay(2500);
    atempt = 3;
    if (alarmStatus == 1) {
      alarmStatus = 0;
    }
    else {
      alarmStatus = 1;
    } 
  }
  else if (passCodeWrong > 0 && atempt > 0 && alarmStatus == 0) {
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print("Try Again.");
    delay(2500);
    alarmStatus = 0;
  }
  else if (passCodeWrong > 0 && atempt == 0) {
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print("Sounding Alarm!");   
    buzzer();
  }
  else
  {
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print(atempt);
    lcd.print(" Attempts Left");
    delay(2500);
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
  keyIndex = 0;
  alarmStatus = 1;
  prompt = true;
}

void distance() { //distance function
  unsigned long currentMillis = millis(); 

  if (currentMillis - lastDistance >= 200) {
    lastDistance = currentMillis;

    digitalWrite(TRIG, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    dura = pulseIn(ECHO, HIGH, 20000); //timeout after 20ms
    dist = (dura * 0.0343) / 2; //convert to cm

    if (dura == 0) return;
    if (dist > 0 && dist < distanceCheck) {
      if (alarmStatus == 1) {
        lcd.clear();
        lcd.print("Movement!");
        lcd.setCursor(0, 1);
        lcd.print("Sounding Alarm.");
        buzzer();
      }
    }
  }
}

void rfidFunction() { //rfid card sacnner function
  lcd.clear();
  lcd.print("Scan Card.");
  lcd.setCursor(0, 1);

  while (!rfid.PICC_IsNewCardPresent()) { delay(10); } 
  while (!rfid.PICC_ReadCardSerial()) { delay(10); }

  bool isCard = (memcmp(rfid.uid.uidByte, unlockCard, 4) == 0);
  bool isFob = (memcmp(rfid.uid.uidByte, unlockFob, 4) == 0);
    
  if (isCard || isFob) {
    lcd.print("Key Matches");
    card = 1;
    if (alarmStatus == 1) {
      alarmStatus = 0;
    }
    else {
      alarmStatus = 1;
    } 

    if (isCard) {
      unlock = 1;
    }
    else { 
      unlock = 2;
    }
  }
  else {
    lcd.print("No Key Match");
    card = 2;
    unlock = 3;
  }

  
  delay(2500);
  lcd.clear();
  keyIndex = 0;
  prompt = true;

  return;
}

void handleRoot() { //function to send information to the websight 
  String serverMessage = homepage1 
                       + serverMessageBox() 
                       + homepage2 
                       + distanceToString() 
                       + homepage3 
                       + distanceMessageBox() 
                       + homepage4 
                       + rfidMessageBox() 
                       + homepage5;
  server.send(200, "text/html", serverMessage);
}

void handleNotFound() { //if you cant connect the server will send this error 404 message 
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleCamera() { //loads the camera html file
  server.send(200, "text/html", CAMERA_HTML);
}

void handleKeypad() { //loads the keypad html file
  server.send(200, "text/html", KEYPAD_HTML);
}

void handleLocation() { //loads the location html file
  server.send(200, "text/html", LOCATION_HTML);
}

float distanceRead() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  dura = pulseIn(ECHO, HIGH, 20000); //timeout after 20ms
  dist = (dura * 0.0343) / 2; //convert to cm

  return dist;
}

String distanceToString() { //converts the distance reading to a string so it can be displayed on the websight
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  dura = pulseIn(ECHO, HIGH, 20000); //timeout after 20ms
  dist = (dura * 0.0343) / 2; //convert to cm

  if (dura == 0) { return "No Reading"; }
  else { return String(dist) + " cm"; }
}

String serverMessageBox() { 
  String serMes = "Error";

  if (alarmStatus == 1) {
    serMes = "Alamra Status<br>-ACTIVE-";
  }
  else {
    serMes = "Alamra Status<br>-DISABLED-";
  }

  return serMes; 
}

String distanceMessageBox() { 
  int d = distanceRead();
  String disMes = "Error";

  if (d > 20) {
    disMes = "NO MOVEMENT";
  }
  else {
    disMes = "MOVEMENT\nDETECTED";
  }

  return disMes; 
}

String rfidMessageBox() { 
  String cardMes = "Error";  

  switch (card) {
    case 0: cardMes = "NO SCAN"; break;
    case 1:
      switch (unlock) {
        case 1: cardMes = "KEY CARD"; break;
        case 2: cardMes = "KEY FOB"; break;
        default: cardMes = "Switch Error";
      } break;
    case 2: cardMes = "NO MATCH"; break;
    default: cardMes = "Switch Error";
  }
  
  return cardMes; 
}