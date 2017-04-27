#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>  
#include "SSD1306.h"
#include <Adafruit_ADS1015.h>

SSD1306  display(0x3c, 0, 2); // Initialise the OLED display using Wire library
Adafruit_ADS1115 ads1115(0x48);  // construct an ads1115 at address 0x48
float Voltage = 0.0;
float Temp1 = 0.0;
float Temp2 = 0.0;

float m=203.04;
float c=3707.478;



// Replace with your network credentials
const char* ssid = "hoving";
const char* password = "groningen";

ESP8266WebServer server(80);
String webPage = "";

void setup() {
  Serial.begin(9600);
  delay(1000);
  webPage += "<h1>ESP8266 Web Server</h1><p>Coolerbox Temp</p>";
  
  
  
  display.init(); // Initialising the UI will init the display too.
//  display.flipScreenVertically();
  display.clear();
  drawHelloWorld(); 
  display.display();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  drawIPAddress();
  ads1115.begin();

  server.on("/", [](){
     //This is executed when you go to the IP address on the display
     char tempChar1[20] = "";
     dtostrf(Temp1, 3, 1, tempChar1);
     char tempChar2[20] = "";
     dtostrf(Temp2, 3, 1, tempChar2);
     
     server.send(200, "text/html", webPage + "<p><br>Temp1:" + tempChar1  + "<br>Temp2:" + tempChar2 + "</p>");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  ESP.wdtDisable();
  display.clear();
  drawVoltage();
  drawIPAddress();
  display.display();
}

void drawVoltage()
{
    int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
    int16_t adc1; 
    adc0 = ads1115.readADC_SingleEnded(0);
    adc1 = ads1115.readADC_SingleEnded(1);
    Voltage = (adc0 * 0.1875)/1000;
    char result[20] = "";
    dtostrf(adc0, 3, 1, result);

    Temp1 = (adc0-c)/m;  //x=(y-c)/m
    char tempChar[20] = "";
    Temp2 = (adc1-c)/m;  //x=(y-c)/m
    
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    dtostrf(Temp1, 3, 1, tempChar);
    display.drawString(0, 10, "Temp1:");
    display.drawString(70, 10, tempChar);
    dtostrf(Temp2, 3, 1, tempChar);
    display.drawString(0, 26, "Temp2:");
    display.drawString(70, 26, tempChar);
    
   // Serial.print("adc0: "); Serial.println(result);
     
   
    
}
void drawIPAddress() 
{
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 50, "IP:");
    display.drawString(30, 50, WiFi.localIP().toString());

    display.display();
    
}
void drawHelloWorld() 
{
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "------");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Booting");
    display.setFont(ArialMT_Plain_24);
   // display.drawString(0, 26, "Hello Lucas & Leo");
}




