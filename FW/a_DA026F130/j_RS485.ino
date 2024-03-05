#ifdef RS485

void checkRS485(){
  static float monitor_freq = 2000; //[Hz]

  while(1){
    if( myTransfer.available() ) {
      // use this variable to keep track of how many
      // bytes we've processed from the receive buffer
      uint16_t recSize = 0;

      recSize = myTransfer.rxObj(input_message, recSize);

      if (input_message.act_id == ACT_ID){
        //if (input_message.act_commander1 == 0 && input_message.act_commander2 == 0){
        //  if(input_message.act_commander3 == 'M'){
        //    String extInput = "M" + String(input_message.act_target_value,3);    // 3 decimal places for the float
        //    char wired_command[10]; 
        //    extInput.toCharArray(wired_command, sizeof(wired_command));
        //    commandExt.run(wired_command);
        //  }
        //}
        String extInput = input_message.act_commander1 + input_message.act_commander2 + input_message.act_commander3 + String(input_message.act_target_value,3);    // 3 decimal places for the float
        char ext_command[10]; 
        extInput.toCharArray(ext_command, sizeof(ext_command));
        commandExt.run(ext_command);

      }

      //recSize = myTransfer.rxObj(arr, recSize);
      //Serial.println(arr);
    }
    vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);
  }
}

#endif