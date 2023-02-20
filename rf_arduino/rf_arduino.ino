#include <SPI.h>
volatile boolean process_send;                    
//char a[]="SENSOR-12-34-56-78\n";                //String to send         
byte pos_send;
char buf [35];
String c;
char a[32];
volatile byte pos;
volatile boolean process_it;
void setup (void)
{
  Serial.begin (9600);                        //Set UART baug rate to 115200
  SPCR |= bit (SPE);                            //Configure ATMEGA328P/Arduino in slave mode
  pinMode(MISO, OUTPUT);                        //Configure MISO as output, SlaveOut
  pos_send=0;
  process_send = false;                           //Initialize flag to FALSE
  pos = 0;   // buffer empty
  process_it = false;
  SPI.attachInterrupt();                        //Enable interrupts for SPI--> You can do that like this too /*SPCR |= bit (SPIE)*/
}  
 
// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;                                //Grab the data byte from the SPI Data Register (SPDR)
 switch(c)
 {
  default:
          if (pos < (sizeof (buf) - 1))
          buf [pos++] = c;
          if (c == '\n')
          process_it = true;
          break;
  case 0:
         if(pos_send < (sizeof(a)-1))
         SPDR = a[pos_send++];                          //process the data byte and put it back into the SPDR for the Master to read it
         else
         process_send = true;                            //Set the Flag  as TRUE
 }
}
 
void loop (void)
{
  if(Serial.available()>0)
{
c=Serial.readStringUntil('\r');
c.toCharArray(a, 32) ;
}
  if (process_it)
    {
    buf [pos] = 0;  
    //Serial.print("data received:");
    Serial.println (buf);
    pos = 0;
    process_it = false;
    }  // end of flag set
  if (process_send)                               //Check if the data has been processed
    {
      pos_send=0;
      process_send = false;                         //Set the Flag to False
    }
}
