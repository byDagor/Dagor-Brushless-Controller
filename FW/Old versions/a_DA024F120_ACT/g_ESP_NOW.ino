void espNowInit(){
  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  //esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&inputData, incomingData, sizeof(inputData));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  //Serial.print("Function: ");
  //Serial.println(inputData.function);
  //Serial.print("Value: ");
  //Serial.println(inputData.value);

  if (inputData.function == "pos"){
    sendData();
  }
  else if(inputData.value == 0){
    String espNowInput = inputData.function;
    char wirelessCommand[10]; 
    espNowInput.toCharArray(wirelessCommand, sizeof(wirelessCommand));
    commandEspNow.run(wirelessCommand);
  }
  else{
    String espNowInput = inputData.function + String(inputData.value,3);
    char wirelessCommand[10]; 
    espNowInput.toCharArray(wirelessCommand, sizeof(wirelessCommand));
    commandEspNow.run(wirelessCommand);
  }
  
}

void sendData(){
  outputData.function = "A";
  outputData.value = sensor.getAngle();
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outputData, sizeof(outputData));
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  /*if (status == 0){
    String success = "Delivery Success :)";
  }
  else{
    String success = "Delivery Fail :(";
  }*/
}
