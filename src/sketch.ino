#include <SPI.h>
#include <Ethernet.h>
#include "dht.h"

dht DHT;

#define DHT11_PIN 7
#define INFR_PIN  6
#define LIGHT_D_PIN 4
#define LIGHT_A_PIN 2
#define SOUND_D_PIN 3
#define FIRE_D_PIN 5
#define FIRE_A_PIN 0

#define HTML 0

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFD, 0xED };
IPAddress ip(192,168,0,113);

EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(FIRE_D_PIN,INPUT);
  pinMode(INFR_PIN,INPUT);
  pinMode(SOUND_D_PIN,INPUT);

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

unsigned int sound = 0, body = 0, sound_p = 0, body_p =0;

void loop() {
  EthernetClient client = server.available();
  if(digitalRead(SOUND_D_PIN) == LOW)
      sound ++;

  if(digitalRead(INFR_PIN)==HIGH)
      body ++;
  
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
//	      client.println("Refresh: 1");  // refresh the page automatically every 5 sec
          client.println();
          if(HTML){
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          client.println("<head>");
          client.println("<title>Qiudog.com</title>");
          client.println("</head>");
          client.println("<body>");
          }
          DHT.read11(DHT11_PIN);
          client.print("Temperature=");
          client.print(DHT.temperature,0);
          client.print(";");
          if(HTML)
          client.print("</br>");

          client.print("Humidity=");
          client.print(DHT.humidity,0);
          client.print(";");
          if(HTML)
          client.println("<br/>");
          
          int light_a = analogRead(LIGHT_A_PIN);
          client.print("Light=");
          client.print(1024-light_a);
          client.print(";");
          if(HTML)
          client.println("<br/>");

          client.print("Sound_detected=");
          if(sound == sound_p){
              client.print(0);
          }else{
              client.print(1);
              sound_p = sound;
          }
          client.print(";");
          if(HTML)
          client.println("<br/>");

          client.print("Fire_detected=");
          if(digitalRead(FIRE_D_PIN)==HIGH){
              client.print(0);
          }else{
              client.print(1);
          }
          client.print(";");
          if(HTML)
          client.println("<br/>");

          client.print("Body_detected=");
          if(body==body_p){
              client.print(0);
          }else{
              body_p = body;
              client.print(1);
          }
          if(HTML){
          client.println("<br/>");
          client.println("</body>");
          client.println("</html>");
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
