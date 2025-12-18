#include <Wire.h>           //lcd
#include "rgb_lcd.h"        //lcd
#include <Keypad.h>         //keypad
#include <SPI.h>            //rfid
#include <MFRC522.h>        //rfid
#include <webPage.h>        //webpage
//#include <camera.h>         //webpage
//#include <keypadWeb.h>      //webpage   
//#include <location.h>       //webpage
#include <WiFi.h>           //webserver
#include <ESPmDNS.h>        //webserver
#include <WebServer.h>      //webserver
#include <Adafruit_GPS.h>   //gps
#include <HardwareSerial.h> //gps

#define GPSSerial Serial2     //gps
Adafruit_GPS GPS(&GPSSerial); //gps
#define GPSECHO false         //gps

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

uint32_t timer = millis();
//bool location = false;          //gps 
const int TRIG = 14;            //distance
const int ECHO = 34;            //distance
unsigned long lastDistance = 0; //distance
long dura;                      //distance
float dist;                     //distance
int distanceCheck = 20;         //distance
unsigned long webTimer = 0;     //keypad web
int webState = 0;               //keypad web
int webIndex = 0;               //keypad web
int webPass[4];                 //keypad web
int webPassWrong = 0;           //keypad web
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

byte pin_rows[ROW] = {2, 0, 4, 13};             //keypad
byte pin_column[COLUMN] = {33, 5, 18};          //keypad
byte unlockCard[4] = {0x65, 0x74, 0x4D, 0x05};  //rfid - hex code for saved card
byte unlockFob[4] = {0x26, 0xF4, 0xAF, 0x01};   //rfid - hex code for saved key fob

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW, COLUMN);  //keypad

String webScreen = "____";  //keypad web

WebServer server(80); //webserver

void setup() {
  Serial.begin(115200);

  Wire.begin(19, 23); //lcd
  lcd.begin(16, 2);   //lcd
  SPI.begin(SCK_RFID, MISO_RFID, MOSI_RFID, SS_RFID); //rfid

  WiFi.mode(WIFI_STA);        //webserver
  WiFi.begin(ssid, password); //webserver
  Serial.println("");         //webserver

  Serial2.begin(9600, SERIAL_8N1, 17, 16);  //gps
  GPS.begin(9600);  //gps
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //gps
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);    //gps
  GPS.sendCommand(PGCMD_ANTENNA);               //gps
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);            //gps


  //all below is related to webserver
  //tells you if its connected or not and shows the IP address
  while (WiFi.status() != WL_CONNECTED) { //waiting for the wifi to connect
    delay(500);
    Serial.print(".");
  } //when the wifi is connected it prints the name of the wifi host and the IPadress to the serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);                 //html - webpage.h file 
  // server.on("/cameraWeb", handleCamera);      //html - camera.h file
  // server.on("/locationWeb", handleLocation);  //html - location.h file
  // server.on("/keypadWeb", handleKeypad);      //html - keypadWeb.h file
  server.on("/pressKey", []() {               //html - keypadWeb.h - collects the inputs from the keypad and also pastes messages to the given window 
    if (!server.hasArg("key")) {
      server.send(400, "text/plain", "Error");  
      return;
    }
    char key = server.arg("key")[0];
    keyPadWeb(key);
    server.send(200, "text/plain", webScreen);
  });
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
  gps();
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
    if (key == '#') { //#key starts the key card scanner function
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

  if (passCodeWrong > 0 && alarmStatus == 1) {  //loose an attempt if you put in the wrong passcode
    atempt--;
  }

  if (passCodeWrong == 0) { //if you put in the correct passcode it prints that you did it correct and resets atempts
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
  else if (passCodeWrong > 0 && atempt > 0 && alarmStatus == 0) { //if you have got more thank 0 attempts and the alarm is disarmed but you put in the wrong code, you get to try again
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print("Try Again.");
    delay(2500);
    alarmStatus = 0;
  }
  else if (passCodeWrong > 0 && atempt == 0) {  //if you put in the wrong code and have no more attempts the alarm will sound 
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print("Sounding Alarm!");   
    buzzer();
  }
  else { //if the alarm is armed but you have more than 0 attempts you get a warning of how many attemptys you have left 
    delay(1000);
    lcd.clear();
    lcd.print("Incorrect.");
    lcd.setCursor(0, 1);
    lcd.print(atempt);
    lcd.print(" Attempts Left");
    delay(2500);
  }
}

void buzzer() { //buzzer function - alarm rings for 20 seconds 
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
  unsigned long currentMillis = millis(); //allows the function to interup any outher functions going on 

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
      if (alarmStatus == 1) { //if the alarm is armed and the sensor seems movment under 20cm away it will sound the alarm 
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

  //checks if the code matches the fob or key card hex code and sets it to true or false 
  bool isCard = (memcmp(rfid.uid.uidByte, unlockCard, 4) == 0);
  bool isFob = (memcmp(rfid.uid.uidByte, unlockFob, 4) == 0);
    
  if (isCard || isFob) {  //sends the update to the webserver and also the lcd screen 
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

// void handleCamera() { //loads the camera html file
//   server.send(200, "text/html", CAMERA_HTML);
// }

// void handleKeypad() { //loads the keypad html file
//   //webScreen = "____";
//   //keyIndex = 0;
//   server.send(200, "text/html", KEYPAD_HTML);
// }

// void handleLocation() { //loads the location html file
//   server.send(200, "text/html", LOCATION_HTML);
// }

float distanceRead() {  //same function as above and bellow but allows the belwo function distancemessagebox() to let the webserver know if its safe or not 
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

String serverMessageBox() { //displays to the webserver what the state of the alarm is 
  String serMes = "Error";

  if (alarmStatus == 1) {
    serMes = "Alamra Status<br>-ACTIVE-";
  }
  else {
    serMes = "Alamra Status<br>-DISABLED-";
  }

  return serMes; 
}

String distanceMessageBox() {  //displays to the webserver using above function weather or not movment has been detected 
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

String rfidMessageBox() { //after a card has been scaned this function tells the webserver if it was the saved key card or a key fob, and if it dosent know the card it says no match 
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

/*
String webDisplay() {
  return webScreen;
}
*/

//this function gets information from the javascript in the keypadWeb.h files keypad buttons to act just like the phisical keypad 
//this function can also do the check code function to check what has been input is correct to avoid issues that would keep freezing the webserver and lcd display 
//only on the webserver keypad, you have to press the * key to progress to the next state as without this implamentation, the last number input would not display on the webserver
void keyPadWeb(char key) {  
  if (webState == 0 && key >= '0' && key <= '9') {
    if (webIndex < 4) {
      webPass[webIndex] = key - '0';
      webIndex++;
    }

    lcd.clear();

    //sends to the lcd display whats being input on the webserver 
    webScreen = "";
    for (int i = 0; i < webIndex; i++) {
      webScreen += String(webPass[i]);
    }
    for (int j = webIndex; j < 4; j++) {
      webScreen += "_";
    }

    lcd.print("Webserver");
    lcd.setCursor(0, 1);
    lcd.print(webScreen);

    if (webIndex == 4) {
      webState = 1;
    }
  }
  else if (webState == 1 && key == '*') {
    webPassWrong = 0;
    for (int i = 0; i < 4; i++) {
      if (webPass[i] != passCode[i]) {
        webPassWrong++;
      }
    }
    
    lcd.clear();

    if (webPassWrong == 0) {
      webScreen = "Correct";
      lcd.print("Webserver");
      lcd.setCursor(0, 1);
      lcd.print(webScreen);
      atempt = 3;
      if (alarmStatus == 1) {
        alarmStatus = 0;
      }
      else {
        alarmStatus = 1;
      }
    }
    else {
      webScreen = "Incorrect";
      atempt--;
      if(alarmStatus == 0) {
        lcd.print("Webserver");
        lcd.setCursor(0, 1);
        lcd.print(webScreen);
      }
      else if (alarmStatus == 1 && atempt > 0) {
        lcd.print("Webserver");
        lcd.setCursor(0, 1);
        lcd.print(webScreen);
        webScreen = String(atempt) + " Attempts Left";
        if (key == '*') {
          lcd.clear();
          lcd.print("Incorrect");
          lcd.setCursor(0, 1);
          lcd.print(webScreen);
        }
      }
      else if (alarmStatus == 1 && atempt == 0) {
        lcd.print("Webserver");
        lcd.setCursor(0, 1);
        lcd.print(webScreen);
        webScreen = "Sounding Alarm";
        if (key == '*') {
          lcd.clear();
          lcd.print("Incorrect");
          lcd.setCursor(0, 1);
          lcd.print(webScreen);
          buzzer();
        }
      }
      else if (alarmStatus = 0){
        lcd.print("Webserver");
        lcd.setCursor(0, 1);
        lcd.print(webScreen);
        webScreen = "Try Again";
        if (key == '*') {
          lcd.clear();
          lcd.print("Incorrect");
          lcd.setCursor(0, 1);
          lcd.print(webScreen);
        }
      }
    }
    webState = 2;
  }
  else if (webState == 2 && key == '*') {

    webIndex = 0;
    webState = 0;
    webScreen = "____";
    prompt = true;
  }
  else if (key == '#') {
    rfidFunction();
  }
}

void gps() {
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