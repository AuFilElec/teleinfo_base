void loop() 
{
  boolean teleInfoReceived;

  #ifdef debug
    Serial.println(F("Call myTeleInfo->readTeleInfo"));
  #endif
  // we parse the teleInfo frame
  teleInfoReceived = myTeleInfo->readTeleInfo();

  // If the frame was correctly reveived, we display it in
  // the console (only if debug)
  if (teleInfoReceived) {
#ifdef debug
    myTeleInfo->displayTeleInfo();
#endif
    myTeleInfo->recordTeleInfoOnMySQLServer();
    // Une petite pause entre chaque requête, on n'est pas pressé
    delay(10000); // 10 secondes
  } else {
    // Délai plus court en cas d'erreur
    delay(1000); // 1 seconde
  }
}


