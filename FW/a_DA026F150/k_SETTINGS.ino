//###########################################
//        SETTINGS STORED IN FLASH (NVS)
//###########################################

// Optional local file with the default Wi-Fi network and update password. It is never committed:
// copy dagor_secrets.example.h to dagor_secrets.h and fill it in.
#if __has_include("dagor_secrets.h")
  #include "dagor_secrets.h"
  #define HAS_DEFAULT_CREDENTIALS
#endif

const char* NVS_NAMESPACE = "dagor";
const char* KEY_ACT_ID = "actId";
const char* KEY_WIFI_SSID = "wifiSsid";
const char* KEY_WIFI_PASSWORD = "wifiPass";
const char* KEY_UPDATE_PASSWORD = "updatePass";
const char* KEY_UPDATE_MODE = "updateMode";

const int ACT_ID_BROADCAST = 0;           // Every board answers to this ID, but only for the update command
const int ACT_ID_MAX = 65535;
const char UPDATE_COMMAND_ID = 'U';

// Stores the compiled-in credentials. Without overwrite only the ones missing in flash are stored.
void storeDefaultCredentials(bool overwrite){
  #ifdef HAS_DEFAULT_CREDENTIALS
    if (overwrite || !nvs.isKey(KEY_WIFI_SSID)) nvs.putString(KEY_WIFI_SSID, DEFAULT_WIFI_SSID);
    if (overwrite || !nvs.isKey(KEY_WIFI_PASSWORD)) nvs.putString(KEY_WIFI_PASSWORD, DEFAULT_WIFI_PASSWORD);
    if (overwrite || !nvs.isKey(KEY_UPDATE_PASSWORD)) nvs.putString(KEY_UPDATE_PASSWORD, DEFAULT_UPDATE_PASSWORD);
  #endif
}

// The first boot after a wired flash stores the compiled-in ID and credentials, later boots use what is in flash.
void settingsInit(){
  nvs.begin(NVS_NAMESPACE, false);
  if (!nvs.isKey(KEY_ACT_ID)) nvs.putInt(KEY_ACT_ID, ACT_ID);
  storeDefaultCredentials(false);
  actId = nvs.getInt(KEY_ACT_ID, ACT_ID);
}

bool addressedToThisBoard(int id, char firstCommand){
  return id == actId || (id == ACT_ID_BROADCAST && firstCommand == UPDATE_COMMAND_ID);
}

// Serial command I<id>
void setActuatorId(char* cmd){
  int id = atoi(cmd);
  if (id <= ACT_ID_BROADCAST || id > ACT_ID_MAX){
    Serial.println("DAGOR: Actuator ID must be between 1 and 65535.");
    return;
  }
  nvs.putInt(KEY_ACT_ID, id);
  actId = id;
  Serial.print("DAGOR: Actuator ID stored -> ");
  Serial.println(id);
}

// Serial command W1
void storeCredentialsCommand(char* cmd){
  #ifdef HAS_DEFAULT_CREDENTIALS
    if (atoi(cmd) != 1) return;
    storeDefaultCredentials(true);
    Serial.println("DAGOR: Wi-Fi network and update password stored.");
  #else
    Serial.println("DAGOR: No credentials compiled in, see dagor_secrets.example.h.");
  #endif
}
