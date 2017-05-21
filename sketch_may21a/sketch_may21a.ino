#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

ESP8266WebServer server(80);

int ws = 0;
int dw = 0;
int wd = 200;
int sd = 800;
int tl = 10;
bool lkfr;
bool fr = true;
int RUNPIN = D5;
int FORPIN = D6;
int DAWPIN = D7;
String redirectString = "<html> <head> <script> window.location.href = 'http://wash.athulraj.com'; </script> </head> </html>";

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(RUNPIN, OUTPUT);
  pinMode(FORPIN, OUTPUT);
  pinMode(DAWPIN, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect("Washing Machine", "password")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  Serial.println("connected...yeey :)");
  Serial.print("local ip : ");
  Serial.println(WiFi.localIP());
  digitalWrite(BUILTIN_LED, LOW);
  server.on("/", [](){
    server.send(200, "text/html", redirectString);
  });
  server.on("/wash/start", [](){
    ws=1;
    server.send(200, "text/html", "Washing");
  });
  server.on("/wash/end", [](){
    ws=0;
    server.send(200, "text/html", "Drum Idle");
  });
  server.on("/spin/start", [](){
    ws=2;
    server.send(200, "text/html", "Spinning");
  });
  server.on("/spin/end", [](){
    ws=0;
    server.send(200, "text/html", "Drum Idle");
  });  
  server.on("/dump/start", [](){
    dw=1;
    server.send(200, "text/html", "Dumping");
  });
  server.on("/dump/end", [](){
    dw=0;
    server.send(200, "text/html", "Not Dumping");
  });
  server.begin();
}

void loop() {
  server.handleClient();
  if(tl>0){
    tl--;
  }else{
    switch(ws){
      case 0 : tl += 0; break;
      case 2 : 
        tl += sd;
        fr = !fr;
      break;
      case 1 : 
        tl += wd; 
        fr = !fr;
      break;
    }
  }
  //Serial.println (String(ws) + "(" + String(fr) + ")" + String(dw));
  if(ws>0)
    digitalWrite(RUNPIN, HIGH);
  else
    digitalWrite(RUNPIN, LOW);
  if(dw>0)
    digitalWrite(DAWPIN, HIGH);
  else
    digitalWrite(DAWPIN, LOW);
  if (lkfr != fr){
    digitalWrite(RUNPIN, LOW);
    delay(10000);
    if(fr>0)
      digitalWrite(FORPIN, HIGH);
    else
      digitalWrite(FORPIN, LOW);
    delay(10000);
    digitalWrite(RUNPIN, LOW);
    lkfr = fr;
  }
  delay(100);
}
