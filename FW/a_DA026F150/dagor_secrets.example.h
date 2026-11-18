// Copy this file to dagor_secrets.h and fill in the values. dagor_secrets.h must never be committed.
// The first boot after a wired flash stores these in the board's flash (NVS), later firmware updates do not need them.
// To store changed values on a board that already has some, send W1 over USB serial.

// Wi-Fi networks in priority order, up to 3. Update mode joins the first one that is in range.
// The password at the same position belongs to the network name at that position.
const char* const DEFAULT_WIFI_SSIDS[] = { "home-wifi-name", "phone-hotspot-name" };
const char* const DEFAULT_WIFI_PASSWORDS[] = { "home-wifi-password", "phone-hotspot-password" };

#define DEFAULT_UPDATE_PASSWORD "password-asked-when-uploading-firmware"
