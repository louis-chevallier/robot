#include "WiFi.h"
//#include "SPIFFS.h"

// install ESPAsyncWebServer (lacamera)
#include "ESPAsyncWebServer.h"
 

String S;

long seko = millis();
#define EKOT(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + String(x)); seko=millis()
#define EKOX(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + #x + " = " + String(x)); seko=millis()
#define EKO() Serial.println(S + __FILE__+ ":" + __LINE__ + "[" + (millis()-seko) + "]"); seko=millis()


 char* ssid = "CHEVALLIER_BORDEAU"; //Enter Wi-Fi SSID
 char* password =  "9697abcdea"; //Enter Wi-Fi Password
 
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient * globalClient = NULL;
 
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){
 
    Serial.println("Websocket client connection received");
    globalClient = client;
 
  } else if(type == WS_EVT_DISCONNECT){
 
    Serial.println("Websocket client connection finished");
    globalClient = NULL;
 
  }
}
 
void setup(){
  Serial.begin(115200);
 
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());
 
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
 
  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ws.html", "text/html");
  });
 
  server.begin();
}
 
void loop(){
   if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
      String randomNumber = String(random(0,20));
      globalClient->text(randomNumber);
   }
   delay(4000);
}
