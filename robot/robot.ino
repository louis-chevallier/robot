//https://randomnerdtutorials.com/esp8266-nodemcu-dc-motor-l298n-motor-driver-control-speed-direction/


/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-dc-motor-l298n-motor-driver-control-speed-direction/
*********/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <List.hpp>
#include <DHT.h>
#include <Hash.h>
#include <Arduino.h>
#include <microTuple.h>
#include <L298N.h>


// server http => http://192.168.1.177/

String S;

long seko = millis();
#define EKOT(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + String(x)); seko=millis()
#define EKOX(x) Serial.println(S + __FILE__ + ":" + __LINE__ + "[" + (millis()-seko) + "] " + #x + " = " + String(x)); seko=millis()
#define EKO() Serial.println(S + __FILE__+ ":" + __LINE__ + "[" + (millis()-seko) + "]"); seko=millis()


/********************************/

// Pin definition
const unsigned int IN1 = 4; // gpio number => D2
const unsigned int IN2 = 5; // D1
const unsigned int EN = 13; // D7

// Create one motor instance
L298N motor(EN, IN1, IN2);

// Initial speed
short theSpeed = 100;

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

void handle_speed() {
  String v = server.arg("value");
  auto vi = v.toInt();
  String r = String("set speed to :") + vi;
  EKOX(r);
  theSpeed = vi; //motor.forwardFor(2000, callback);
  motor.setSpeed(abs(theSpeed));
  EKOX(theSpeed);
  if (theSpeed > 0) {
    motor.forward();
  } else {
    motor.backward();
  }

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", r.c_str());
  Serial.println("end");
}

void handle_index() {
  Serial.println("index");
 
  String npage(page);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", npage.c_str());
  Serial.println("end");
}

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
  server.on("/speed", handle_speed); //Handle Index page
  
  server.begin(); //Start the server
  Serial.println("setup");
  Serial.println("Server listening");
}


void webLoop() {
  server.handleClient(); //Handling of incoming client requests

}


// Motor A
int motor1Pin1 = 12; 
int motor1Pin2 = 14; 
int enable1Pin = 13; 

// Setting minimum duty cycle
int dutyCycle = 60;

void motorSetup() {
  return;

  for (;;) {
    EKOT("forward");
    motor.setSpeed(100);
    motor.forward();
    EKO();
    delay(3000);
    motor.stop();

    EKOT("backward");
    motor.setSpeed(100);
    motor.backward();
    EKO();
    delay(3000);
    motor.stop();
  }
}

void callback()
{
  EKOT("callback");
  motor.reset();
}


void setup() {

// put your setup code here, to run once:
  Serial.begin(115200); //Begin Serial at 115200 Baud
  Serial.print("starting");
  EKO();
  delay(10);
  webSetup();
  EKO();
  motorSetup();

  EKOT("ok");
  /*
  EKOX(D1);
  EKOX(D2);
  EKOX(D3);
  EKOX(D4);
  EKOX(D5);
*/

  EKO();
  motor.forward();
  EKO();
  //motor.forwardFor(5000, callback);
  EKO();
  //delay(3000);

  // Stop
  motor.stop();
  EKO();
  //Serial.begin(115200);
}

void loop() {
  webLoop();
  /*
  if (theSpeed > 0) {
    motor.forward();
  } else {
    motor.backward();
  }
  */
}

void motorx() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);


  // testing
  EKOT("Testing DC Motor...");

  //Apply power to spin at maximum speed
  digitalWrite(enable1Pin, HIGH);

  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(1000);

  // Move DC motor backwards at maximum speed
  Serial.println("Moving Backwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(1000);

  // Move DC motor forward with increasing speed
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  while (dutyCycle <= 255){
    analogWrite(enable1Pin, dutyCycle);   
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    delay(500);
  }
  dutyCycle = 60;
}
