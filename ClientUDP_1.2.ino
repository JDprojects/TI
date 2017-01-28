#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

const char *ssid = "*****";
const char *password = "******";

WiFiUDP Udp;
WiFiClient client;

unsigned int localUdpPort = 4211;  //Port
char incomingPacket[255];         //Buffer
const int led = LED_BUILTIN;             

const char *an = "00";
const char *aus = "01";
void setup() {
  
 pinMode ( led, OUTPUT );
 digitalWrite ( led, LOW );
 
 Serial.begin ( 115200 );
 
 WiFi.begin ( ssid, password );
 Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  Serial.println("Starting connection to server...");
  Udp.begin(localUdpPort);
}

void loop(){ // to check every packet and it´s size, accordingly to the incomingPackets the esp will act 

// Print the incoming UDP packages. Checks the size 
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

     
   if ( strcmp(incomingPacket, an  ) )
    {
    digitalWrite ( led, HIGH);
    } 
    if ( strcmp(incomingPacket, aus  ) )
    {
    digitalWrite ( led, LOW);
    }
    
    }
 }

