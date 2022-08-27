#include <SoftwareSerial.h>
#include <dht11.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define TX 2
#define RX 3
#define dht_apin 11 // Analog Pin sensor is connected to
#define LEDR 6 //Led
#define LEDG 7//Led
#define fan 8
dht11 dhtObject;
String AP = "Dialog 4G";       // AP NAME
String PASS = "TRDD76R99RH"; // AP PASSWORD
String API = "2UWIAKZO8QTUQMDE";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
const int buzzer = 5;//buzzer
int alarm;//aram value to methode
SoftwareSerial mySerial(13,12);
SoftwareSerial esp8266(TX,RX); 
LiquidCrystal_I2C lcd(0x27, 16, 2);
  
void setup() {
  lcd.begin();
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(fan, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(buzzer, OUTPUT);//buzzer
  pinMode(4,INPUT);//fire
  pinMode(A0,INPUT);//gas vsensor
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
 
 /*-----------------------------------------------------------*/
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Gas : "+getgasValue());
  lcd.setCursor(0,1);
  lcd.print("Temperature : "+getTemperatureValue());
  /*----------------------------------------------------------*/
  warningAlarm();
  /*----------------------------------------------------------*/
 String getData = "GET /update?api_key="+ API +"&field1="+getTemperatureValue()+"&field2="+getHumidityValue()+"&field3="+getgasValue()+"&field4="+getfireValue();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void warningAlarm()
{
alarm=getgastoValue();
if(alarm>50)
 {
  digitalWrite(fan,HIGH);
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
  tone(buzzer,1000);
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Gas Leaking");
  lcd.setCursor(0,1);
  lcd.print("");
  sms();
 }
 else
 {
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);
  noTone(buzzer);
 }
}


String getTemperatureValue(){

   dhtObject.read(dht_apin);
   Serial.print(" Temperature(C)= ");
   int temp = dhtObject.temperature;
   Serial.println(temp); 
   delay(50);
   return String(temp); 
  
}


String getHumidityValue(){

   dhtObject.read(dht_apin);
   Serial.print(" Humidity in %= ");
   int humidity = dhtObject.humidity;
   Serial.println(humidity);
   delay(50);
   return String(humidity); 
  
}


String getgasValue(){
   Serial.print(" Gas(level)= ");
   int val=analogRead(A0); 
   Serial.println(val); 
   delay(50);
   return String(val); 
  
}

int getgastoValue(){
   Serial.print(" Gas(level)= ");
   int val=analogRead(A0); 
   Serial.println(val); 
   delay(50);
   return int(val);   
}

String getfireValue(){
   Serial.print("fire= ");
   int val=digitalRead(4); 
   Serial.println(val); 
   delay(50);
   return String(val);  
}


void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
 void sms()
{
  Serial.println("Initializing..."); 
  delay(1000);
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+94772303895\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("Gas Leakage detected"); //text content
  updateSerial();
  mySerial.write(26);
  delay(30000);  
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
 
