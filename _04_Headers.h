#include "Arduino.h"
#include <SPI.h>
#include <Ethernet2.h>
#include <SoftwareSerial.h>
#include "_06_RestClient.h"

class TeleInfo
{
public:
  TeleInfo(String version, RestClient* client, char* apiKey);
  boolean readTeleInfo();
  boolean recordTeleInfoOnMySQLServer();
  void displayTeleInfo();
  
private :
  SoftwareSerial* mySerial;
  RestClient* client;
  char* api_key;

  char HHPHC;
  
  int ISOUSC;             // intensitÃ© souscrite  
  int IINST;              // intensitÃ© instantanÃ©e en A
  int IMAX;               // intensitÃ© maxi en A
  int ADPS;               // Alerte dépassement en A
  int PAPP;               // puissance apparente en VA
  
  unsigned long BBRHCJB;  // compteur Heures Creuses Bleu  en W
  unsigned long BBRHPJB;  // compteur Heures Pleines Bleu  en W
  unsigned long BBRHCJW;  // compteur Heures Creuses Blanc en W
  unsigned long BBRHPJW;  // compteur Heures Pleines Blanc en W
  unsigned long BBRHCJR;  // compteur Heures Creuses Rouge en W
  unsigned long BBRHPJR;  // compteur Heures Pleines Rouge en W
  
  String PTEC;            // RÃ©gime actuel : HPJB, HCJB, HPJW, HCJW, HPJR, HCJR
  String DEMAIN;          // RÃ©gime demain ; ----, BLEU, BLAN, ROUG
  String ADCO;            // adresse compteur
  String OPTARIF;         // option tarifaire
  uint32_t BASE;            // compteur BASE en W
  String MOTDETAT;        // status word
  String pgmVersion;      // TeleInfo program version

  char chksum(char *buff, uint8_t len);
  boolean handleBuffer(char *bufferTeleinfo, int sequenceNumber);
  int exist(const char *s, char c);
};

