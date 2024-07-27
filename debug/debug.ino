#include "ESPAsyncWebServer.h"
#include "microTuple.h"
#include <List.hpp>
String S;
long seko = millis();

void myprint(const String &);

#define EKOT(x) myprint(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms] " + String(x) + "."); seko=millis()
#define EKOX(x) myprint(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms] " + #x + "=" + String(x) + "."); seko=millis()
#define EKO()   myprint(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms]"); seko=millis()

MicroTuple<String, String> split(const String &mess, const String &sep = "?") {
  auto index = mess.indexOf(sep);
  return MicroTuple<String, String>(mess.substring(0, index), mess.substring(index+1));
} 


struct Element;

List<Element*> elements = List<Element*>();

struct Element {
  Element() { EKO(); elements.add(this); }
  virtual void loop() {}
  virtual void setup() {}
};

#include "code.h"
String jscode((const char*)bin2c_code_js);
#include "page.h"
String page((const char*)bin2c_page_html);

struct MyServer : Element {
  const char* ssid = "CHEVALLIER_BORDEAU"; //Enter Wi-Fi SSID
  const char* password =  "9697abcdea"; //Enter Wi-Fi Password
  AsyncWebServer server;
  AsyncWebSocket ws;
  AsyncWebSocketClient * globalClient;
  int lastthcms;
  int count ;
 MyServer() : server(80), ws("/ws"), globalClient(NULL), lastthcms(0), count(0)
 {}
  
  void setup();
  
  void loop(){
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
    }
    delay(4);

    if (millis() > lastthcms + 1000) {
      EKOT("coucou");
      EKOX(lastthcms);
      lastthcms = millis();
      
    }
  }

  void receive(const String &s) {
    count ++;
    auto tt = split(s, "=");    
    if (tt.get<0>() == "message") {
      if (count < 20) {
        globalClient->text("count=xx");
      } else {
        EKO();
        count = 0;
      }
    }
    
  }
  
  void text(const String &mess) {
    if (globalClient != NULL) {
      globalClient->text(mess);
    }
  }
};

MyServer myserver;

void myprint(const String &s) {
  Serial.println(s);
  myserver.text("eko=" + s);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  //EKOX(type);
  if(type == WS_EVT_CONNECT){
    EKOT("connect");
    EKOT("Websocket client connection received");
    myserver.globalClient = client;
 
  } else if(type == WS_EVT_DISCONNECT){
    EKOT("disconnect");
    EKOT("Websocket client connection finished");
    myserver.globalClient = NULL;
  } else if(type == WS_EVT_DATA) {
    //EKOX(len);
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      //Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      //EKOX(msg.c_str());
      myserver.receive(msg);
      /*
        if(info->opcode == WS_TEXT)
        client->text("I got your text message");
        else
        client->binary("I got your binary message");
      */

    } else {
      EKO();
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }
      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());
      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

void MyServer::setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    EKOT("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  page.replace("JSCODE", jscode);
  
  EKO();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  EKO();
 
  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
      EKO();
      request->send(200, "text/html", page);
    });

  server.on("/speed", HTTP_GET, [](AsyncWebServerRequest *request){
      EKO();
      request->send(200, "text/plain", "speed received");
      //request->send(SPIFFS, "/ws.html", "text/html");
    });
  EKO();
  
  server.begin();
  EKOT("server ok");
};


void setup() {
  EKO();
  for (int i = 0; i < elements.getSize(); i++) {
    elements[i]->setup();
  }
  EKOT("setup ok");
}
void loop() {
  for (int i = 0; i < elements.getSize(); i++) {
    elements[i]->loop();
  }
}


