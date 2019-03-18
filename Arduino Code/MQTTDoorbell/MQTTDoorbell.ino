/**********************************************************************************************
  MQTT Doorbell Module
  Written by: Jay Collett jay.collett AT gmail.com
  Description: This sketch is for my custom PCB which allows for a much smarter
               doorbell without the need for the expensive (and ugly) smart
               doorbells with cameras and such. Simple, and so much cheaper than
               those other "smart" doorbells. You can find more information out
               about this project here:
               https://www.jaycollett.com/2019/02/a-smarter-doorbell-without-all-the-extras/
  Sketch includes unmodified code libraries from the following awesome sources:
      PubSubClient  https://github.com/knolleary/pubsubclient
      ESP8266WiFi   https://github.com/esp8266/Arduino

  This work (excluding external modules/libraries) is licensed under CC BY-SA 4.0
 ***********************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define doorbellButtonSensePin 4

#define WIFI_SSID "xxxxxxxx"
#define WIFI_PASS xxxxxxxxx"
#define MQTT_PORT 1883

int   wifiRetryCount = 0;
char  fmversion[7] = "v1.4";                  // firmware version of this sensor
char  mqtt_server[] = "192.168.0.x";          // MQTT broker IP address
char  mqtt_username[] = "doorbellsensor";     // username for MQTT broker (USE ONE)
char  mqtt_password[] = "xxxxxxxxxxxxxxx";    // password for MQTT broker
char  mqtt_clientid[] = "doorbellSensor";     // client id for connections to MQTT broker

const String HOSTNAME = "doorbellSensor";
const String topic = "binarySensor/doorbell";

IPAddress ip;
WiFiClient WiFiClient;
PubSubClient mqttclient(WiFiClient);

void setup() {

  pinMode(doorbellButtonSensePin, INPUT);
  Serial.begin(115200);
  
  WiFi.hostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  
  mqttclient.setServer(mqtt_server, MQTT_PORT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    if(wifiRetryCount >= 120){ // 250mS delay x 120 iterations = 30 seconds
      ESP.restart();
    }
    wifiRetryCount++;
  }

  wifiRetryCount = 0;
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Calling MQTT Connect");
  MQTT_connect(); // connect to wifi/mqtt as needed
  Serial.println("Pushing initial OFF payload data to MQTT server");
  mqttclient.publish(topic.c_str(), "OFF");
  
}

void loop() {

  
  // loop indefinitely, checking to see if someone has hit the doorbell button
  // doorbell sense pin will go HIGH if the doorbell button is being pushed
  if(digitalRead(doorbellButtonSensePin) == HIGH){
    delay(100); // poor man's debounce
     if(digitalRead(doorbellButtonSensePin) == HIGH){
      mqttclient.publish(topic.c_str(), "ON");
      Serial.println("Somone hit the doorbell, tell the MQTT server...");
      delay(2000); // give time for the payload to register, and prevent silly doorbell mashers from iratating me further...
      mqttclient.publish(topic.c_str(), "OFF");
     }
  }

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Wifi connection was lost...attempting reconnect.");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while(WiFi.status() != WL_CONNECTED){
      delay(250);
      Serial.print(".");
      if(wifiRetryCount >= 120){ // 250mS delay x 120 iterations = 30 seconds
      ESP.restart();
    }
    wifiRetryCount++;
    }
    wifiRetryCount = 0;
  }

  // call this routine each loop
  MQTT_connect();
}



// #############################################################################
//  mqtt Connect function
//  This function connects and reconnects as necessary to the MQTT server and
//  WiFi.
//  Should be called in the loop to ensure connectivity
// #############################################################################
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqttclient.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  // Loop until we're reconnected
  while (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttclient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
  Serial.println("MQTT Connected!");
}
