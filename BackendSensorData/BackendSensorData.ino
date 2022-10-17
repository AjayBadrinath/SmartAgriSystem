/*
 * This Code below is used for collecting raw sensor data and send it via UART Protocol to ESP via Arduino
 */
#include <Servo.h> //Servo library for operating servo .


#define sensorpin A0
#define sensorpower 7
#define servo 8
#include "DHT.h"

Servo servo1;


DHT dht (7,DHT11);

int led = 5;        //For led 

int sensor = 2;     //IR SENSOR 

int motion = LOW;   //For motion detection

class Temp{
  public://visiblity set to global scope
  Temp(){};//dummy constructor referencing to own class
  float Temperature();
  float Humidity();
  
  };
  float Temp::Temperature(){
    return dht.readTemperature();
    delay(2000);
    }
   float Temp::Humidity(){
    return dht.readHumidity();
    delay(2000);
    }
Temp t;
void setup() {
  // put your setup code here, to run once:
  servo1.attach(3);     //Servo Connected to pin 3 as PWM
  
  pinMode(sensorpin,INPUT);//Soil Moisture A0 pin

  
  pinMode(sensorpower,OUTPUT);  //drive soil moisture sensor
  
  digitalWrite(sensorpower,LOW);
  dht.begin();
  Serial.begin(9600);
}

void loop() {
  /*
  IR sensor detection
  */

  if (digitalRead(sensor) == HIGH)
  {
    digitalWrite(led, LOW);

    delay(500);

    if (motion == LOW)

    {
      motion = HIGH;
    }

  }

  else

  {

    digitalWrite(led, HIGH);

    delay(500);

    if (motion == HIGH)

    {
      motion = LOW;
    }

  Serial.print("Soil Moisture: ");
  Serial.println(read_val());
  delay(1500);
  Serial.print("Humidity: ");
  
  float k=t.Humidity();
  Serial.print(k);
  Serial.print("\n");
  delay(1500);
  
  
  Serial.print("Temperature: ");
  Serial.print(t.Temperature());
  
  
  Serial.print("\n"); 
   
  delay(1500);
  if (read_val()+t.Humidity() <80.0){
    
    servo1.write(45);
    delay(1000);
    servo1.write(180);
    delay(1000);
   
    }
    
  delay(1000);
}

float read_val(){
  digitalWrite(sensorpower,HIGH);
  delay(20);
  float a=analogRead(sensorpin);
  digitalWrite(sensorpower,LOW);
  return 100-((a/1024)*100);
   }
