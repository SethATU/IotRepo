#include <webpage.h>
#include <Wire.h>   
#include "rgb_lcd.h"
#include <esp_now.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

rgb_lcd lcd;

const char* ssid = "Backup";         
const char* password = "nonono12345";

typedef struct struct_message {
  float dist;
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

  board["distance"] = incomingReadings.dist;

  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  Serial.printf("DISTANCE: %2f cm\n", incomingReadings.dist);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(27, 26);
  lcd.begin(16, 2);   

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
    String serverData = homepage1
                    + String(incomingReadings.dist) + " cm"
                    + homepage2;
  request->send(200, "text/html", serverData);
  });
  server.begin();

  lcd.print("Loading"); 
  for (int l = 0; l < 9; l++) {
    lcd.print(".");
    delay(250);
  }
}

void loop() {

}