#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

WiFiUDP Udp;
IPAddress maddr;
const char* serverip = "192.168.10.1";
const int serverport = 8889;
const int localport = 1518;
const char* ssid     = "TELLO-9C845C";
const char* password = "";
char incomingPacket[255];              

const char movelist[][15] = {"takeoff","up 400","flip b","land"};/*"lift 100","right 100","foward 100","back 100","down 50","cw 360","ccw 360","left 50","right 100","left 50","forward 50","back 100","forward 50",*/
void udpWriteCmd(const char* cmd){
  Udp.beginPacket(maddr, serverport);
  Udp.write(cmd, strlen(cmd));
  Udp.endPacket();
}

void doNextMove(){
  static unsigned int idx = 0;
  if(idx < sizeof(movelist)/sizeof(*movelist))
    udpWriteCmd(movelist[idx++]);
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Init"));
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F(" connected"));
  Udp.begin(localport);
  maddr.fromString(serverip);
  udpWriteCmd("command");
  delay(100);
  doNextMove();
}

unsigned long lasttick = 0;
void loop() {
  // put your main code here, to run repeatedly:
  int packetSize = Udp.parsePacket();
  if (packetSize){
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0){
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
  }
  unsigned long thistick = millis();
  if(thistick - (lasttick + 2000)< 0xff000000){
    
    doNextMove();
    lasttick = thistick;
  }
}
