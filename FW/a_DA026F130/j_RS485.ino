#ifdef RS485

void checkRS485(){
  if( myTransfer.available() ) {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;

    recSize = myTransfer.rxObj(input_message, recSize);

    if (input_message.act_id == ACT_ID){
      if (input_message.act_commander1 == 0 && input_message.act_commander2 == 0){
        if(input_message.act_commander3 == 'M'){
          String espNowInput = "M" + String(input_message.act_target_value,3);    // 3 decimenal places for the float
          char wired_command[10]; 
          espNowInput.toCharArray(wired_command, sizeof(wired_command));
          commandEspNow.run(wired_command);
        }
      }
    }

    //recSize = myTransfer.rxObj(arr, recSize);
    //Serial.println(arr);
  }
}

#endif