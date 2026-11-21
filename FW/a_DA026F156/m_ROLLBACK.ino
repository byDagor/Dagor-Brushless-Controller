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
