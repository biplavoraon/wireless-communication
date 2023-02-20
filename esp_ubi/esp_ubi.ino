#include <SPI.h>
#include <ESP8266WiFi.h>
#define CS 15                   //define chip select line for manual control
char spi_dat[35];
byte d;
const int sleep_time = 20;  // Time to sleep (in seconds) between posts to Ubidots
WiFiClient client;
String variable_id1 = "597465cec03f972ca45a79b1";
String token = "A1E-yeZHp4IWwO2VKnLr5QC6GHIcvFXKIV";

// We'll also initialize the values for our Wi-Fi network
const char* ssid = "SNR_Arvind";
const char* password = "SNRarvind";

//////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
void ubiSave_value(String, String);

void setup()
{
  pinMode(CS,OUTPUT);           //configure the line as output
  Serial.begin(9600);         //Set UART baud rate
  digitalWrite(CS,HIGH);        //Initialize the CS line to HIGH
  SPI.begin();                  //Initialize the SPI module--> configures the MOSI, MISO and CLOCK lines
 
  /*Configure the SPI bus as follows
  1. SPI bus speed = 1 MHz
  2. Data Out = From MSB bit ---> To LSB bit
  3. Data Mode = SPI MODE0*/
SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
 // Debug messsages to indicate we're about to connect to the network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Use the scanNetworks method inside the Wi-Fi class to scan for any available Wi-Fi networks
  // nearby. If none are found, go to sleep
  int n = WiFi.scanNetworks();

  Serial.println("scan done"); 
  if (n == 0)
  {
    Serial.println("no networks found");
   // Serial.println("Going into sleep");
// ESP.deepSleep(sleep_time * 1000000);
  }

  // If networks are found, attempt to connect to our Wi-Fi network
  WiFi.begin(ssid, password);

  // While the connection is not established, IDLE inside this while loop
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Once the connection to our Wi-Fi netowrk is successful, print some debug messages
  Serial.println("");
  Serial.println("Wi-Fi connected");
}
 
void loop()
{
 digitalWrite(CS,LOW);           //Pull CS Line Low
 SPI.transfer(0);             //Send a byte (0x02) to the slave i.e. Arduino UNO
 delayMicroseconds(30);
  for(byte pos=0;pos < (sizeof(spi_dat)-1);pos++)
  {
  spi_dat[pos] = SPI.transfer(0);   //Received the processed data byte from the slave
  delayMicroseconds(30);
  if(spi_dat[pos]=='\n')
  {
   d=++pos;
  break;
  }
  }
  spi_dat[d]=0;
  
   digitalWrite(CS,HIGH);          //Pull CS Line High
   Serial.print("data received:");
   Serial.println(spi_dat);
   String temp(spi_dat);
   ubiSave_value(String(variable_id1), String(temp));

  // Send some debug messages over USB
 // Serial.println("Ubidots data");
  Serial.println("data: "+ temp);
//  Serial.println(" Going to Sleep for a while !" );

  // deepSleep time is defined in microseconds. Multiply seconds by 1e6
  //ESP.deepSleep(sleep_time * 1000000);//one or other
  
  // Wait a few seconds before publishing additional data to avoid saturating the system
 // delay(sleep_time*1000);
//  Serial.println(spi_dat);          //UART - Print the data received from the slave
//  delay(4000);                    //Delay of 4s
}

void ubiSave_value(String variable_id, String value)
{
  // Prepare the value that we're to send to Ubidots and get the length of the entire string
  // that's being sent
  String var = "{\"value\": " + value +"}"; // We'll pass the data in JSON format
  String length = String(var.length());

  // If we get a proper connection to the Ubidots API
  if (client.connect("things.ubidots.com", 80))
  {
    //Serial.println("Connected to Ubidots...");
    delay(100);

    // Construct the POST request that we'd like to issue
    client.println("POST /api/v1.6/variables/"+variable_id+"/values HTTP/1.1");
    // We also use the Serial terminal to show how the POST request looks like
   // Serial.println("POST /api/v1.6/variables/"+variable_id+"/values HTTP/1.1");
    // Specify the contect type so it matches the format of the data (JSON)
    client.println("Content-Type: application/json");
   // Serial.println("Content-Type: application/json");
    // Specify the content length
    client.println("Content-Length: "+ length);
   // Serial.println("Content-Length: "+ length);
    // Use our own API token so that we can actually publish the data
    client.println("X-Auth-Token: "+ token);
    //Serial.println("X-Auth-Token: "+ token);
    // Specify the host
    client.println("Host: things.ubidots.com\n");
   // Serial.println("Host: things.ubidots.com\n");
    // Send the actual data
    client.print(var);
   // Serial.print(var+"\n");
  }
  else
  {
    // If we can't establish a connection to the server:
    Serial.println("Ubidots connection failed...");
  }
  
  // If our connection to Ubidots is healthy, read the response from Ubidots
  // and print it to our Serial Monitor for debugging!
  while (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
  
  // Done with this iteration, close the connection.
//  if (client.connected())
//  {
//    Serial.println("Disconnecting from Ubidots...");
//    client.stop();
//  }
}
