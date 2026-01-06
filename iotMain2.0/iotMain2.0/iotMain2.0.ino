//#include <webpage.h>
#include <testWebpage.h>
#include <esp_now.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

const char* ssid = "Backup";         
const char* password = "nonono12345";

typedef struct struct_message {
  float dist;
  float humi;
  float celc;
  float fara;
  float latt;
  float lonn;
  char latC;
  char lonC;
} struct_message;

struct_message incomingReadings;

JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  //board["alarm"] = incomingReadings.alar;
  board["distance"] = incomingReadings.dist;
  //board["movement"] = incomingReadings.move;
  //board["user"] = incomingReadings.user;
  //board["key"] = incomingReadings.key;
  board["latitude"] = incomingReadings.latt;  
  board["latitudeX"] = String(incomingReadings.latC);
  board["longitude"] = incomingReadings.lonn;   
  board["longitudeY"] = String(incomingReadings.lonC);
  board["celcius"] = incomingReadings.celc;
  board["farenheight"] = incomingReadings.fara;
  board["humidity"] = incomingReadings.humi;

  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  //serial print the data that is sent from modual 1
  Serial.printf("DISTANCE: %.2fcm\n", incomingReadings.dist);
  Serial.printf("HUMIDITY: %.2f%%\n", incomingReadings.humi);
  Serial.printf("CELCIUS: %.2f°C\n", incomingReadings.celc);
  Serial.printf("FARENHEIGHT: %.2f°F\n", incomingReadings.fara);
  Serial.printf("LATITUDE: %.4f %c\n", incomingReadings.latt, incomingReadings.latC);
  Serial.printf("LONGITUDE: %.4f %c\n", incomingReadings.lonn, incomingReadings.lonC);
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
}

void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    lastEventTime = millis();
  }
}