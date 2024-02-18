//###########################################
//      ESP-NOW FUNCTIONS DEFINITIONS
//###########################################

#ifdef ESP_NOW

#define TRY_ESP_ACTION(action, name) if(action == ESP_OK) {Serial.println("\t+ "+String(name));} else {Serial.println("----------Error while " + String(name) + " !---------------");}

#define CHANNEL 11
#define DATARATE WIFI_PHY_RATE_1M_L

const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};      // Set address to eanble ESP-NOW broadcast, sending packets over broadcast disables ACK, very important for high bandwidth.

void espNowInit(){
  WiFi.disconnect();
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("Controller MAC address: ");
  Serial.println(WiFi.macAddress());
  // Init ESP-NOW
  TRY_ESP_ACTION( esp_wifi_stop(), "Stop WIFI");
  TRY_ESP_ACTION( esp_wifi_deinit(), "De init");
  wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
  my_config.ampdu_tx_enable = 0;
  TRY_ESP_ACTION( esp_wifi_init(&my_config), "Disable AMPDU");
  TRY_ESP_ACTION( esp_wifi_start(), "Restart WiFi");
  TRY_ESP_ACTION( esp_wifi_set_promiscuous(true), "Set promiscuous");
  TRY_ESP_ACTION( esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE), "Channel set");
  TRY_ESP_ACTION( esp_wifi_internal_set_fix_rate(WIFI_IF_AP, true, DATARATE), "Fixed rate set up");
  TRY_ESP_ACTION( esp_now_init(), "ESP-NOW Init");

  esp_now_register_recv_cb(OnDataRecv);
  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); 
  peerInfo.channel = CHANNEL;  
  peerInfo.encrypt = false; 

  // Add peer
  TRY_ESP_ACTION( esp_now_add_peer(&peerInfo), "Add peer");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  
  memcpy(&inputData, incomingData, sizeof(inputData));
  
  if (inputData.act_id == ACT_ID){
    if (inputData.act_commander1 == 0 && inputData.act_commander1 == 0){
      if(inputData.act_commander1 == 77){
        String espNowInput = "M" + String(inputData.act_target_value,3);    // 3 decimenal places for the float
        char wirelessCommand[10]; 
        espNowInput.toCharArray(wirelessCommand, sizeof(wirelessCommand));
        commandEspNow.run(wirelessCommand);
      }
    }
  }
  
  
  /*
  inputData.leg_id = *(int*)incomingData;

  if (inputData.act_id == ACT_ID){

    inputData.kneeFunc = String((char*)(incomingData+4));
    inputData.kneeValue = *(float*)(incomingData+16);
    
    if (inputData.kneeFunc == "home" && gravityCompMode){
      Serial.print("New Home at: ");
      Serial.println( sensor.getAngle() );
      if (motor.sensor_direction == CW) motor.sensor_offset = sensor.getAngle();
      if (motor.sensor_direction == CCW) motor.sensor_offset = -sensor.getAngle();
      gravityCompMode = false;
      commandEspNow.run("M0");
    }
    else if(inputData.kneeValue == 0){
      //Serial.println(inputData.kneeFunc);
      String espNowInput = inputData.kneeFunc;
      char wirelessCommand[10]; 
      espNowInput.toCharArray(wirelessCommand, sizeof(wirelessCommand));
      commandEspNow.run(wirelessCommand);
    }
    else{
      String espNowInput = "M" + String(inputData.kneeValue,3);
      char wirelessCommand[10]; 
      espNowInput.toCharArray(wirelessCommand, sizeof(wirelessCommand));
      commandEspNow.run(wirelessCommand);
    }  
  }
  */
}

void sendData(){
  /*outputData.function = "A";
  outputData.value = sensor.getAngle();
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outputData, sizeof(outputData));*/
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

#endif
