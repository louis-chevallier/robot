#include "ESPAsyncWebServer.h"
#include <L298N.h>

#include "microTuple.h"
#include <List.hpp>
#include <UltraPing.h>


String S;

long seko = millis();
#define EKOT(x) Serial.println(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms] " + String(x) + "."); seko=millis()
#define EKOX(x) Serial.println(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms] " + #x + "=" + String(x) + "."); seko=millis()
#define EKO()   Serial.println(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms]"); seko=millis()

struct Element;
List<Element*> elements = List<Element*>();
struct Element {
  Element() { EKO(); elements.add(this); }
  virtual void loop() {}
  virtual void setup() {}
};

MicroTuple<String, String> split(const String &mess, const String &sep = "?") {
  auto index = mess.indexOf(sep);
  return MicroTuple<String, String>(mess.substring(0, index), mess.substring(index+1));
} 


#define TIMES_TO_LOOP 1000000

volatile uint16_t ux16, uy16, uresult16, uz16, uw16(1), uuu(5);
volatile uint32_t ux32, uy32, uresult32;
volatile uint64_t ux64, uy64, uresult64;

volatile int16_t x16, y16, result16;


const int BUF_SIZE = 20000;
uint16_t buffer[BUF_SIZE];
auto i_buffer = 0;

uint16_t seed16() {
  return random(0, 0xffff);
}


struct MyServer : Element {
  const char* ssid = "TENDA"; //Enter Wi-Fi SSID
  const char* password =  "9697abcdea"; //Enter Wi-Fi Password
 
  AsyncWebServer server;
  AsyncWebSocket ws;
  AsyncWebSocketClient * globalClient;

  MyServer() : server(80), ws("/ws"), globalClient(NULL)
    {
      EKOT("build myserver");
    }
  
  void setup();

  void loop(){
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
      EKO();
    }
    delay(4);    
  }
};

MyServer myserver;


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
  } else if(type == WS_EVT_DATA){
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
      auto t = split(msg);
      command(t.get<0>(), t.get<1>());

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
#include "code.h"
String jscode((const char*)bin2c_bench_js);
#include "page.h"
String page((const char*)bin2c_bench_html);


void MyServer::setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    EKOT("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  EKO();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  EKO();
  
  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
      EKO();
      request->send(200, "text/html", page);
    });

  EKO();
  
  server.begin();
  EKOT("server ok");
}

void command(const String &com, const String &param) {
  auto tt = split(com, "_");
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("serial on");
  
  EKO();

  ux16 = seed16();
  uy16 = seed16();
  uz16 = seed16();
  x16 = ux16;
  y16 = uy16;
  EKO();


  //uuu = EXP5(ux16+2);
  auto
    nmb_op = 800000,
    block = 3,
    loop =  nmb_op/block;
  {
    uint64_t micros_start  = micros();
    for(int i = 0; i < loop; i++) {
      uresult16 = ux16 + uy16 / uz16; // 3
      uw16 ++;
    }
    auto micros_end = micros();
    
    Serial.print("uint16_t microseconds ");
    Serial.println(micros_end - micros_start);
    
  }
  {
    uint64_t micros_start  = micros();
    for(int i = 0; i < TIMES_TO_LOOP; i++)
      uresult16 = ux16 * uy16;
    auto micros_end = micros();
    
    Serial.print("uint16_t microseconds ");
    Serial.println(micros_end - micros_start);
  }


  
  EKO();
  for (int i = 0; i < elements.getSize(); i++) {
    elements[i]->setup();
  }
  EKOT("setup ok");

}
void loop() {
  for (int i = 0; i < elements.getSize(); i++) {
    elements[i]->loop();

    auto
      nmb_op = 800000,
      block = 3,
      loop =  nmb_op/block;
    {
      uint64_t micros_start  = micros();
      for(int i = 0; i < loop; i++) {
        uresult16 = ux16 + uy16 * uz16; // 3
        uw16 ++;
        buffer[i_buffer] = uresult16;
        i_buffer = (i_buffer + 1) % BUF_SIZE;
        
      }
      auto micros_end = micros();
      
      //Serial.print("uint16_t microseconds ");
      //Serial.println(micros_end - micros_start);
      
    }
    
    
  }
}


