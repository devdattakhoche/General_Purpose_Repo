#include "certs.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"


// Wifi credentials
const char *WIFI_SSID = "Galaxy A51E621";
const char *WIFI_PASSWORD = "15412342";

// The name of the device. This MUST match up with the name defined in the AWS console
#define DEVICE_NAME "GOT8"

// The MQTTT endpoint for the device (unique for each AWS account but shared amongst devices within the account)
#define AWS_IOT_ENDPOINT "akhy31d6knuov-ats.iot.us-east-2.amazonaws.com"

// The MQTT topic that this device should publish to
//#define AWS_IOT_TOPIC "$aws/things/" DEVICE_NAME "/shadow/update"
#define AWS_IOT_TOPIC "RSSI"

// How many times we should attempt to connect to AWS
#define AWS_MAX_RECONNECT_TRIES 50

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retries = 0;
//  while (WiFi.status() != WL_CONNECTED && retries < 15){
    Serial.print("Connecting to WIFI : ");
    while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    
    Serial.print("*");
    retries++;
  }
  if(WiFi.status() != WL_CONNECTED){
    esp_sleep_enable_timer_wakeup(1 * 60L * 1000000L);
    esp_deep_sleep_start();
  }else Serial.print(" Connected!");
}

void connectToAWS()
{
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  int retries = 0;
  Serial.print("\nConnecting to AWS IOT : ");

  while (!client.connect(DEVICE_NAME) && retries < AWS_MAX_RECONNECT_TRIES) {
    Serial.print("%");
    delay(100);
    retries++;
  }
  if(!client.connected()){
    Serial.println(" Timeout!");
    return;
  }
  Serial.println(" Connected!");
}

void sendJsonToAWS()
{
  StaticJsonDocument<200> jsonDoc;
//  JsonObject stateObj = jsonDoc.createNestedObject("state");
  
  jsonDoc["deviceId"] = WiFi.macAddress();
  int numberOfNetworks = WiFi.scanNetworks();
     
  for(int i =0; i<numberOfNetworks; i++)
//       jsonDoc[WiFi.BSSIDstr(i)] = WiFi.RSSI(i);
//        Serial.println(WiFi.SSID(i));
//        Serial.println(WiFi.BSSIDstr(i));
       jsonDoc[WiFi.SSID(i)] = WiFi.RSSI(i);

  Serial.println("Publishing message to AWS...");
  char jsonBuffer[512];
  serializeJson(jsonDoc, jsonBuffer);

  client.publish(AWS_IOT_TOPIC, jsonBuffer);
}

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  connectToAWS();
}

void loop() {
  sendJsonToAWS();
  client.loop();
  delay(1000);
}
