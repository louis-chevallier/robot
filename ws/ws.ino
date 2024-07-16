//#include "WiFi.h"
//#include "SPIFFS.h"

// install ESPAsyncWebServer (lacamera)
// install ESPAsyncHTTPUpdateServer ( mohamad Mahdi)
#include "ESPAsyncWebServer.h"
#include <L298N.h>
#include <HC_SR04.h>

/* pinout

Label	GPIO	Input	Output	Notes
D0	GPIO16	no interrupt	no PWM or I2C support	HIGH at boot
used to wake up from deep sleep
D1	GPIO5	OK	OK	often used as SCL (I2C)
D2	GPIO4	OK	OK	often used as SDA (I2C)
D3	GPIO0	pulled up	OK	connected to FLASH button, boot fails if pulled LOW
D4	GPIO2	pulled up	OK	HIGH at boot
connected to on-board LED, boot fails if pulled LOW
D5	GPIO14	OK	OK	SPI (SCLK)
D6	GPIO12	OK	OK	SPI (MISO)
D7	GPIO13	OK	OK	SPI (MOSI)
D8	GPIO15	pulled to GND	OK	SPI (CS)
Boot fails if pulled HIGH
RX	GPIO3	OK	RX pin	HIGH at boot
TX	GPIO1	TX pin	OK	HIGH at boot
debug output at boot, boot fails if pulled LOW
A0	ADC0	Analog Input	X	
*/
String S;

long seko = millis();
#define EKOT(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + String(x) + "."); seko=millis()
#define EKOX(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + #x + "=" + String(x) + "."); seko=millis()
#define EKO() Serial.println(S + __FILE__+ ":" + __LINE__ + "[" + (millis()-seko) + "]"); seko=millis()

// Pin definition
const unsigned int IN1 = 4; // gpio number => D2
const unsigned int IN2 = 5; // D1
const unsigned int EN = 13; // D7

// Create one motor instance
L298N motor(EN, IN1, IN2);


// Initial speed
short theSpeed = 100;

char* ssid = "CHEVALLIER_BORDEAU"; //Enter Wi-Fi SSID
char* password =  "9697abcdea"; //Enter Wi-Fi Password
 
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient * globalClient = NULL;

//////////////////////////////
// HC SR04
long lasthcms=0;
const int trigPin = 12;
const int echoPin = 14;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

//HC_SR04<14> sensor(trigPin  );   // sensor with echo and trigger pin


//////////////////////////////////////

void motorSetup() {

    EKOT("forward");
    motor.setSpeed(200);
    motor.forward();
    EKO();
    delay(500);
    motor.stop();

    EKOT("backward");
    motor.setSpeed(200);
    motor.backward();
    EKO();
    delay(500);
    motor.stop();
}

void command(const String &com, const String &param) {
  EKOX(com);
  EKOX(param);
  if (com == "speed") {
    EKOX(param.toInt());
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
}


void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  //EKOX(type);
  if(type == WS_EVT_CONNECT){
    EKOT("connect");
    Serial.println("Websocket client connection received");
    globalClient = client;
 
  } else if(type == WS_EVT_DISCONNECT){
    EKOT("disconnect");
    Serial.println("Websocket client connection finished");
    globalClient = NULL;
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
      EKOX(msg.c_str());
      auto index = msg.indexOf("?");
      command(msg.substring(0, index), msg.substring(index+1)); 


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
    .slidecontainer {
      width: 100%; /* Width of the outside container */
    }
    </style>
    <style>
            html, body {
                overflow: hidden;
                width: 100%;
                height: 100%;
                margin: 0;
                padding: 0;
            }
            #renderCanvas {
                width: 100%;
                height: 100%;
                touch-action: none;
            }
     </style>



  </head>
  <body>
  hello
  TAG
  <div id="demo"> </div>
  <div class="slidecontainer">
      <input type="range" min="-255" max="255" value="0" class="slider" id="myRange">
  </div>

  <script>


  (() => {
  const ws = new WebSocket('ws://192.168.1.177:80/ws')
  ws.onopen = () => {
    console.log('ws opened on browser')
    ws.send('hello world')
  }

  ws.onmessage = (message) => {
    console.log(`message received ` + message.data)
  }

  var slider = document.getElementById("myRange");
  var output = document.getElementById("demo");
  output.innerHTML = slider.value; // Display the default slider value

  slider.oninput = function() {
    output.innerHTML = this.value;
    ws.send('speed?' + this.value);
    //console.log(this.value);

} 

})()

  </script>


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
  
  motorSetup();



  ////////// HC sr04
  //pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  //pinMode(echoPin, INPUT);

  //sensor.begin();

  server.begin();
  EKO();
}
 
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
