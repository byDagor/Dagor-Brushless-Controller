#ifdef RS485


void checkRS485(void * print_data){
  static float monitor_freq = 2000; //[Hz]

  Serial1.begin(921600, SERIAL_8N1, UART_RX, UART_TX);
  myTransfer.begin(Serial1);
  pinMode(COMMS_DIR, OUTPUT);

  vTaskDelay(100 / portTICK_PERIOD_MS);

  while(1){

    if (state_machine < DRV_WARNING){

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
          String extInput = String(input_message.act_commander1) + String(input_message.act_commander2) + String(input_message.act_commander3) + String(input_message.act_target_value,3);    // 3 decimal places for the float
          char ext_command[10]; 
          extInput.toCharArray(ext_command, sizeof(ext_command));
          commandExt.run(ext_command);

        }

      }
      else{
        vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);
      }
      //recSize = myTransfer.rxObj(arr, recSize);
      //Serial.println(arr);
    }
    else{
      //Serial.println("Delay");
      vTaskDelay((1000/10) / portTICK_PERIOD_MS);
    }
  }

}


void taskRS485(){
  xTaskCreatePinnedToCore(
    checkRS485, /* Function to implement the task */
    "RS485", /* Name of the task */
    4*2048,  /* Stack size in words */
    NULL,
    4,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

#endif