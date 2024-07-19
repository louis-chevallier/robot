// install ESPAsyncWebServer (lacamera)
// install ESPAsyncHTTPUpdateServer ( mohamad Mahdi)
#include "ESPAsyncWebServer.h"
#include <L298N.h>
#include <HC_SR04.h>
#include "microTuple.h"
#include <List.hpp>

/* pinout

Label	GPIO	Input           Output                  Notes
D0	GPIO16	no interrupt	no PWM  or I2C support	HIGH at boot, used to wake up from deep sleep                           HCB
D1	GPIO05	OK	        OK	                often used as SCL (I2C)                                                 IN2 A
D2	GPIO04	OK	        OK	                often used as SDA (I2C)                                                 IN1 A
D3	GPIO00	pulled up	OK	                connected to FLASH button, boot fails if pulled LOW                     IN2 B in4
D4	GPIO02	pulled up	OK	                HIGH at boot, connected to on-board LED, boot fails if pulled LOW       IN1 B in3
D5	GPIO14	OK	        OK	                SPI (SCLK)                                                              ENB
D6	GPIO12	OK	        OK	                SPI (MISO)                                                              HCA
D7	GPIO13	OK	        OK	                SPI (MOSI)                                                              ENA
D8	GPIO15	pulled to GND	OK	                SPI (CS), Boot fails if pulled HIGH                                     HCC
RX	GPIO03	OK	        RX pin	                HIGH at boot                                                            Trigger                
TX	GPIO01	TX pin	        OK	                HIGH at boot, debug output at boot, boot fails if pulled LOW            
A0	ADC0	Analog Input	X	
*/
String S;

long seko = millis();
#define EKOT(x) Serial.println(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms] " + String(x) + "."); seko=millis()
#define EKOX(x) Serial.println(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms] " + #x + "=" + String(x) + "."); seko=millis()
#define EKO()   Serial.println(S + __FILE__ + ":" + String(__LINE__) + ": [" + String(millis()-seko) + "ms]"); seko=millis()


//////////////////////////////
// HC SR04
long lasthcms=0;
const int trigPin = 3; // RX
#define echoPinA 12 // D6
#define echoPinB 16 // D0
#define echoPinC 15 // D8

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

struct Element;

List<Element*> elements = List<Element*>();


struct Element {
  Element() { EKO(); elements.add(this); }
  virtual void loop() {}
  virtual void setup() {}
};

struct HCSR04_Multiple : Element {
  HC_SR04_BASE *Slaves[2] = { new HC_SR04<echoPinA>(trigPin), new HC_SR04<echoPinB>(trigPin) };
  
  HC_SR04<echoPinC> &sonicMaster;
  
  HCSR04_Multiple() : sonicMaster(*new HC_SR04<echoPinC>(trigPin, Slaves, 2)) {
    elements.add(this);
  }
  
  void setup() {
    //sensor.begin();
    sonicMaster.beginAsync();
  }  
    
  void loop() {
    sonicMaster.startAsync(200000);
    while(!sonicMaster.isFinished())
    {
       // Do something usefulle while measureing
       // all echo pins which doesnt support interrupt will have a 0 result
    }
   
    for (int i = 0; i < sonicMaster.getNumberOfSensors(); i++) {
      Serial.print(sonicMaster.getDist_cm(i));
      Serial.print("  ");
    }
  }
  
};


///////////////////////////////////////


// Pin definition
const unsigned int IN1A = 4; // gpio number => D2
const unsigned int IN2A = 5; // D1
const unsigned int ENA = 13; // D7

const unsigned int IN1B = 2; // D4
const unsigned int IN2B = 0; // D3
const unsigned int ENB = 14; // D5



// Initial speed
short theSpeed = 100;

struct Motor : Element, L298N {
  Motor(int in1, int in2, int en) : L298N(en, in1, in2) {

  }
  void setup() {

    EKOT("forward");
    setSpeed(200);
    forward();
    EKO();
    delay(500);
    stop();

    EKOT("backward");
    setSpeed(200);
    backward();
    EKO();
    delay(500);
    stop();
    EKO();

  }
};

// Create one motor instance
Motor motorA(ENA, IN1A, IN2A), motorB(ENB, IN1B, IN2B);

MicroTuple<String, String> split(const String &mess, const String &sep = "?") {
  auto index = mess.indexOf(sep);
  return MicroTuple<String, String>(mess.substring(0, index), mess.substring(index+1));
} 

void command(const String &com, const String &param) {
  //EKOX(com);
  //EKOX(param);
  auto tt = split(com, "_");
 
  if (tt.get<0>() == "speed") {
    speed(tt.get<1>(), param);
  }
}

void speed(const String &motor_, const String &param) {
  auto &motor = motor_ == "A" ? motorA : motorB;
    //EKOX(param.toInt());
    auto v = param.toInt();
    if (abs(v) > 40) {
      if (v>0) { 
        motor.forward();
      } else {
        motor.backward();
      }
    } else {
      motor.stop();
    }    
    motor.setSpeed(abs(param.toInt()));
}


String page(R""""(
#include "html.h"
)"""");

struct MyServer : Element {


  const char* ssid = "CHEVALLIER_BORDEAU"; //Enter Wi-Fi SSID
  const char* password =  "9697abcdea"; //Enter Wi-Fi Password
 
  AsyncWebServer server;
  AsyncWebSocket ws;
  AsyncWebSocketClient * globalClient;

    MyServer() : server(80), ws("/ws"), globalClient(NULL) {}

    void setup();

  void loop(){
   if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
      //EKO();
      /*
      auto r = random(0,100); 
      if (r == 1) {
        String randomNumber = String(random(0,100));
        globalClient->text(randomNumber);
      }
      */
   }
   delay(4);    
    /*
   if (millis() > lasthcms + 1000) {
    auto d = sensor.getDist_cm();
    EKOX(d);
    sensor.startMeasure();
    Serial.println(d);
    lasthcms = millis();
    if (d < 5) {
      //globalClient->text("dist " + d);
    }
   }
   */


   
  }
};

MyServer myserver;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  //EKOX(type);
  if(type == WS_EVT_CONNECT){
    EKOT("connect");
    Serial.println("Websocket client connection received");
    myserver.globalClient = client;
 
  } else if(type == WS_EVT_DISCONNECT){
    EKOT("disconnect");
    Serial.println("Websocket client connection finished");
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




struct HCSR04 : Element {
  HC_SR04<echoPinA> &sensor;
  int lastthcms;
  HCSR04() : sensor(*new HC_SR04<echoPinA>(trigPin)), lastthcms(0) {
    
  }
  
  void setup() {
    //sensor.begin();
    sensor.begin();
  }  
    
  void loop() {
    if (millis() > lasthcms + 100) {
      sensor.startMeasure();
      auto d = sensor.getDist_cm();
      //EKOX(d);
      lasthcms = millis();
      if (d < 50 && myserver.globalClient != NULL) {
        //EKO();
        //myserver.globalClient->text(String("distance=") + d);
        myserver.globalClient->text(String("distance_A=") + d);
      }
   }
  }
  
};

HCSR04 hcsr04;


void setup() {
  for (int i = 0; i < elements.getSize(); i++) {
    elements[i]->setup();
  }
}
void loop() {
  for (int i = 0; i < elements.getSize(); i++) {
    elements[i]->loop();
  }
}


