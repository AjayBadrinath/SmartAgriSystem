#include <WiFi.h>

#include "ThingSpeak.h"

const char* ssid="SSN";
const char* passwd="";

#define RXp2 16//UART 
#define TXp2 17//UART

WiFiServer server(80);
WiFiClient client;

unsigned long channel=1;
/*
 * API KEYS
 
 */
const char* API="";
const char* API_H="";
const char * API_S="";

String header;
/*Time to update graph*/
unsigned long lT=0;
unsigned long TimDelay=30000;
/*compute WiFi timeout to reconnect*/
unsigned long currentTime=millis();

unsigned long prev=0;

const long tout=2000;
int ref=0;


void setup(){
  Serial.begin(115200);// Baudrate
  Serial2.begin(9600,SERIAL_8N1,RXp2,TXp2);//UART
  
  Serial.print("Connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid,passwd);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.println("Wifi connected");
  Serial.println("IP addr:");
  Serial.println(WiFi.localIP());
  server.begin();
  ThingSpeak.begin(client);
  }


void loop(){
  WiFiClient client=server.available();
  
  if (client){
    currentTime=millis();
    prev=currentTime;
    Serial.println("New Client");
    String currentLine="";
  
   while (client.connected() && currentTime - prev <= tout) {  // loop while the client's connected
      currentTime = millis();
   
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //float c= client.read();
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        
        if (c == '\n') {                    
          
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            ref=ref+1;
            
            
            
            client.println("<!DOCTYPE html><html>");
            client.print("<head>");
            
            client.print("<meta http-equiv=\"refresh\" content=\"30\">");//refresh every 30 sec prev set to 2s
            client.println("<meta name=\"viewport\"  content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            
            
            client.println("</style></head>");
            client.println("<body><h1> Temperature And Humidity</h1>");

            Serial.println("===");
            String xk=Serial2.readStringUntil('\n');
            
            int idx1,idx2,idx3;
            float flt_val;
            String tmp;
            
            Serial.println(xk);
            
            if (xk.indexOf("T")==0){
               tmp=xk.substring(13,18);
               flt_val=tmp.toFloat();
               client.println("<body><p>Temperature:");
              client.println(flt_val);
              client.println("</p>");   
               
              Serial.println("=======");
              Serial.println(flt_val);
              Serial.println("=======");
              ThingSpeak.writeField(1,1,flt_val,API);
            }
            else if (xk.indexOf("H")==0){
              tmp=xk.substring(10,15);
               flt_val=tmp.toFloat();
                client.println("<body><p>Humidity:");
               client.println(flt_val);
              client.println("</p>");  
              Serial.println("=======");
              Serial.println(flt_val);
              Serial.println("=======");
              ThingSpeak.writeField(2,1,flt_val,API_H);
            }
            else if(xk.indexOf("S")==0){
              tmp=xk.substring(14,19);
              float flt_val=tmp.toFloat();
               client.println("<body><p>Soil Moisture:");
              client.println(flt_val);
              client.println("</p>");  
              Serial.println("=======");
              Serial.println(flt_val);
              Serial.println("=======");
              ThingSpeak.writeField(3,1,flt_val,API_S);
            }
            
            client.println("<table border =3>");
            client.println("<tr><td colspan=3><h1 align =\"center\">Dashboard</h1></td></tr>");
            client.println("<tr><td><iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/1890291/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&title=Temparature&type=line&xaxis=Time&yaxis=Temparature\"></iframe></td>");
            client.println("<td><iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/1892240/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&title=Humidity&type=line&xaxis=Time&yaxis=Humidity+%25\"></iframe></td>");      
            client.println("<td><iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/1892713/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&xaxis=Time&yaxis=SoilMoisture%25\"></iframe></td>");       
            client.println("</tr></table>");
            client.println("</body></html>");
            
            
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;
          currentLine+='\n';      
        }
      }
   }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
