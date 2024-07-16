//#include "WiFi.h"
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
    EKOT("connect");
    Serial.println("Websocket client connection received");
    globalClient = client;
 
  } else if(type == WS_EVT_DISCONNECT){
    EKOT("disconnect");
    Serial.println("Websocket client connection finished");
    globalClient = NULL;
 
  }
}
 
String page(R""""(
<!DOCTYPE html>
<html>
  <head>
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <!-- 
    <meta http-equiv="refresh" content="4">
    --!>
    <title>Robot</title>
    <style>
    .bb {
        font-size: 80px;
    }
    </style>
    <script src="https://cdn.plot.ly/plotly-2.27.0.min.js" charset="utf-8"></script>

  <script>


  (() => {
  const ws = new WebSocket('ws://192.168.1.177:80/ws')
  ws.onopen = () => {
    console.log('ws opened on browser')
    ws.send('hello world')
  }

  ws.onmessage = (message) => {
    console.log(`message received ${message}`)
  }

})()

  </script>


  </head>
  <body>
  hello
  TAG
  </body>
  </html>
)"""");


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
  EKO();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
 EKO();
  
  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    EKO();
    request->send(200, "text/html", page);
    //request->send(SPIFFS, "/ws.html", "text/html");
  });

   server.on("/speed", HTTP_GET, [](AsyncWebServerRequest *request){
    EKO();
    request->send(200, "text/plain", "speed received");

    //request->send(SPIFFS, "/ws.html", "text/html");
  });
  EKO();
  
  server.begin();
  EKO();
}
 
void loop(){
   if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
      //EKO();
      auto r = random(0,100); 
      if (r == 1) {
        String randomNumber = String(random(0,100));
        globalClient->text(randomNumber);
      }
   }
   delay(40);
}
