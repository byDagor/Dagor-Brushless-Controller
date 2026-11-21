//###########################################
//       WIRELESS FIRMWARE UPDATE MODE
//###########################################

// Commander callback of the update command, U1. It only raises a flag: the restart happens in loop(),
// not inside a comms callback.
void onUpdateCommand(char* cmd){
  noteCommand('U', cmd);
  if (atoi(cmd) == 1) updateRequested = true;
}

// Serial commands (I, W) are registered on the serial commander only, so the ID and the credentials cannot be changed wirelessly.
void registerSystemCommands(){
  command.add('I', setActuatorId, " Actuator ID");
  command.add('W', storeCredentialsCommand, " Store Wi-Fi network and update password");
  commandSystem.add('I', setActuatorId, " Actuator ID");
  commandSystem.add('W', storeCredentialsCommand, " Store Wi-Fi network and update password");
  commandSystem.add('L', printLoopRate, " Loop rate");
  command.add('S', onStatusCommand, " Status");
  command.add('R', onRestartCommand, " Restart");
  commandSystem.add('S', onStatusCommand, " Status");
  commandSystem.add('R', onRestartCommand, " Restart");
  #ifdef DEBUG_INJECT
    command.add('J', onInjectCommand, " Inject a reason bit for tests");
  #endif

  #ifdef OTA_UPDATE
    command.add(UPDATE_COMMAND_ID, onUpdateCommand, " Restart in update mode");
    commandSystem.add(UPDATE_COMMAND_ID, onUpdateCommand, " Restart in update mode");
    #if defined(ESP_NOW) || defined(RS485)
      commandExt.add(UPDATE_COMMAND_ID, onUpdateCommand, " Restart in update mode");
    #endif
  #endif
}

#ifdef OTA_UPDATE

volatile bool updateInProgress = false;

// Motor and gate driver off, then restart. The next boot only brings up Wi-Fi and the update service.
void enterUpdateMode(){
  validateImage();
  motor.disable();
  driver.disableGate();
  nvs.putBool(KEY_UPDATE_MODE, true);
  Serial.println("DAGOR: Restarting in update mode.");
  Serial.flush();
  ESP.restart();
}

// The flag is cleared as soon as it is read, so a failed or interrupted update never traps the board in update mode.
bool updateModeRequested(){
  bool requested = nvs.getBool(KEY_UPDATE_MODE, false);
  if (requested) nvs.putBool(KEY_UPDATE_MODE, false);
  return requested;
}

void otaStart(){
  updateInProgress = true;
  Serial.println("DAGOR: Update started.");
}

void otaProgress(unsigned int done, unsigned int total){
  static int lastTenth = -1;
  int tenth = (done * 10UL) / total;
  if (tenth != lastTenth){
    lastTenth = tenth;
    Serial.print("DAGOR: Update ");
    Serial.print(tenth * 10);
    Serial.println("%");
  }
}

void otaEnd(){
  Serial.println("DAGOR: Update finished, restarting.");
}

// Takes an int instead of ota_error_t: Arduino generates the prototype before the library header is included.
void otaError(int error){
  updateInProgress = false;
  Serial.print("DAGOR: Update error ");
  Serial.println(error);
}

// Scans, then joins the first stored network (in storage order) that is in range. If joining one fails,
// the next stored network in range is tried.
bool joinKnownNetwork(){
  int found = WiFi.scanNetworks();

  for (int network = 0; network < WIFI_NETWORKS_MAX; network++){
    String ssid = nvs.getString(wifiKey(KEY_WIFI_SSID, network).c_str(), "");
    if (ssid.length() == 0) continue;

    bool inRange = false;
    for (int i = 0; i < found; i++){
      if (WiFi.SSID(i) == ssid) inRange = true;
    }
    if (!inRange) continue;

    String password = nvs.getString(wifiKey(KEY_WIFI_PASSWORD, network).c_str(), "");
    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < updateWifiTimeoutMs){
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED){
      Serial.print("DAGOR: Joined Wi-Fi network ");
      Serial.println(network + 1);
      WiFi.scanDelete();
      return true;
    }
    WiFi.disconnect();
  }

  WiFi.scanDelete();
  return false;
}

// Runs from setup() when the update flag is set and never returns: the board restarts after an update,
// when something is missing, or when the timeout expires. No motor, driver or comms code is started.
void runUpdateMode(){
  gpio_init();
  Serial.println("DAGOR: Update mode.");

  String updatePassword = nvs.getString(KEY_UPDATE_PASSWORD, "");

  if (!anyNetworkStored() || updatePassword.length() == 0){
    Serial.println("DAGOR: No Wi-Fi network or update password stored, back to normal operation.");
    ESP.restart();
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  if (!joinKnownNetwork()){
    Serial.println("DAGOR: Could not join any stored Wi-Fi network, back to normal operation.");
    ESP.restart();
  }

  String hostname = "dagor-" + String(actId);
  ArduinoOTA.setHostname(hostname.c_str());
  ArduinoOTA.setPassword(updatePassword.c_str());
  ArduinoOTA.onStart(otaStart);
  ArduinoOTA.onProgress(otaProgress);
  ArduinoOTA.onEnd(otaEnd);
  ArduinoOTA.onError(otaError);
  ArduinoOTA.begin();

  Serial.print("DAGOR: Ready for firmware as ");
  Serial.print(hostname);
  Serial.print(".local at ");
  Serial.println(WiFi.localIP());

  unsigned long modeStart = millis();
  while (true){
    ArduinoOTA.handle();
    if (!updateInProgress && millis() - modeStart > updateModeTimeoutMs){
      Serial.println("DAGOR: Update timeout, back to normal operation.");
      ESP.restart();
    }
    delay(1);
  }
}

#endif


//###########################################
//     NEW FIRMWARE VALIDATION AND ROLLBACK
//###########################################

// A firmware image delivered by an update stays "pending verification" until this sketch validates it.
// - Motor start-up failed: the image is rejected at once and the board goes back to the previous image.
// - Motor start-up worked: the image is validated rollbackValidationMs after boot.
// - The board restarts or crashes before that: the bootloader goes back to the previous image by itself.

bool imageIsPendingVerification(){
  esp_ota_img_states_t state;
  const esp_partition_t* running = esp_ota_get_running_partition();
  return esp_ota_get_state_partition(running, &state) == ESP_OK && state == ESP_OTA_IMG_PENDING_VERIFY;
}

void validateImage(){
  if (!imagePending) return;
  esp_ota_mark_app_valid_cancel_rollback();
  imagePending = false;
  Serial.println("DAGOR: New firmware validated.");
}

// Does not return when there is a previous image to go back to.
void rejectImage(){
  Serial.println("DAGOR: New firmware failed its start-up checks, going back to the previous one.");
  Serial.flush();
  esp_ota_mark_app_invalid_rollback_and_reboot();
  Serial.println("DAGOR: There is no previous firmware to go back to, keeping this one.");
  validateImage();
}

// Called at the end of the start-up, once the motor initialisation result is known.
void checkImageAtBoot(){
  imagePending = imageIsPendingVerification();
  Serial.print("DAGOR: Firmware image -> ");
  Serial.println(imagePending ? "new, waiting for validation" : "already validated");
  if (imagePending && state_machine != LIFE_IS_GOOD) rejectImage();
}

// Called from loop()
void validateImageWhenHealthy(){
  if (millis() >= rollbackValidationMs) validateImage();
}
