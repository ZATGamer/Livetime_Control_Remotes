void deBounce() {
  // Prevents next code from running untill the button is relesed.
  // This prevents 100 button presses when you only want 1.
  // Turns on the RED LED to let you know to Let Go of the Button.
  // enters admin menu if held for more then 2 seconds and a 2nd button is pressed.
  unsigned long start = millis();
  bool ran = false;
  byte two_buttons;
  int batt_LED = select_battery_status_light();
  while(check_buttons() != 255) {
    two_buttons = 0;
    ran = true;
    if (!digitalRead(batt_LED)){
      digitalWrite(batt_LED, HIGH);
    }
    for(byte i = 0; i < active_button_count; i++) {
      if(digitalRead(active_button_pins[i]) == LOW) two_buttons++;
    }
    if (two_buttons >= 2 && start + 5000 < millis()) {
      admin();
    }
  }
  if (ran) digitalWrite(batt_LED, LOW);
  delay(20);
}


byte check_buttons() {
  // Loop though all active buttons and read state.
  // If button is pressed. Return that button index number.
  // If no button pressed. Return 255.
  for(byte button = 0; button < active_button_count; button++) {
    if (digitalRead(active_button_pins[button]) == LOW) {
      return button;
    }
  }
  return 255;
}


void read_config() {
  // Read all active button configs from EEPROM.
  for (byte button = 0; button < active_button_count; button++) {
    inputKeycodes[button] = EEPROM.read(active_button_config_addresses[button]);
  }
}


String translate_to_keycode(byte config_value){
  // This translates a general byte to what HID Key Command that is.
  String key_press_code;
  switch(config_value){
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      // Left Control Then Number.
      key_press_code = F("01-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 21:
      // Page Up
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 22:
      // Page Dwon
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 23:
      // Up Arrow
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 24:
      // Down Arrow
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 25:
      // S
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 26:
      // X
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 27:
      // V
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
    case 28:
      // Return Key
      key_press_code = F("00-00-");
      key_press_code += String (hex_codes[config_value], HEX);
      break;
  }
  return key_press_code;
}


void build_active() {
  // Build out the Active Buttons Array based on what EEPROM says is active.
  byte is_active = 0;
  for(byte i = 0; i < potential_buttons_count; i++){
    if(EEPROM.read(potential_buttons[i]) == 1) {
      active_button_pins[is_active] = button_pins[i];
      active_button_config_addresses[is_active] = potential_buttons_config[i];
      is_active++;
    }
  }
  active_button_count = is_active;
}


void setup_buttons(){
  // Setup the buttons, This allows for a recheck after a Hard Reset without a power cycle.
  if (EEPROM.read(first_time_address) != 1) first_time();;
  
  // Now build out the active buttons
  build_active();
  
  for (int thisButton = 0; thisButton < active_button_count; thisButton++) {
    pinMode(active_button_pins[thisButton], INPUT_PULLUP); // Button Pin modes
  }
}


float battery_voltage() {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  return measuredvbat;
}


int select_battery_status_light() {
  float batt_level = battery_voltage();
  if (batt_level >= 3.8) {
    return GREEN_LED;
  }
  else if (batt_level >= 3.5 && batt_level < 3.8) {
    return YELLOW_LED;
  }
  else if (batt_level >= 3.3 && batt_level < 3.5) {
    return RED_LED;
  }
}


void send_key_press(byte button) {
  // Send the command of the button press  
  String command = "AT+BLEKEYBOARDCODE=";
  String keycode = translate_to_keycode(inputKeycodes[button]);

  // Release the key
  ble.print(F("AT+BLEKEYBOARDCODE="));
  ble.println(keycode);
  ble.sendCommandCheckOK(F("AT+BLEKEYBOARDCODE=00-00"));
}


void bluetooth_setup() {
  /* Disable command echo from Bluefruit */
  ble.echo(false);

  /* Print Bluefruit information */
//  ble.info();
  
  String device_name = (F("AT+GAPDEVNAME=APD LiveTime "));
  device_name += get_device_name(EEPROM.read(device_name_address));
  if (! ble.println(device_name) ) {
    error(F("Could not set device name?"));
  }

  /* Enable HID Service */
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    if ( !ble.sendCommandCheckOK(F( "AT+BleHIDEn=On" ))) {
      error(F("Could not enable Keyboard"));
    }
  }else
  {
    if (! ble.sendCommandCheckOK(F( "AT+BleKeyboardEn=On"  ))) {
      error(F("Could not enable Keyboard"));
    }
  }

  /* Add or remove service requires a reset */
  if (! ble.reset() ) {
    error(F("Couldn't reset??"));
  }
  
  // Turn off the MODE LED
  ble.sendCommandCheckOK(F("AT+HWMODELED=disable"));
}


