Bitcoin price ticker for the WEMOS D1 Mini
==========================================

Connects to WIFI, downloads price from Bitstamp and updates every 10 seconds. Time of last update is also displayed.

Re-boots if there is a problem downloading the price.

Tested on a WEMOS D1 Mini with Mini OLED.

Instructions
------------

Edit file to insert your WiFi SSID and password. 

Upload to the device using Arduino studio

Ensure the following dependencies are loaded:
* ESP8266WiFi.h
* WiFiClientSecure.h
* SFE_MicroOLED.h     https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
* TimeLib.h           http://playground.arduino.cc/Code/Time
