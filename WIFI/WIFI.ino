#include "WiFi.h"

///SSID of your network
char ssid[] = "Mtnl";
//password of your WPA Network
char password[] = "20830829";

void setup() {
   Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address
   Serial.println("\nMy MAC address: ");
   Serial.print(WiFi.macAddress());
   Serial.println("\n------------------------ ");

    int numberOfNetworks = WiFi.scanNetworks();
     
  for(int i =0; i<numberOfNetworks; i++){
      Serial.print("Network name: ");
      Serial.println(WiFi.SSID(i));
      Serial.print("Signal strength: ");
      Serial.println(WiFi.RSSI(i));
      Serial.printf("BSSID: %s\n", WiFi.BSSIDstr(i).c_str());
      Serial.println("-----------------------");
      }

  }


void loop() {}
