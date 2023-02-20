#include <ESP8266WiFi.h>
char spi_dat[]="12345";
byte d;
char * ssid = "espserver";
char * password = "password123";
IPAddress server(192,168,4,1);
WiFiClient aclient;

void setup() {
  Serial.begin(9600);         //Set UART baud rate
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  checkconnectivity();
  aclient.stop();
  aclient.connect(server,9001);
}

void loop() {
 aclient.println(spi_dat);
 aclient.flush(); 
 delay(2000); 
 checkconnectivity();

}

void checkconnectivity(){
  
    while(WiFi.status() != WL_CONNECTED)
    {
      for(int i=0; i < 10; i++)
      {
        delay(250);
        Serial.print(".");
        delay(250);
      }
      Serial.println("");
    }
  }
