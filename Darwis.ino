#define BLYNK_TEMPLATE_ID "TMPL0wBcfkmo"
#define BLYNK_DEVICE_NAME "Darwis FYP"
#define BLYNK_AUTH_TOKEN "_Bfs6qY9VH7Jg8tfyHRQE2VCe1Z5598c";

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>        
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TelegramCertificate.h>
#include <UniversalTelegramBot.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SabunBasuhPinggan";
char pass[] = "159357258";

// Initialize Telegram BOT
#define BOTtoken "2007673673:AAGZZK9jv5gdYZfGWZyoCo2w7Eqp2q6kN1M"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "916111228"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


//DHT11 Blynk
DHT dht;

//PIR Sensor Declaration
int PIRsensor = 13;  //D7

// define the GPIO connected with Relays and switches  
#define RelayPin1 4 //D2

#define VPIN_BUTTON_1  V1


int toggleState_1 = 0; //Define integer to remember the toggle state for relay 1

BlynkTimer timer;

int SoilSensor = A0;  /* Connect Soil moisture analog sensor pin to A0 of NodeMCU */
int sensorvalue = 0;
int outputvalue = 0;

int rainSensor = A0;
int rainSensorVal = 0;
int readSoilVCC;
int readRainVCC;
int Pin_SoilVCC = 5;   //D1
int Pin_RainVCC = 15;  //D8


BLYNK_CONNECTED() 
{  
    // Request the latest state from the server  
    Blynk.syncVirtual(VPIN_BUTTON_1); 
}

// When App button is pushed - switch the state  
BLYNK_WRITE(VPIN_BUTTON_1) 
{
  toggleState_1 = param.asInt();  
  digitalWrite(RelayPin1, toggleState_1);
}
 

void setup() 
{
  Serial.begin(115200); /* Define baud rate for serial communication */


  dht.setup(D4);

  //Relay Pin
  pinMode(RelayPin1, OUTPUT);

  //During Starting all Relays should TURN OFF  
  digitalWrite(RelayPin1, toggleState_1);

  //Rain Sensor
  pinMode(rainSensor,INPUT);

  //PIR Sensor
  pinMode(PIRsensor, INPUT); // declare sensor as input

  //Anolog Sensor VCC Control
  pinMode(Pin_SoilVCC, OUTPUT);
  pinMode(Pin_RainVCC, OUTPUT);

  Blynk.begin(auth, ssid, pass);

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}


int SoilVCCRead() 
{
    digitalWrite(Pin_SoilVCC, HIGH); // Turn Soil Moisture On
    sensorvalue=analogRead(SoilSensor);
    outputvalue=map(sensorvalue,1024, 425, 0, 100); //100,0 pottupaaru
    delay(1000);
    digitalWrite(Pin_RainVCC, LOW); // Turn Rain Sensor Off
}
 
int RainVCCRead() 
{
    digitalWrite(Pin_SoilVCC, LOW); //  Turn Soil Moisture On
    rainSensorVal = analogRead(rainSensor);
    Serial.println(rainSensorVal);
    delay(1000);
    digitalWrite(Pin_RainVCC, HIGH); // Turn Rain Sensor Off
}

void loop() 
{
  //Soil Moisture Sensor Read And Print
  SoilVCCRead();

    if(outputvalue>65)
    {
      Serial.println("Soil is Wet. Please Turn Off The Pump");
      Serial.print(outputvalue);
      digitalWrite(RelayPin1, LOW);
      bot.sendMessage(CHAT_ID, "Soil is Wet.  Please Turn Off The Pump", "");
      delay(1000); 
    }

    else if(outputvalue<25)
    {
      Serial.println("Water your Plant. Please Turn On The Pump");
      Serial.print(outputvalue);
      digitalWrite(RelayPin1, HIGH);
      bot.sendMessage(CHAT_ID, "Water your Plant. Please Turn On The Pump", "");
      delay(1000);
    }

    Blynk.virtualWrite(V2,sensorvalue);
    delay(5000);

 
    //Rain Sensor Read and Print
    RainVCCRead();
    delay(200);

    Blynk.virtualWrite(V5,rainSensorVal);
    delay(5000);

    //PIR Sensor Read and Print
    long state = digitalRead(PIRsensor);
    if(state == HIGH) 
    {
      Serial.println("Motion detected!");
      delay(1000);
    }
    else 
    {
      Serial.println("Motion absent!");
      delay(1000);
    }

    Blynk.virtualWrite(V6,state);
   

    //To Update Serial Print With Latest Pump State
    if(toggleState_1 == 1)
    {     
      Serial.println("Pump ON");
    }

    else
    {   
        Serial.println("Pump OFF");  
    }  
  
  delay(500);

  //DHT11 Sensor Read And Print
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");
  delay(dht.getMinimumSamplingPeriod()); /* Delay of amount equal to sampling period */
  float humidity = dht.getHumidity();/* Get humidity value */
  float temperature = dht.getTemperature();/* Get temperature value */
  Serial.print(dht.getStatusString());/* Print status of communication */
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, temperature);

  delay(500);

    //Blynk WiFi Status Update
    if (WiFi.status() != WL_CONNECTED)  
    {  
      Serial.println("WiFi Not Connected");  
    }  
    else  
    {  
      Serial.println("WiFi Connected");    
    } 

    Blynk.run();
}
