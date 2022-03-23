//==============New Blynk IoT===============//
#define BLYNK_TEMPLATE_ID "TMPLyFFOEjzV"
#define BLYNK_DEVICE_NAME "PROJECTEVAPFARM"

#define BLYNK_FIRMWARE_VERSION        "0.2.0" 
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_WROVER_BOARD
#include "BlynkEdgent.h"
#include <SimpleTimer.h>
//==============New Blynk IoT===============//

//==================NTP=====================//
#include <NTPClient.h>
#include <WiFiUdp.h>
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
//==================NTP=====================//

//==MCU Digital Pin Check blynk connected===//
#define ledblynk           15
//==MCU Digital Pin Check blynk connected===//

//============Blynk Virtual Pin============//
//V0    sw1_pevap_status
//V1    sw1_pevap       ปั้มน้ำ
//V2    sw2_led         ไฟปลูก
//V3    sw3_airflow     พัดลม
//V4    sw4_heater      ตัวทำความร้อน
//V5    Humi1
//V6    Temp1
//V7    Light1
//V8    Windspeed
//V9    CurrentDate
//V10   CurrentTime
//V11   SoilMoisture
//============Blynk Virtual Pin============//

//===========ปุ่ม เปิด-ปิด sw1_pevap============//
#define Relay1_sw1_pevap               26
#define Widget_Btn_sw1_pevap           V1   

#define Widget_Led_sw1_pevap           V0
WidgetLED LedBlynk_sw1(Widget_Led_sw1_pevap);
//===========ปุ่ม เปิด-ปิด sw1_pevap============//
     
//=========ปุ่ม เปิด-ปิด sw2_led===========//
#define Relay2_sw2_led              25
#define Widget_Btn_sw2_led          V2          
//=========ปุ่ม เปิด-ปิด sw2_led===========//

//=========ปุ่ม เปิด-ปิด sw3_airflow===========//
#define Relay3_sw3_airflow            33
#define Widget_Btn_sw3_airflow        V3          
//=========ปุ่ม เปิด-ปิด sw3_airflow===========//


//==========ปุ่ม เปิด-ปิด sw4_heater============//
#define Relay4_sw4_heater             32
#define Widget_Btn_sw4_heater         V4               
//==========ปุ่ม เปิด-ปิด sw4_heater============//

//==================Modbus================//
#include <ModbusMaster.h>
///////// PIN /////////
#define MAX485_DE             18    //DE
#define MAX485_RE_NEG         19    //RE
#define RX2                   16    //RO,RX
#define TX2                   17    //DI,TX
// instantiate ModbusMaster object
ModbusMaster node1;     //3IN1 Humi,Temp, Lux
ModbusMaster node2;     //Windspeed 
ModbusMaster node3;     //SoilMoisture
//==================Modbus================//

//====Modbus Pre & Post Transmission1====//
void preTransmission1()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}
void postTransmission1()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}
//====Modbus Pre & Post Transmission1====//

//====Modbus Pre & Post Transmission2====//
void preTransmission2()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}
void postTransmission2()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}
//====Modbus Pre & Post Transmission2====//

//====Modbus Pre & Post Transmission3====//
void preTransmission3()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}
void postTransmission3()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}
//====Modbus Pre & Post Transmission3====//

//=============Setup Function============//
void setup()
{
  //Modbus pinMode
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2); //RX2=16,RO ,TX2=17, DI
 
  node1.begin(1, Serial2);
  node1.preTransmission(preTransmission1);
  node1.postTransmission(postTransmission1);

  node2.begin(2, Serial2);
  node2.preTransmission(preTransmission2);
  node2.postTransmission(postTransmission2);
  
  node3.begin(3, Serial2);
  node3.preTransmission(preTransmission3);
  node3.postTransmission(postTransmission3);
  
  // Setup Pin Mode
  pinMode(Relay1_sw1_pevap,OUTPUT);         // ESP32 PIN gpio26
  pinMode(Relay2_sw2_led,OUTPUT);           // ESP32 PIN GPIO25
  pinMode(Relay3_sw3_airflow,OUTPUT);       // ESP32 PIN GPIO33   
  pinMode(Relay4_sw4_heater,OUTPUT);        // ESP32 PIN GPIO32
  pinMode(ledblynk,OUTPUT);                 // ESP32 PIN GPIO15         
  
  // Set Defult Relay Status
  digitalWrite(Relay1_sw1_pevap,LOW);       // ESP32 PIN gpio26
  digitalWrite(Relay2_sw2_led,LOW);         // ESP32 PIN GPIO25
  digitalWrite(Relay3_sw3_airflow,LOW);     // ESP32 PIN GPIO33   
  digitalWrite(Relay4_sw4_heater,LOW);      // ESP32 PIN GPIO32   
  digitalWrite(ledblynk,HIGH);              // ESP32 PIN GPIO15  
  
  BlynkEdgent.begin();
  timer.setInterval(10000L,datetime);
  timer.setInterval(5000L, GzwsSensorData);
  timer.setInterval(5000L,WindSensorData);
  timer.setInterval(5000L,SoilMoisture1);
}
//=============Setup Function============//

//========GZWS Humi&Temp and Light=======//
void GzwsSensorData(){
//======node1 Slave ID 1======// 
  uint8_t result1; 
  float humi1 = (node1.getResponseBuffer(0)/10.0f);
  float temp1 = (node1.getResponseBuffer(1)/10.0f);
  float light1 = (node1.getResponseBuffer(2));
  float light_per1;
//======node1 Slave ID 1======// 

//======node1 Slave ID 1======//   
  Serial.println("GZWS Data1");
  result1 = node1.readHoldingRegisters(0x0000, 3); // Read 3 registers starting at 1)
  if (result1 == node1.ku8MBSuccess)
  {
    Serial.print("Humi1: ");
    Serial.println(node1.getResponseBuffer(0)/10.0f);
    Serial.print("Temp1: ");
    Serial.println(node1.getResponseBuffer(1)/10.0f);
    light_per1 = (light1 = node1.getResponseBuffer(2));
    light_per1 = map(light_per1, 0,65535, 0,100);   
    Serial.print("Light1: ");
    Serial.println(light_per1);
  }
  delay(1000);
  Blynk.virtualWrite(V5,humi1);
  Blynk.virtualWrite(V6,temp1);
  Blynk.virtualWrite(V7,light_per1);
}
//========GZWS Humi&Temp and Light=======//

//==============WindSpeed Sensor===========//
void WindSensorData(){
uint8_t result;
  float wind01 = (node2.getResponseBuffer(0)); //Change m/s to km/h= m/s *3.6, Change km/h to m/s = km/h /3.6 
  Serial.println("Get Wind Speed Data");
   result = node2.readHoldingRegisters(0x0000, 1); // Read 2 registers starting at 1)
  if (result == node2.ku8MBSuccess)
  {
    Serial.print("Winspeed: ");
    Serial.println(node2.getResponseBuffer(0));
  }
  delay(1000);
  Blynk.virtualWrite(V8, wind01);
}
//==============WindSpeed Sensor===========//

//===========SoilMoisture Sensor1==========//
void SoilMoisture1(){ 
  uint8_t result1;  
  float soil_01 = (node3.getResponseBuffer(2)/10.0f);
  
  Serial.println("Get Soil Moisture Data1");
  result1 = node3.readHoldingRegisters(0x0000, 3); // Read 2 registers starting at 1)
  if (result1 == node3.ku8MBSuccess)
  {
    Serial.print("Soil Moisture Sensor1: ");
    Serial.println(node3.getResponseBuffer(2)/10.0f);
  }
  delay(1000);
  Blynk.virtualWrite(V11,soil_01);
}
//===========SoilMoisture Sensor1==========//

//===========Display Date&Time=============//
void datetime() {
// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(25200);

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  Blynk.virtualWrite(V9, dayStamp);
  
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  Blynk.virtualWrite(V10, timeStamp);  
  delay(1000);
}
//===========Display Date&Time===========//

//=============Blynk conneted============//
BLYNK_CONNECTED()
{
 if (Blynk.connected())
 {
    Serial.println("Blynk Connected");
    digitalWrite(ledblynk, LOW);  //ledpin for check blynk connected 
    Serial.println("Blynk connected");
    Blynk.syncAll();
 }
}
//=============Blynk conneted============//

//==============Loop Function============// 
void loop() {      
  BlynkEdgent.run();   
  if (Blynk.connected())
    {
      Blynk.run();
    } 
      timer.run();
}
//==============Loop Function============// 

//==========Manual Switch1 pevap=========//
BLYNK_WRITE(Widget_Btn_sw1_pevap){     
      int valuebtn1 = param.asInt();
      if(valuebtn1 == 1){
       digitalWrite(Relay1_sw1_pevap,HIGH);     
       Blynk.virtualWrite(Widget_Led_sw1_pevap, "color", "#24F70A");
       Blynk.setProperty(Widget_Led_sw1_pevap , "label", "เปิดปั้มน้ำ");
       LedBlynk_sw1.on(); 
      }
       else{              
       digitalWrite(Relay1_sw1_pevap,LOW);   
       Blynk.setProperty(Widget_Led_sw1_pevap , "label", "ปิดปั้มน้ำ");
       LedBlynk_sw1.off();
     }
}
//==========Manual Switch1 pevap=========//

//===========Manual Switch2 Led==========//
BLYNK_WRITE(Widget_Btn_sw2_led){
      int valuebtn2 = param.asInt();
      if(valuebtn2 == 1){
        digitalWrite(Relay2_sw2_led,HIGH);        
      }
       else{              
        digitalWrite(Relay2_sw2_led,LOW);        
     }
}
//===========Manual Switch2 Led==========//

//=========Manual Switch3 airflow========//
BLYNK_WRITE(Widget_Btn_sw3_airflow){
      int valuebtn3 = param.asInt();
      if(valuebtn3 == 1){
        digitalWrite(Relay3_sw3_airflow,HIGH);        
      }
       else{              
        digitalWrite(Relay3_sw3_airflow,LOW);        
     }
}
//=========Manual Switch3 airflow========//

//=========Manual Switch4 heater=========//
BLYNK_WRITE(Widget_Btn_sw4_heater){
      int valuebtn4 = param.asInt();
      if(valuebtn4 == 1){
        digitalWrite(Relay4_sw4_heater,HIGH);        
      }
       else{              
        digitalWrite(Relay4_sw4_heater,LOW);        
     }
}
//=========Manual Switch4 heater=========//
