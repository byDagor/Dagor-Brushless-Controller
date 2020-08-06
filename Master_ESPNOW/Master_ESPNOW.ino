//Sparkfun: 24:6F:28:51:ED:A4
//ESP-DEVKIT Grey Yuneec: CC:50:E3:AF:D2:8C
//Devkit 2: FC:F5:C4:31:80:70

#include <esp_now.h>
#include <WiFi.h>

float target = 0;
float i = 0;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFC, 0xF5, 0xC4, 0x31, 0x80, 0x70};

// Structure example to send data (Must match the receiver structure)
typedef struct struct_message {
  String function;
  float value;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Set values to send
  serialEvent();
  delay(1);

  i += 0.1;

  if (i>=20) i = 0;
  
  myData.function = "Target";
  myData.value = i;
        
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) Serial.println("Sent with success");
  else Serial.println("Error sending the data");
}

void serialEvent() {
  // a string to hold incoming data
  static String inputString;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline
    // end of input
    if (inChar == '\n') {
      if(inputString.charAt(0) == 'P'){
        //motor.PI_velocity.P = inputString.substring(1).toFloat();
        //printGains();
      }/*else if(inputString.charAt(0) == 'I'){
        motor.PI_velocity.I = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'F'){
        motor.LPF_velocity.Tf = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'K'){
        motor.P_angle.P = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'R'){
        motor.PI_velocity.voltage_ramp = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'L'){
        motor.PI_velocity.voltage_limit = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'V'){
        motor.P_angle.velocity_limit = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'T'){
        Serial.print("Average loop time is (microseconds): ");
        Serial.println((micros() - timestamp)/t);
        t = 0;
        timestamp = micros();
      }else if(inputString.charAt(0) == 'C'){
        Serial.print("Contoller type: ");
        int cnt = inputString.substring(1).toFloat();
        if(cnt == 0){
          Serial.println("angle!");
          motor.controller = ControlType::angle;
        }else if(cnt == 1){
          Serial.println("velocity!");
          motor.controller = ControlType::velocity;
        }else if(cnt == 2){
          Serial.println("volatge!");
          motor.controller = ControlType::voltage;
        }
        Serial.println();
        t = 0;
        timestamp = micros();
      }*/
      else{
        target = inputString.toFloat();
        Serial.print("Target : ");
        Serial.println(target);
        inputString = "";

        myData.function = "Target";
        myData.value = target;
        
        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
         
        if (result == ESP_OK) Serial.println("Sent with success");
        else Serial.println("Error sending the data");
      }
      inputString = "";
    }
  }
}
