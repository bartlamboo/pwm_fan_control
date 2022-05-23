#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>

// Wifi Variables
const char* ssid = "";
const char* password = "";

// MQTT Broker
const char *mqtt_broker = "";
const char *topic = "extractor-fan/value";
const char *statustopic = "extractor-fan/status";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int   mqtt_port = 1883;

// Pin 0 is called D3. (project fails when connected to D4 because pin is in unwanted state when booting)
#define PWMPIN 0

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.nl.pool.ntp.org");

void setup() {
  // Set PWN pin properties
  Serial.begin(115200);
  pinMode(PWMPIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // Set Wifi properties and connect
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.hostname("extractor-fan");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Print the IP address for debug purposes
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start NTP client and set to 
  timeClient.begin();
  timeClient.setTimeOffset(3600);
  
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // publish and subscribe
  statusUpdate("MQTT connected");
  client.subscribe(topic);
}

void statusUpdate(String statusmessage) {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  statusmessage = formattedTime+"> "+statusmessage;
  char charBuf[statusmessage.length()+1];
  statusmessage.toCharArray(charBuf, statusmessage.length()+1);
  client.publish(statustopic, charBuf);
  Serial.println(statusmessage);
}

void callback(char *topic, byte *payload, unsigned int length) {
  // Convert payload output into string
  String output;
  for (int i = 0; i < length; i++) {
      output = output+(char) payload[i];
  }
  
  // PWM range seems to be between 0 and 256.
  // Multiplying input with 64 so 4 should be the maximum value. 

  int pwmout = output.toInt() * 64;
  if (pwmout >= 0 && pwmout <= 2048) {
    statusUpdate("Setting PWN to: "+String(pwmout));
    analogWrite(PWMPIN,pwmout);
  }
  else
  {
    statusUpdate("Invalid PWN value received: "+String(pwmout));    
  }
}

void loop() {
  client.loop();
}
