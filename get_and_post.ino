
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include<SPI.h>
#include <LiquidCrystal_I2C.h>
#define ENGINE_D0 D0 // D0 pin for relay module.

const char* ssid     = "SSID";
const char* password = "PASSWORD";

const char* host = "http://iot.roootx.tech";
String apiKeyValue = "cNTSG123zgnD";

// Choose two Arduino pins to use for softwarge serial
int RXPin = D4; 
int TXPin = D3; 
const int chipSelect = D8;
int GPSBaud = 9600;

TinyGPSPlus gps;

SoftwareSerial gpsSerial(RXPin, TXPin);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(D0, OUTPUT);
digitalWrite(ENGINE_D0, HIGH); //Turn OFF Engine
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  
  
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  lcd.setCursor(0, 0);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
   Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  Serial.print("Initializing SD card...");
  
  lcd.setCursor(1,1);
  lcd.print("Initializing SD");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Initialization");
    lcd.setCursor(3, 1);
    lcd.print("Faild!");
    while (1);
  }
  Serial.println("initialization done.");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done...");

}
void loop()
{
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("test test");
  String dataString = "";
  String dataString2 = "";
  String dataString3 = "";
  String dataString4 = "";
  String dataString5 = "";
  String dataString6 = "";
  
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
//      displayInfo();


///////////////////////////////////display data part///////////////////////////////////////////////////////////////////////////

  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    Serial.print("Number of Satellites: ");
    Serial.println(gps.satellites.value(), 6);
    Serial.print("Speed Km/h: ");
    Serial.println(gps.speed.kmph(), 6);

   
  }
  else
  {
    Serial.println("Location: Not Available");
     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Location");
    lcd.setCursor(2, 1);
    lcd.print("Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
    Serial.println();
    Serial.print("Number of Satellites: ");
    Serial.println(gps.satellites.value(), 6);
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  delay(500);


      
 if (gps.location.isValid())
  {    
    dataString = String(gps.altitude.meters(), 3);
    dataString2 = String(gps.location.lat(), 6);
    dataString3 = String(gps.location.lng(), 6); 
    dataString5 = String(gps.speed.kmph(), 6);
    dataString6 = String(gps.satellites.value(), 6);
    
  File dataFile = SD.open("GpsDat.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print("Speed Km/h: ");
    dataFile.println(dataString5);
    dataFile.print("Satellites: ");
    dataFile.println(dataString6);
    dataFile.print("Altitude: ");
    dataFile.println(dataString);
    dataFile.print("Longtitude: ");
    dataFile.println(dataString2);
    dataFile.print("Latitude: ");
    dataFile.print(dataString3);
    dataFile.print("Time: ");
    if (gps.time.hour() < 10) dataFile.print(F("0"));
    dataFile.print(gps.time.hour());
    dataFile.print(":");
    if (gps.time.minute() < 10) dataFile.print(F("0"));
    dataFile.print(gps.time.minute());
    dataFile.print(":");
    if (gps.time.second() < 10) dataFile.print(F("0"));
    dataFile.print(gps.time.second());
    dataFile.print(".");
    if (gps.time.centisecond() < 10) dataFile.print(F("0"));
    dataFile.println(gps.time.centisecond());
    dataFile.println();
    dataFile.close(); 
    
  }
  else
  {
    Serial.println("Failed to open file"); 
    Serial.println();
  }

////////////////////////////////server part//////////////////////////////////////////////////////////////////////

  // Wait a few seconds between measurements.
  //delay(2000);
  if(WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    HTTPClient http;  // Declare object of class HTTPClient.
 

    http.begin(client, "http://iot.roootx.tech/post-esp-data.php");
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(dataString) + 
                             "&value2=" + String(dataString2) + 
                             "&value3=" + String(dataString3)+ 
                             "&value4=" + String(dataString5)+ "&value5=" +String(dataString6)  + "";
    Serial.print("httpRequestData: ");
    
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);

     if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  delay(100);  


//////////////////////////POST_METHORD////////////////////////////////////////////////////////////////////////////////
WiFiClient client;
HTTPClient http;
String GetAddress, LinkGet, getData;
int id = 0;   // ID in Database
GetAddress = "post/GetData.php";
LinkGet = host + GetAddress; // create link for data comunication.
getData = "ID=" + String(id);
Serial.println("Connect to Server");
Serial.println("Get Engine Status from Server");
Serial.println("Request Link : ");
Serial.println(LinkGet);
http.begin(client, "http://iot.roootx.tech/GetData.php"); //get file location in server.
http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header.
int httpCodeGet = http.POST(getData); // Send the request to server.
String payloadGet = http.getString(); // Get the response data from server.
Serial.print("Response Code : ");
Serial.println(httpCodeGet); // HTTP return code.
Serial.print("Returned data from Server: ");
Serial.println(payloadGet); // print request response data.

if (payloadGet == "1"){
  digitalWrite(ENGINE_D0, LOW); //Turn ON Engine
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Engine ON");
}
if (payloadGet == "0"){
  digitalWrite(ENGINE_D0, HIGH); //Turn OFF Engine
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Engine OFF");
  lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Engine OFF");
}

Serial.println("-----------Closing Connection-------------");
http.end(); // close the connection between divice and server.
Serial.println();

delay(200);


  
  }
  else
  {
    Serial.println("Location: Not Available");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Location");
    lcd.setCursor(1,1);
    lcd.print("Not Available");
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
      
      // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 50000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
 }
