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

 
