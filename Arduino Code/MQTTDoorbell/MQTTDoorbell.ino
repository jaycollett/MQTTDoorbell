#include <ESP8266WiFi.h>
#include <PubSubClient.h>



#define doorbellButtonSensePin 4

#define WIFI_SSID "xxxxxxxxxxxxxx"
#define WIFI_PASS "xxxxxxxxxxxx"
#define MQTT_PORT 1883

char  fmversion[7] = "v1.4";                  // firmware version of this sensor
char  mqtt_server[] = "192.168.0.x";          // MQTT broker IP address
char  mqtt_username[] = "xxxxxxxxxxxxxxx";    // username for MQTT broker (USE ONE)
char  mqtt_password[] = "xxxxxxxxxxxxx";      // password for MQTT broker
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
  }
  
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
  if(digitalRead(doorbellButtonSensePin) == LOW){
    delay(100); // poor man's debounce
     if(digitalRead(doorbellButtonSensePin) == LOW){
      mqttclient.publish(topic.c_str(), "ON");
      Serial.println("Somone hit the doorbell, tell the MQTT server...");
      delay(2000); // give time for the payload to register, and prevent silly doorbell mashers from iratating me further...
      mqttclient.publish(topic.c_str(), "OFF");
     }
  }
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
