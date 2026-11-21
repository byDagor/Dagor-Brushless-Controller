//###########################################
//        SETTINGS STORED IN FLASH (NVS)
//###########################################

// Optional local file with the default Wi-Fi networks and update password. It is never committed:
// copy dagor_secrets.example.h to dagor_secrets.h and fill it in.
#if __has_include("dagor_secrets.h")
  #include "dagor_secrets.h"
  #define HAS_DEFAULT_CREDENTIALS
#endif

const char* NVS_NAMESPACE = "dagor";
const char* KEY_ACT_ID = "actId";
const char* KEY_WIFI_SSID = "wifiSsid";            // followed by the network number, wifiSsid0, wifiSsid1...
const char* KEY_WIFI_PASSWORD = "wifiPass";
const char* KEY_UPDATE_PASSWORD = "updatePass";
const char* KEY_UPDATE_MODE = "updateMode";

const int WIFI_NETWORKS_MAX = 3;          // Wi-Fi networks a board can remember, tried in order
const int ACT_ID_BROADCAST = 0;           // Every board answers to this ID, but only for the update command
const int ACT_ID_MAX = 65535;
const char UPDATE_COMMAND_ID = 'U';

#ifdef HAS_DEFAULT_CREDENTIALS
  const int defaultNetworkCount = sizeof(DEFAULT_WIFI_SSIDS) / sizeof(DEFAULT_WIFI_SSIDS[0]);
  static_assert(sizeof(DEFAULT_WIFI_SSIDS) / sizeof(DEFAULT_WIFI_SSIDS[0]) <= WIFI_NETWORKS_MAX, "dagor_secrets.h lists more Wi-Fi networks than a board can remember");
  static_assert(sizeof(DEFAULT_WIFI_SSIDS) == sizeof(DEFAULT_WIFI_PASSWORDS), "dagor_secrets.h needs one password per Wi-Fi network");
#endif

// Key of a stored Wi-Fi network, for example wifiSsid1
String wifiKey(const char* name, int network){
  return String(name) + String(network);
}

bool anyNetworkStored(){
  for (int network = 0; network < WIFI_NETWORKS_MAX; network++){
    if (nvs.getString(wifiKey(KEY_WIFI_SSID, network).c_str(), "").length() > 0) return true;
  }
  return false;
}

// Stores the compiled-in networks and update password. Without overwrite only the ones missing in flash are stored.
// With overwrite, networks that are no longer in the file are removed.
void storeDefaultCredentials(bool overwrite){
  #ifdef HAS_DEFAULT_CREDENTIALS
    for (int network = 0; network < WIFI_NETWORKS_MAX; network++){
      String ssidKey = wifiKey(KEY_WIFI_SSID, network);
      String passwordKey = wifiKey(KEY_WIFI_PASSWORD, network);
      if (network < defaultNetworkCount){
        if (overwrite || !nvs.isKey(ssidKey.c_str())){
          nvs.putString(ssidKey.c_str(), DEFAULT_WIFI_SSIDS[network]);
          nvs.putString(passwordKey.c_str(), DEFAULT_WIFI_PASSWORDS[network]);
        }
      } else if (overwrite){
        nvs.remove(ssidKey.c_str());
        nvs.remove(passwordKey.c_str());
      }
    }
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
  noteCommand('I', cmd);
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
  noteCommand('W', "");   // the argument holds a password, it is never stored
  #ifdef HAS_DEFAULT_CREDENTIALS
    if (atoi(cmd) != 1) return;
    storeDefaultCredentials(true);
    Serial.print("DAGOR: Wi-Fi networks and update password stored, networks -> ");
    Serial.println(defaultNetworkCount);
  #else
    Serial.println("DAGOR: No credentials compiled in, see dagor_secrets.example.h.");
  #endif
}
