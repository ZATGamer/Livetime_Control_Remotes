unsigned long blink_start;
bool currently_indicating;
void blink_LED(int LED) {
  // Blink the LED at a rate of 500ms
  // NON CODE STOPPING
  if (blink_start + 500 < millis()){
    digitalWrite(LED, !digitalRead(LED));
    blink_start = millis();
  }
}


bool flip = false;
void green_yellow(){
  // Blink Green then Red.
  // NON CODE STOPPING
  if (blink_start + 500 < millis()) {
    if(flip){
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      flip = false;
    }
    else if (!flip) {
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(YELLOW_LED, HIGH);
      flip = true;
    }
    blink_start = millis();
  }
}


void flash_ALL() {
  // Turn all LEDs ON delay 150 then all LEDs OFF
  // CODE STOPPING
  for(byte led = 0; led < LED_count; led++) {
    if(!digitalRead(LED_colors[led])) digitalWrite(LED_colors[led], HIGH);
  }
  delay(150);
  for(byte led = 0; led < LED_count; led++) {
    if(digitalRead(LED_colors[led])) digitalWrite(LED_colors[led], LOW);
  }
}


void lights_off() {
  // Turn all the LEDs ON.
  for (int thisLED = 0; thisLED < LED_count; thisLED++) {
    if (digitalRead(LED_colors[thisLED])) digitalWrite(LED_colors[thisLED], LOW);
  }
}


void lights_on() {
  // Turn all the LEDs ON
  for (int thisLED = 0; thisLED < LED_count; thisLED++) {
    if (!digitalRead(LED_colors[thisLED])) digitalWrite(LED_colors[thisLED], HIGH);
  }
}


byte flash_count = 0;
bool flash_break;
bool flash_done;
bool flash_10_done;
unsigned long flash_break_count;
unsigned long flash_start;
void flash_count_c(byte count) {
  // Flash LED x Number of times
  // GREEN LED indicates Single Digits i.e. No Yellow, 5 Green = 5
  // Yellow LED indicates Doubble Digits i.e. 1 Yellow, 2 Green = 12
  if (!flash_break){
    if (count <= 9){
      // Set the 10 to ture and then flash the green
      flash_10_done = true;
      flash_x_count(count, GREEN_LED, flash_done);
    }
    else if (count >= 10 && count < 100) {
      // Flash the Yellow first
      byte yellow_flash;
      byte green_flash_modifier;
      if (count >= 10 && count < 20) yellow_flash = 1, green_flash_modifier = 10;
      else if (count >= 20 && count < 30) yellow_flash = 2, green_flash_modifier = 20;
      else if (count >= 30 && count < 40) yellow_flash = 3, green_flash_modifier = 30;
      else if (count >= 40 && count < 50) yellow_flash = 4, green_flash_modifier = 40;
      else if (count >= 50 && count < 60) yellow_flash = 5, green_flash_modifier = 50;
      else if (count >= 60 && count < 70) yellow_flash = 6, green_flash_modifier = 60;
      else if (count >= 70 && count < 80) yellow_flash = 7, green_flash_modifier = 70;
      else if (count >= 80 && count < 90) yellow_flash = 8, green_flash_modifier = 80;
      else if (count >= 90) yellow_flash = 9, green_flash_modifier = 90;
      
      if (!flash_10_done){
        flash_x_count(yellow_flash, YELLOW_LED, flash_10_done);
      }
      // Flash the Green Last
      else {
        flash_x_count(count - green_flash_modifier, GREEN_LED, flash_done);
      }
    }
    if (flash_done && flash_10_done) {
      // If done flashing, take a break.
      flash_break = true;
      flash_break_count = millis();
    }
  }
  else {
    if (flash_break_count + 1000 < millis()) {
      // Break finish, Start flashing the number again.
      flash_break = false;
      flash_done = false;
      flash_10_done = false;
    }
  }
}


void flash_x_count(byte count, byte LED, bool &done) {
  // Flash a given LED x number of times.
  if (flash_start + 150 < millis()) {
    flash_start = millis();
    digitalWrite(LED, !digitalRead(LED));
    if (!digitalRead(LED)) {
      flash_count++;
    }
  }
  if(flash_count >= count) {
    if (digitalRead(LED)) digitalWrite(LED, LOW);
    done = true;
    flash_count = 0;
  }
}

