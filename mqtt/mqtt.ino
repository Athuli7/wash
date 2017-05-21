#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

int RUNPIN = D5;
int FORPIN = D6;
int DAWPIN = D7;
String redirectString = "<html> <head> <script> window.location.href = 'http://wash.athulraj.com'; </script> </head> </html>";
long lastMsg = 0;
char msg[50];
int value = 0;

void actOn(char inpChar){
  switch(inpChar){
    case 'A' : 
      digitalWrite(RUNPIN, HIGH);
      Serial.print("A");
    break;
    case 'B' : 
      digitalWrite(RUNPIN, LOW);
      Serial.print("B");
    break;
    case 'C' : 
      digitalWrite(FORPIN, HIGH);
      Serial.print("C");
    break;
    case 'D' : 
      digitalWrite(FORPIN, LOW);
      Serial.print("D");
    break;
    case 'E' : 
      digitalWrite(DAWPIN, HIGH);
      Serial.print("E");
    break;
    case 'F' : 
      digitalWrite(DAWPIN, LOW);
      Serial.print("F");
    break;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  actOn((char)payload[0]);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /*
      client.publish("outTopic", "hello world");
      */
      client.subscribe("/athul/washing/machine");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(RUNPIN, OUTPUT);
  pinMode(FORPIN, OUTPUT);
  pinMode(DAWPIN, OUTPUT);
  digitalWrite(RUNPIN, LOW);
  digitalWrite(FORPIN, LOW);
  digitalWrite(DAWPIN, LOW);
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
  
  server.begin();

  client.setServer("broker.hivemq.com", 1883);
  client.setCallback(callback);
  
  while (!client.connected()) {
    reconnect();
  }
  
}

void loop() {
  server.handleClient();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);
}
