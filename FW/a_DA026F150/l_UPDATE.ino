//###########################################
//       WIRELESS FIRMWARE UPDATE MODE
//###########################################

// Commander callback of the update command, U1. It only raises a flag: the restart happens in loop(),
// not inside a comms callback.
void onUpdateCommand(char* cmd){
  if (atoi(cmd) == 1) updateRequested = true;
}

// Serial commands (I, W) are registered on the serial commander only, so the ID and the credentials cannot be changed wirelessly.
void registerSystemCommands(){
  command.add('I', setActuatorId, " Actuator ID");
  command.add('W', storeCredentialsCommand, " Store Wi-Fi network and update password");

  #ifdef OTA_UPDATE
    command.add(UPDATE_COMMAND_ID, onUpdateCommand, " Restart in update mode");
    #if defined(ESP_NOW) || defined(RS485)
      commandExt.add(UPDATE_COMMAND_ID, onUpdateCommand, " Restart in update mode");
    #endif
  #endif
}

#ifdef OTA_UPDATE

volatile bool updateInProgress = false;

// Motor and gate driver off, then restart. The next boot only brings up Wi-Fi and the update service.
void enterUpdateMode(){
  motor.disable();
  digitalWrite(enGate, LOW);
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

// Runs from setup() when the update flag is set and never returns: the board restarts after an update,
// when something is missing, or when the timeout expires. No motor, driver or comms code is started.
void runUpdateMode(){
  gpio_init();
  Serial.println("DAGOR: Update mode.");

  String ssid = nvs.getString(KEY_WIFI_SSID, "");
  String wifiPassword = nvs.getString(KEY_WIFI_PASSWORD, "");
  String updatePassword = nvs.getString(KEY_UPDATE_PASSWORD, "");

  if (ssid.length() == 0 || updatePassword.length() == 0){
    Serial.println("DAGOR: No Wi-Fi network or update password stored, back to normal operation.");
    ESP.restart();
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), wifiPassword.c_str());

  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED){
    if (millis() - wifiStart > updateWifiTimeoutMs){
      Serial.println("DAGOR: Could not join the Wi-Fi network, back to normal operation.");
      ESP.restart();
    }
    delay(100);
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
