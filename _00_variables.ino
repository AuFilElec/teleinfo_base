#include "_04_Headers.h"
#include "Arduino.h"
#include <SPI.h>
#include <Ethernet2.h>
#include <SoftwareSerial.h>

boolean ethernetIsConnected = false;

byte arduinoMAC[] = { 0x90, 0xA2, 0xDA, 0x10, 0x2E, 0xBF }; // Adresse MAC Arduino
IPAddress arduinoIP = (192, 168, 1, 18);                     // Adresse IP Arduino
IPAddress gateway = (192, 168, 1, 1);                      // Adresse IP de Broadcast LAN

char api_key[21] = "YQFCKcxGJ52BSrHCc73U"; // API KEY Jeedom

unsigned long loopCounter = 0L;
RestClient client = RestClient("192.168.1.17", 80);

TeleInfo* myTeleInfo;


