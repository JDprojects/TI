#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

// Hotspot Data       *Change to your own*
const char *ssid = "*****";
const char *password = "*****";

WiFiUDP Udp;
WiFiClient client;
ESP8266WebServer server ( 80 );

unsigned int localUdpPort = 4210;  //Port
char incomingPacket[255];         //Buffer

char *replyOn = "00";            // sniffed Data:01
char *replyOff = "01";                  //sniffed Data:00
const int led = LED_BUILTIN;
IPAddress IP (192,168,0,255); //IP Multicast?!
int portOut = 4211;


void handleRoot() {

  server.send ( 200, "text/html", R"=====(<html>
<head>
</head>
<body>
<form method="get" action="/Ein">
    <button type="submit">Ein</button>
</form>
<form method="get" action="/Aus">
    <button type="submit">Aus</button>
</form>
</body>
</html>)=====");
  digitalWrite ( led, HIGH );

}


//Click on "Ein" this Method runs
void handleEin() {

  //http Header with the html
  server.send ( 200, "text/html", R"=====(<html>
<head>
</head>
<body>
Die LED ist an!
</body>
</html>)=====");
  digitalWrite ( led, LOW );
  client.print("1");

  //send UDP-Packet to the IP-Adress and the Port       *IMPORTANT: Change the Port, to the Port your PC uses!*
 Udp.beginPacketMulticast(IP, portOut, WiFi.localIP());
 Udp.write(replyOn);   //send char=0 --> in Wireshark it´s Data:30...
  Udp.endPacket();
}


//Click on "Aus" this Method runs
void handleAus() {

  //http Header with the html
  server.send ( 200, "text/html", R"=====(<html>
<head>
</head>
<body>
Die LED ist aus!
</body>
</html>)=====");
  digitalWrite ( led, HIGH );
  client.print("0");

  //send UDP-Packet to the IP-Adress and the Port       *IMPORTANT: Change the Port, to the Port your PC uses!*
 Udp.beginPacketMulticast(IP, portOut, WiFi.localIP());
 Udp.write(replyOff);    //send int=0
  Udp.endPacket();
}


void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}


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

  server.on ( "/", handleRoot );
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );    //Unnecessary
  } );


  //server.onNotFound ( handleNotFound );
  server.on("/Ein", handleEin);
  server.on("/Aus", handleAus);
  server.onNotFound(handleNotFound);

  //start the Server
  server.begin();
  Serial.println ( "HTTP server started" );

  //Start the UDP. Print the IP-Adress and the Port
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  // replace with the multicast IP-Address from your network
  Udp.beginPacketMulticast(IP, portOut, WiFi.localIP());


}

void loop() {


  // Print the incoming UDP packages. Checks the size     *Important: recognize the Source IP-Adress and Port*
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
   

  }
  //handle the Clients
  server.handleClient();
}
