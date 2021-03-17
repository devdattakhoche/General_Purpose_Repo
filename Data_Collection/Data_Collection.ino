#include "WiFi.h"
#define MAX_NETWORKS 10

double features[4];
double avg[4]={0};
double n[4]={1};
String knownNetworks[10] = {"Mtnl","TP-LINK_48BD21","Tenda_743E68","Shetty18"};
int cnt = 0;
int len = sizeof(knownNetworks) / sizeof(knownNetworks[0]);
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

int getIndex(String s) {
  int flag = 0;
  for (int i = 0; i < len; i++) { 
    if (knownNetworks[i] == s) {
      flag = 1;
      return i;
    }
  }
  if (flag == 0) return -1;
}

void loop() {
  if(cnt==200){
        Serial.println("=============================================");
    delay(20000);
    cnt=0;
  }
  scan();
  printFeatures();
  cnt++;
}
void printFeatures() {
  const uint16_t numFeatures = sizeof(features) / sizeof(double);

  for (int i = 0; i < numFeatures; i++) {
    Serial.print(features[i]);
//    avg[i]=(avg[i]+features[i]);
//    Serial.println((avg[i]/n[i])*0.5 + features[i]*0.5);
//    n[i]++;
    Serial.print(i == numFeatures - 1 ? '\n' : ',');
  }
}
void scan() {
  int numNetworks = WiFi.scanNetworks();

  resetFeatures();

  // assign RSSIs to feature vector
  for (int i = 0; i < numNetworks; i++) {
    String ssid = WiFi.SSID(i);
    
    int networkIndex = getIndex(ssid);

    // only create feature if the current SSID is a known one
    if (networkIndex != (-1)) {
      features[networkIndex] = WiFi.RSSI(i);
    }
  }

}

void resetFeatures() {
  const uint16_t numFeatures = sizeof(features) / sizeof(double);
  for (int i = 0; i < numFeatures; i++)
    features[i] = 0;
}
