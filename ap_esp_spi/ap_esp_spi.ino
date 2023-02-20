#include <ESP8266WiFi.h>
#include <SPI.h>
 
#define CS 15 
#define MAXSC 4

char * ssid="espserver";
char * password="password123";
String str;
IPAddress ip;
WiFiServer server(9001);
WiFiClient aclient[MAXSC];

void setup() {
  pinMode(CS,OUTPUT);           //configure the line as output
  Serial.begin(9600);         //Set UART baud rate
  digitalWrite(CS,HIGH);        //Initialize the CS line to HIGH
  WiFi.disconnect();
  //Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  delay(1000);
  ip=WiFi.softAPIP();
  server.begin();
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
}

void loop() {
//  char c;
  if (server.hasClient())
                    {
                      for(int i = 0; i < MAXSC; i++)
                      {
                        //find free/disconnected spot
                        if (!aclient[i] || !aclient[i].connected())
                        {
                          if(aclient[i]) aclient[i].stop();
                          aclient[i] = server.available();
                          Serial.print("New Client : "); Serial.print(String(i+1) + " - ");
                          continue;
                        }
                      }
                      // no free / disconnected spot so reject
                      WiFiClient aclient = server.available();
                      aclient.stop();
                    }
                    
for(int i = 0; i < MAXSC; i++)
 {
             
              if (aclient[i] && aclient[i].connected())
                  {
                    
                  if(aclient[i].available())
                      {
                      while(aclient[i].available()) 
                        {
                         // my code
                          String Message = aclient[i].readStringUntil('\r');
            aclient[i].flush();
            Serial.println("Client No " + String(i) + " - " + Message) ;
            str=Message;
            str+='\n';
            }
}
} }
 digitalWrite(CS,LOW);           //Pull CS Line Low
  for (int i=0 ; i<str.length(); i++)
  {
    delayMicroseconds(30);
    SPI.transfer (str[i]);
    delayMicroseconds(30);
  }
 

 digitalWrite(CS,HIGH);          //Pull CS Line High
  delay(2000);
}
