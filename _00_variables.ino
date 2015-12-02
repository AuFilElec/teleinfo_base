#include "_04_Headers.h"
#include "Arduino.h"
#include <SPI.h>
#include <Ethernet2.h>
#include <SoftwareSerial.h>

boolean ethernetIsConnected = false;

byte arduinoMAC[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Adresse MAC Arduino
IPAddress arduinoIP = (192, 168, 1, 154);                   // Adresse IP Arduino

char api_key[21] = ""; // API KEY Jeedom

RestClient client = RestClient("192.168.1.17", 80);

TeleInfo* myTeleInfo;
