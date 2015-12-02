//=================================================================================================================
// The TeleInfo is a class that stores the data retrieved from the teleinfo frames, and displays them on a LCD
// Various displays are available : 
//     + Instant values : Actual color, actual mode, instant current, actual color counter, instant power
//     + To morrow color : Actual color and to morrow color when known
//     + A display for each color (blue, white, red) and both modes (HC, HP)
//
// The various displays can be directly selected by pressing the buttons placed below the LCD, so button handling
// routine that generates interrupts is also part of this class
//
//=================================================================================================================
#include "_04_Headers.h"

//=================================================================================================================
// Basic constructor
//=================================================================================================================
TeleInfo::TeleInfo(String version, RestClient* restClient, char* apiKey)
{
  #ifdef debug
    Serial.print(F("Constructor TeleInfo: "));
    Serial.println(version);
  #endif
 //  Serial.begin(1200,SERIAL_7E1);
  mySerial = new SoftwareSerial(8, 9); // RX, TX
  mySerial->begin(1200);
  pgmVersion = version;

  client = restClient;
  api_key = apiKey;
  

  // variables initializations
  ADCO = "031328141543"; // Identifiant du compteur
  OPTARIF = "----"; // Option tarifaire souscrite
  ISOUSC = 0;       // Intensité souscrite en A
  ADPS = 0;         // Alerte Dépassement en A
  BASE = 0;    // Compteur Base
  PTEC = "----";    // Période tarifaire en cours : TH.., HPJB, HCJB, HPJW, HCJW, HPJR, HCJR
  IINST = 0;        // intensité instantanée en A
  IMAX = 0;         // intensité maxi en A
  PAPP = 0;         // puissance apparente en VA
  MOTDETAT = "------";
}

//=================================================================================================================
// Capture des trames de Teleinfo
//=================================================================================================================
boolean TeleInfo::readTeleInfo()
{
#define startFrame 0x02
#define endFrame 0x03
#define stopComm 0x04
#define startLine 0x0A
#define endLine 0x0D
#define maxFrameLen 280

  #ifdef debug
    Serial.println("readTeleInfo called");
  #endif
  int comptChar=0; // variable de comptage des caractÃƒÂ¨res reÃƒÂ§us 
  char charIn=0; // variable de mÃƒÂ©morisation du caractÃƒÂ¨re courant en rÃƒÂ©ception

  char bufferTeleinfo[21] = "";
  int bufferLen = 0;
  int checkSum;

  #ifdef debug
    Serial.print("En attente du début de frame");
  #endif
  //--- wait for starting frame character 
  while (charIn != startFrame)
  { // "Start Text" STX (002 h) is the beginning of the frame
 //   if (Serial.available())
//      charIn = Serial.read(); // Serial.read() vide buffer au fur et à mesure
    if (mySerial->available()) {
      charIn = mySerial->read()& 0x7F; // Serial.read() vide buffer au fur et à mesure
      #ifdef debug
        Serial.print(".");
      #endif
    }
  }
  #ifdef debug
    Serial.println("Trouvée");
  #endif
  
  // On attend la prochaine vague
  /*while (!mySerial->available()) {
    delay(10);
  }*/
  
  int sequenceNumber = 0;    // number of information group

  while (charIn != endFrame)
  { // tant que des octets sont disponibles en lecture : on lit les caractères
    if (mySerial->available()) {
      charIn = mySerial->read()& 0x7F;

      // incrémente le compteur de caractères reçus
      comptChar++;
      if (comptChar > maxFrameLen) {
        #ifdef debug
          Serial.print(F("comptChar("));
          Serial.print(comptChar);
          Serial.println(") > maxFrameLen(280)");
        #endif
        return false;
      }
      if (charIn == stopComm) {
        Serial.println(F("Fin de connexion envoyé par ERDF."));
        return false;
      }
      if (charIn == startLine) {
        bufferLen = 0;
      }
      
      bufferTeleinfo[bufferLen] = charIn;
      // on utilise une limite max pour éviter String trop long en cas erreur réception
      // ajoute le caractère reçu au String pour les N premiers caractères
      if (charIn == endLine) {
        #ifdef debug
          Serial.println(bufferTeleinfo);
        #endif
        checkSum = bufferTeleinfo[bufferLen -1];
        // we clear the 1st character
        if (chksum(bufferTeleinfo, bufferLen) == checkSum) {
          strncpy(&bufferTeleinfo[0], &bufferTeleinfo[1], bufferLen -3);
          bufferTeleinfo[bufferLen -3] =  0x00;
          sequenceNumber++;
          if (! handleBuffer(bufferTeleinfo, sequenceNumber)) {
            #ifdef debug
              Serial.print(F("Error handleBuffer: "));
              Serial.println(sequenceNumber);
            #endif
            return false;
          }
        }
        else {
          #ifdef debug
            Serial.print(F("checkSum wrong: "));
            Serial.println(checkSum);
          #endif
          return false;
        }
      } else {
        bufferLen++;
      }
    }
  }
  return true;
}

int TeleInfo::exist(const char *s, char c)
{
  unsigned int i;
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] == c) {
      return i;
    }
  }
  return -1;
}
//=================================================================================================================
// Frame parsing
//=================================================================================================================
//void handleBuffer(char *bufferTeleinfo, uint8_t len)
boolean TeleInfo::handleBuffer(char *bufferTeleinfo, int sequenceNumber)
{
  // create a pointer to the first char after the space
  char* resultString = strchr(bufferTeleinfo, ' ') + 1;
  
  if (strncmp("ADCO", bufferTeleinfo, 4) == 0) {
    ADCO = String(resultString);
    #ifdef debug
      Serial.print(F("ADCO found: "));
      Serial.println(ADCO);
    #endif
  } else if (strncmp("OPTARIF", bufferTeleinfo, 7) == 0) {
    OPTARIF = String(resultString);
    #ifdef debug
      Serial.print(F("OPTARIF found: "));
      Serial.println(OPTARIF);
    #endif
  } else if (strncmp("ISOUSC", bufferTeleinfo, 6) == 0) {
    ISOUSC = atol(resultString);
    #ifdef debug
      Serial.print(F("ISOUSC found: "));
      Serial.println(ISOUSC);
    #endif
  } else if (strncmp("BASE", bufferTeleinfo, 4) == 0) {
    BASE = atol(resultString);
    #ifdef debug
      Serial.print(F("BASE found: "));
      Serial.println(BASE);
    #endif
  } else if (strncmp("PTEC", bufferTeleinfo, 4) == 0) {
    PTEC = String(resultString);
    #ifdef debug
      Serial.print(F("PTEC found: "));
      Serial.println(PTEC);
    #endif
  } else if (strncmp("IINST", bufferTeleinfo, 5) == 0) {
    IINST = atol(resultString);
    #ifdef debug
      Serial.print(F("IINST found: "));
      Serial.println(IINST);
    #endif
  } else if (strncmp("IMAX", bufferTeleinfo, 4) == 0) {
    IMAX = atol(resultString);
    #ifdef debug
      Serial.print(F("IMAX found: "));
      Serial.println(IMAX);
    #endif
  } else if (strncmp("PAPP", bufferTeleinfo, 4) == 0) {
    PAPP = atol(resultString);
    #ifdef debug
      Serial.print(F("PAPP found: "));
      Serial.println(PAPP);
    #endif
  } else if (strncmp("ADPS", bufferTeleinfo, 4) == 0) {
    ADPS = atol(resultString);
    #ifdef debug
      Serial.print(F("ADPS found: "));
      Serial.println(ADPS);
    #endif
  } else if (strncmp("MOTDETAT", bufferTeleinfo, 8) == 0) {
    MOTDETAT = String(resultString);
    #ifdef debug
      Serial.print(F("MOTDETAT found: "));
      Serial.println(MOTDETAT);
    #endif
  } else {
    Serial.print(F("Unknown: "));
    Serial.print(bufferTeleinfo);
    Serial.print(F(" - at: "));
    Serial.println(sequenceNumber);
    return false;
  }
  return true;
}

//=================================================================================================================
// Calculates teleinfo Checksum
//=================================================================================================================
char TeleInfo::chksum(char *buff, uint8_t len)
{
  //uint8_t sum = 32, cs;
  int i;
  char sum = 0;
  for (i=1; i<(len-2); i++) 
    sum += (int)buff[i];
  sum = (sum & 0x3F) + 0x20;
  return(sum);
}

//=================================================================================================================
// This function displays the TeleInfo Internal counters
// It's usefull for debug purpose
//=================================================================================================================
void TeleInfo::displayTeleInfo()
{
#ifdef debug
  Serial.print(F(" "));
  Serial.println();
  Serial.print(F("ADCO "));
  Serial.println(ADCO);
  Serial.print(F("OPTARIF "));
  Serial.println(OPTARIF);
  Serial.print(F("BASE "));
  Serial.println(BASE);
  Serial.print(F("ISOUSC "));
  Serial.println(ISOUSC);
//  Serial.print(F("BBRHCJB "));
//  Serial.println(BBRHCJB);
//  Serial.print(F("BBRHPJB "));
//  Serial.println(BBRHPJB);
//  Serial.print(F("BBRHCJW "));
//  Serial.println(BBRHCJW);
//  Serial.print(F("BBRHPJW "));
//  Serial.println(BBRHPJW);
//  Serial.print(F("BBRHCJR "));
//  Serial.println(BBRHCJR);
//  Serial.print(F("BBRHPJR "));
//  Serial.println(BBRHPJR);
  Serial.print(F("PTEC "));
  Serial.println(PTEC);
//  Serial.print(F("DEMAIN "));
//  Serial.println(DEMAIN);
  Serial.print(F("IINST "));
  Serial.println(IINST);
  Serial.print(F("IMAX "));
  Serial.println(IMAX);
  Serial.print(F("ADPS "));
  Serial.println(ADPS);
  Serial.print(F("PAPP "));
  Serial.println(PAPP);
//  Serial.print(F("HHPHC "));
//  Serial.println(HHPHC);
  Serial.print(F("MOTDETAT "));
  Serial.println(MOTDETAT);
#endif
}

//=================================================================================================================
// Send results to zibase PHP server
//=================================================================================================================
boolean TeleInfo::recordTeleInfoOnMySQLServer()
{
  char httpRequest[255];
  char hostString[25];
  char demain[5];
  char ptec[5];
  char optarif[5];
  char adco[12];
  //char api_key[21] = "YQFCKcxGJ52BSrHCc73U";
  char base[10];

  PTEC.toCharArray(ptec,5);
  OPTARIF.toCharArray(optarif,5);
  ADCO.toCharArray(adco, 12);
  //BASE.toCharArray(base, 10);


  sprintf(httpRequest, "/plugins/teleinfo/core/php/jeeTeleinfo.php?api=%s&ADCO=%s&OPTARIF=%s&BASE=%lu&PTEC=%s&IINST=%u&IMAX=%u&PAPP=%u",
    api_key,adco,optarif,BASE,ptec,IINST,IMAX,PAPP);

  int statut = 0;
  
  statut = client->get(httpRequest/*, &response*/);
  delay(500);
  
  #ifdef debug
    Serial.print("First request Status: ");
    Serial.println(statut);
  #endif
  
  if (statut != 200) {
    Serial.print("Erreur requete: ");
    Serial.println(statut);
    return false;
  }
  sprintf(httpRequest, "/plugins/myTeleInfo/core/php/jeeTeleinfo.php?api=%s&ADCO=%s&OPTARIF=%s&BASE=%lu&PTEC=%s&IINST=%u&IMAX=%u&PAPP=%u", 
    api_key,adco,optarif,BASE,ptec,IINST,IMAX,PAPP);

  statut = 0;
  
  statut = client->get(httpRequest/*, &response*/);
  delay(500);
  
  #ifdef debug
    Serial.print("Second request Status: ");
    Serial.println(statut);
  #endif

  if (statut != 200) {
    Serial.print("Erreur requete: ");
    Serial.println(statut);
    return false;
  }
  return true;
}


