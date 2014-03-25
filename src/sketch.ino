#include <SPI.h>
#include <Ethernet.h>
#include "dht.h"

dht DHT;

#define DHT11_PIN 7
#define INFR_PIN  6
#define LIGHT_D_PIN 4
#define LIGHT_A_PIN 2
#define SOUND_A_PIN 1
#define FIRE_D_PIN 5
#define FIRE_A_PIN 0

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFD, 0xED };
IPAddress ip(192,168,0,113);

EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(FIRE_D_PIN,INPUT);
  pinMode(INFR_PIN,INPUT);

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient client = server.available();
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
	  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          client.println("<head>");
          client.println("<title>Qiudog.com</title>");
          client.println("</head>");
          client.println("<body>");
//          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
//            int sensorReading = analogRead(analogChannel);
//            client.print("analog input ");
//            client.print(analogChannel);
//            client.print(" is ");
//            client.print(sensorReading);
//            client.println("<br />");       
//          }
          DHT.read11(DHT11_PIN);
          client.print("Temperature: ");
          client.print(DHT.temperature,1);
          client.print("</br>");
          client.print("Humidity: ");
          client.print(DHT.humidity,1);
          client.println("%<br/>");
          
          int light_a = analogRead(LIGHT_A_PIN);
          client.print("Light: ");
          client.print(light_a);
          client.println("<br/>");

          int sound_a = analogRead(SOUND_A_PIN);
          client.print("Sound: ");
          client.print(1024-sound_a);
          client.println("<br/>");

          client.print("House on fire: ");
          if(digitalRead(FIRE_D_PIN)==HIGH){
              client.print(0);
          }else{
              client.print(1);
          }
          client.println("<br/>");

          client.print("Some one at home: ");
          if(digitalRead(INFR_PIN)==HIGH){
              client.print(0);
          }else{
              client.print(1);
          }
          client.println("<br/>");
          client.println("</body>");
          client.println("</html>");
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
