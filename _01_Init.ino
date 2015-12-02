void setup()
{
  Serial.begin(115200);
  Serial.println("Starting ...");
  delay(300);
  // attempting to connect with Ethernet
  client.dhcp(arduinoMAC, arduinoIP);
  #ifdef debug
    Serial.println("Connected");
  #endif
  //ethernetIsConnected = internetConnect();
  // starting TeleInfo capture
  myTeleInfo = new TeleInfo(version, &client, api_key);
}


