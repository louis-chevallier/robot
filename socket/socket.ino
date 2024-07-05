#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <List.hpp>
#include <DHT.h>
#include <Hash.h>
#include <Arduino.h>
#include <microTuple.h>

#include <WebSocketsServer.h>


String S;

long seko = millis();
#define EKOT(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + String(x)); seko=millis()
#define EKOX(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + #x + " = " + String(x)); seko=millis()
#define EKO() Serial.println(S + __FILE__+ ":" + __LINE__ + "[" + (millis()-seko) + "]"); seko=millis()

/********************************/
// webserver

ESP8266WebServer server(80);
//WebSocketsServer server = WebSocketsServer(80);

// chez nous
const char* ssid = "CHEVALLIER_BORDEAU"; //Enter Wi-Fi SSID
const char* password =  "9697abcdea"; //Enter Wi-Fi Password
const char* WURL = "http://176.161.19.7:8080/main";
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
  </head>
  <body>
  hello
  TAG
  </body>
  </html>
)"""");

void handle_index() {
  Serial.println("index");
 
  String npage(page);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", npage.c_str());
  Serial.println("end");
}

void handle_x() {
  // method get
  String npage("ok");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", npage.c_str());
  Serial.println("end");
};


void webSetup() {
  
  WiFi.begin(ssid, password);  //Connect to the WiFi network
  
  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
      delay(500);
      Serial.println("Waiting to connect...");
  }
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
  server.on("/", handle_index); //Handle Index page
  server.on("/x", handle_x);
  
  server.begin(); //Start the server
  Serial.println("setup");
  pinMode(2, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(2, HIGH);  
  Serial.println("Server listening");
}

void webLoop() {
  server.handleClient(); //Handling of incoming client requests
}

/***********************/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //Begin Serial at 115200 Baud
  Serial.print("starting");
  EKO();
  delay(10);
  webSetup();
  
  EKOX("ok");
  EKOX(D1);
  EKOX(D2);
  EKOX(D3);
  EKOX(D4);
  EKOX(D5);
}

void loop() {
  // put your main code here, to run repeatedly:
  webLoop();
}
