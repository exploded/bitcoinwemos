/******************************************************************************
   bitcoinprice.ino

   This sketch connects to WiFi, gets the last bitcoin price from Bitstamp and
   displays it on a MicroOLED. Updates every 10 seconds.

   Development environment specifics:
    WEMOS D1 Mini
    OLED Shield for WeMos D1 mini 0.66" inch 64X48 IIC I2C

   Distributed as-is; no warranty is given.
 ******************************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SFE_MicroOLED.h>
#include <TimeLib.h>

const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxxxx";

const char* host = "www.bitstamp.net";
const int httpsPort = 443;
const int fontSize = 2;  // 0 Micro, 1 tiny, 2 medium 3 large

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "d0:26:ab:06:64:07:bc:88:56:6d:83:be:0a:29:00:b5:10:e5:27:d2";

// MicroOLED Definition
#define PIN_RESET 255
#define DC_JUMPER 0

// MicroOLED Object Declaration //
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration

void setup()
{

  // Setup serial-output
  Serial.begin(115200);
  delay(10);

  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.

  printTitle("Connecting", 0);
  oled.display();



  WiFi.mode(WIFI_STA);
  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Set the hostname
  WiFi.hostname("weemos");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  printTitle("Getting   price fromBitstamp", 0);
  oled.display();
  
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
  // long rssi = WiFi.RSSI();
  // Serial.print("signal strength (RSSI):");
  // Serial.print(rssi);
  // Serial.println(" dBm");

}

void printPrice()
{

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  // Serial.print("connecting to ");
  // Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    ESP.reset() ;
    return;
  }

  if (client.verify(fingerprint, host)) {
    // Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  String url = "/api/ticker";
  // Serial.print("requesting URL: ");
  // Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: DisplayWemosESP8266\r\n" +
               "Connection: close\r\n\r\n");

  // Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      //  Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"high\":")) {
    //  Serial.println("API call successfull!");
  } else {
    Serial.println("API call has failed");
    ESP.reset() ;
  }

  int start = line.indexOf("last") + 8;
  int end = line.indexOf("timestamp") - 7;

  if (fontSize == 1) {
    // font is small enough to show 1 decimal
    end = line.indexOf("timestamp") - 5;
  }

  if (fontSize == 0) {
    // font is small enough to show 2 decimal2
    end = line.indexOf("timestamp") - 4;
  }

  String price = line.substring(start, end);
  printTitle(price, fontSize);

  start = line.indexOf("timestamp") + 13;
  end = line.indexOf("bid") - 4;
  int timeStamp = line.substring(start, end).toInt() + 10*3600;
  ;
  oled.setFontType(0);
  oled.setCursor(6,38);
  //oled.print(day(timeStamp));
  //oled.print("-");
  //oled.print(month(timeStamp));
  //oled.print("-");
  //oled.print(year(timeStamp));
  //oled.print(" ");

  oled.print(hour(timeStamp));
  oled.print(":");
  oled.printf("%02d",minute(timeStamp));
  oled.print(":");
  oled.printf("%02d",second(timeStamp));
  oled.display();
 

}

void loop()
{
  printPrice();
  delay(5000);
}

// Center and print a small title
void printTitle(String title, int font)
{
  // Font 0. 5x8 font. 255 possible characters
  // Font 1. 8x16
  // Font 2. 10x16. Only numbers and '.' are defined. Looks like 7-segment displays.
  // Font 3. 12x48. The biggest font
  // Font 4. 31x48.

  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen

  int newX = 0;
  int newY = 0;
  int lines = 1 + (title.length() * oled.getFontWidth()) / oled.getLCDWidth();

  if (lines  <= 1) {
    // Single line so we try to center it
    newX = middleX - ((1 + oled.getFontWidth()) * (title.length() / 2));
  }
  newY = middleY - (oled.getFontHeight() * lines / 2);

  oled.setCursor(newX, newY );

  // Print the title:
  oled.print(title);
  
}
