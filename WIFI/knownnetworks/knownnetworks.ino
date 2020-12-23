
#include "WiFi.h"
#define MAX_NETWORKS 10

//using namespace Eloquent::DataStructures;

double features[3];
String knownNetworks[10] = {"Mtnl", "Galaxy A51E621", "TP-LINK_48BD21"};
int cnt = 0;
int len = sizeof(knownNetworks) / sizeof(knownNetworks[0]);
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  //    Serial.println(getIndex("Galaxy A51E621"));
  // and so on
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
  scan();
  printFeatures();
  //    Serial.println("\n");
  //  delay(1000);
}
void printFeatures() {
  const uint16_t numFeatures = sizeof(features) / sizeof(double);

  for (int i = 0; i < numFeatures; i++) {
    Serial.print(features[i]);
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
      //          Serial.println(ssid);
      //          Serial.println(networkIndex);
      features[networkIndex] = WiFi.RSSI(i);
    }

    //    else if(cnt<10){
    //      for (i = 0; i < len; i++) {
    //        if (knownNetworks[i] == "") {
    //          cnt = cnt + 1;
    //          Serial.println(ssid);
    //          knownNetworks[i] = ssid;
    //          break;
    //        }
    //      }
    //    }
  }

}

void resetFeatures() {
  const uint16_t numFeatures = sizeof(features) / sizeof(double);

  for (int i = 0; i < numFeatures; i++)
    features[i] = 0;

}
