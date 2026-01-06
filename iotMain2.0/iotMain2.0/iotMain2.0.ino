//#include <webpage.h>
#include <testWebpage.h>
#include <esp_now.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#define BUZZ 32

const char* ssid = "Backup";         
const char* password = "nonono12345";
String moveString = "Error";
String alarmString = "Error";
String userString = "Error";
String keyString = "Error";

typedef struct struct_message1 {
  float dist;
  float humi;
  float celc;
  float fara;
  float latt;
  float lonn;
  char latC;
  char lonC;
  int move;
} struct_message1;
struct_message1 incomingReadings1;

typedef struct struct_message2 {
  int alar;
  int user;
  int key;
} struct_message2;
struct_message2 incomingReadings2;

JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  if(len == sizeof(struct_message1)) {
    memcpy(&incomingReadings1, incomingData, sizeof(incomingReadings1));
  } else if(len == sizeof(struct_message2)) {
    memcpy(&incomingReadings2, incomingData, sizeof(incomingReadings2));
  }
  
  if(incomingReadings1.move == 1) { moveString = "Movement";}
  else { moveString = "No Movement"; }

  if(incomingReadings2.alar == 0) { alarmString = "Disarmed"; }
  else { alarmString = "Active"; }

  if(incomingReadings2.user == 1) { userString = "Seth K"; }
  else if(incomingReadings2.user == 2) { userString = "Seth F"; }
  else { userString = "Unknown"; }

  if(incomingReadings2.key == 1) { keyString = "Card"; }
  else if(incomingReadings2.key == 2) { keyString = "Fob"; }
  else { keyString = "Unknown"; }

  board["alarm"] = alarmString;
  board["distance"] = incomingReadings1.dist;
  board["movement"] = moveString;
  board["user"] = userString;
  board["key"] = keyString;
  board["latitude"] = incomingReadings1.latt;  
  board["latitudeX"] = String(incomingReadings1.latC);
  board["longitude"] = incomingReadings1.lonn;   
  board["longitudeY"] = String(incomingReadings1.lonC);
  board["celcius"] = incomingReadings1.celc;
  board["farenheight"] = incomingReadings1.fara;
  board["humidity"] = incomingReadings1.humi;

  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  /*serial print the data that is sent from modual 1 and 2*/

  Serial.printf("DISTANCE: %.2fcm\n", incomingReadings1.dist);
  Serial.printf("MOVEMENT: %d\n", incomingReadings1.move);  
  Serial.printf("LATITUDE: %.4f %c\n", incomingReadings1.latt, incomingReadings1.latC);
  Serial.printf("LONGITUDE: %.4f %c\n", incomingReadings1.lonn, incomingReadings1.lonC);
  Serial.printf("CELCIUS: %.2f°C\n", incomingReadings1.celc);
  Serial.printf("FARENHEIGHT: %.2f°F\n", incomingReadings1.fara);
  Serial.printf("HUMIDITY: %.2f%%\n", incomingReadings1.humi);
  Serial.printf("------------------------------------------------\n");
  Serial.printf("ALARM: %d\n", incomingReadings2.alar);
  Serial.printf("USER: %d\n", incomingReadings2.user);
  Serial.printf("KEY: %d\n", incomingReadings2.key);
  Serial.printf("------------------------------------------------\n");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);       
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());       

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(200, "text/html", index_html);
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });

  server.addHandler(&events);
  server.begin();
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    lastEventTime = millis();
  }

  if(incomingReadings1.move == 1 && incomingReadings2.alar == 1) {
    alarmString = "Alert";
    digitalWrite(BUZZ, HIGH);
    delay(250);
    digitalWrite(BUZZ, LOW);
    delay(250);
  }
}