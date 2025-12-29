#include <webpage.h>
#include <WiFi.h>     
#include <ESPmDNS.h>  
#include <WebServer.h>
#include <Wire.h>   
#include "rgb_lcd.h"

rgb_lcd lcd;

const char* ssid = "Backup";         
const char* password = "nonono12345";

WebServer server(80);

void setup() {
  Serial.begin(115200);

  Wire.begin(27, 26);
  lcd.begin(16, 2);   

  WiFi.mode(WIFI_STA);       
  WiFi.begin(ssid, password);
  Serial.println("");        

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
  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  lcd.print("Loading"); //little loading bar printed to lcd
  for (int l = 0; l < 9; l++) {
    lcd.print(".");
    delay(250);
  }
}

void loop() {
  server.handleClient();
  delay(2);
}

void handleRoot() {
  String serverData = homepage1;
  server.send(200, "text/html", serverData);
}

void handleNotFound() {
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