void admin() {
  // This is the Admin menu.
  flash_ALL();
  deBounce();

  // Setup menu variables 
  int menu_item = 0;
  byte last_menu_item;
  boolean x = true;

  // Enter the Menu Loop
  while(x) {
    // Check Button Press
    byte button_press = check_buttons();
    
    if(button_press == 0){
      // Button A pressed go to next menu item.
      menu_item++;
      // Make sure the button is let go before continuing.
      deBounce();
    }
    if (menu_item > 2) menu_item = 0; // Set menu_item to valid value if out of range.
    
    switch (menu_item) {
      case 0:
        if (last_menu_item != menu_item) {
          // Make sure lights are off if you just switched menu items.
          last_menu_item = menu_item;
          lights_off();
        }
        
        // Bind Bluetooth
        if (button_press == 1) {
          // Delete any BLE bonds.
          if (ble.sendCommandCheckOK(F( "AT+GAPDELBONDS" ))) {
            flash_ALL();
            x = false;
          }
          else{
            flash_ALL();
            flash_ALL();
            flash_ALL();
          }
        }
        blink_LED(GREEN_LED);
        break;
      case 1:
        if (last_menu_item != menu_item) {
          // Make sure lights are off if you just switched menu items.
          last_menu_item = menu_item;
          lights_off();
        }
        
        // Set Button Keyboard Value
        if (button_press == 1) {
          deBounce();
          // Loop though all active buttons to configure each.
          for(byte button = 0; button < active_button_count; button++) {
            // Figure out what Key your setting.
            byte key = 255;
            while(key == 255){
              green_yellow();
              key = check_buttons();
            }
            // Enter set_key for button just pressed.
            set_key(active_button_config_addresses[key]);
          }

          // Exit Menu
          x = false;
        }
        blink_LED(YELLOW_LED);
        break;
      case 2:
        // Exit Menus
        if (last_menu_item != menu_item) {
          // Make sure lights are off if you just switched menu items.
          last_menu_item = menu_item;
          lights_off();
        }
        if (button_press == 1) {
          // Exit Menu
          x = false;
        }
        blink_LED(RED_LED);
        break;
    }
  }
  // Flash all the lights 2 times to indicate exiting the menu.
  for (byte i = 0; i < 2; i++) {
    flash_ALL();
  }
  deBounce();
}


void first_time() {
  flash_ALL();

  // Tempory setup of buttons A and B
  // Must be done as this is called before button setup.
  active_button_pins[0] = button_pins[0];
  active_button_pins[1] = button_pins[1];
  active_button_count = 2;
  
  // Set all potential buttons to input and set Pullup's HIGH
  for (byte thisButton = 0; thisButton < potential_buttons_count; thisButton++) {
    pinMode(button_pins[thisButton], INPUT_PULLUP); // Button Pin modes
  }

  // Instead of having to press Button A for all potential Buttons,
  // If you press enter without selecting another key it will exit.
  bool continue_discovery = true;
  
  // Now figure out what buttons are "active" Pins 2 and 3 are always active.
  for (byte i = 0; i < potential_buttons_count; i++) {
    deBounce();
    
    bool x = true;
    bool button_on = false;
    byte button_pressed;

    // No Button discovered, Exit.
    if (!continue_discovery) break;
    
    if(i != 0 && i != 1){ // as Buttons A and B are requires, Will just set them as ON.
      
      while(x) {
        // Turn the RED LED on if a button was discovered.
        if(button_on) {
          if(!digitalRead(RED_LED)) digitalWrite(RED_LED, HIGH);
        }
        else {
          if(digitalRead(RED_LED)) digitalWrite(RED_LED, LOW);
        }

        flash_count_c(i);
        
        for (byte button = 2; button < potential_buttons_count; button++) {
          // Check all buttons starting with Button C 
          if(digitalRead(button_pins[button]) == LOW) {
            // If a button other the A and B were Pressed set this to true.
            button_on = true;
            button_pressed = button;
            deBounce();
          }
        }

        if(digitalRead(button_pins[0]) == LOW) {
          // if button a pressed
          if(button_on){
            // and button_on toggled
            if (EEPROM.read(potential_buttons[button_pressed]) != 1) {
              // and EEPROM doesn't already show this as active
              EEPROM.write(potential_buttons[button_pressed], 1); // Write to EEPROM
            }
          }
          // if button_on false
          else continue_discovery = false; // Exit discovery
          x = false;
          deBounce();
        }
      }
      lights_off();
    }
    else {
      // The EEPROM check for buttons A and B
      if (EEPROM.read(potential_buttons[i]) != 1) {
        EEPROM.write(potential_buttons[i], 1);
      }
    }
  }
  // If you made it this far. 
  // You are done with the first time Setup. 
  // So we will Set the EEPROM as so.
  if (EEPROM.read(first_time_address) != 1) {
    EEPROM.write(first_time_address, 1);
  }

  // Now set all the Potential Button Pins to OUTPUT and LOW (Power savings)
  // The active buttons will be reset up in the Main Setup Section.
  for (byte thisButton = 0; thisButton < potential_buttons_count; thisButton++) {
    pinMode(button_pins[thisButton], OUTPUT);
    digitalWrite(button_pins[thisButton], LOW);
  }
}


void set_key(int key_address) {
  lights_off();
  deBounce();
  
  // Check EEPROM first and set menu_item to current config.
  byte menu_item = EEPROM.read(key_address);
  
  boolean x = true;
  while(x) {
    byte button_press = check_buttons();
    if(button_press == 0){
      menu_item++;
    }
    
    if (menu_item > 28) menu_item = 1; // Set menu_item to valid value if out of range.

    // Potential HID commands that the button can be set to.
    switch(menu_item) {
      case 1:
        flash_count_c(1);
        if (button_press == 1) EEPROM.write(key_address, 1), x = false;
        break;
      case 2:
        flash_count_c(2);
        if (button_press == 1) EEPROM.write(key_address, 2), x = false;
        break;
      case 3:
        flash_count_c(3);
        if (button_press == 1) EEPROM.write(key_address, 3), x = false;
        break;
      case 4:
        flash_count_c(4);
        if (button_press == 1) EEPROM.write(key_address, 4), x = false;
        break;
      case 5:
        flash_count_c(5);
        if (button_press == 1) EEPROM.write(key_address, 5), x = false;
        break;
      case 6:
        flash_count_c(6);
        if (button_press == 1) EEPROM.write(key_address, 6), x = false;
        break;
      case 7:
        flash_count_c(7);
        if (button_press == 1) EEPROM.write(key_address, 7), x = false;
        break;
      case 8:
        flash_count_c(8);
        if (button_press == 1) EEPROM.write(key_address, 8), x = false;
        break;
      case 9:
        flash_count_c(9);
        if (button_press == 1) EEPROM.write(key_address, 9), x = false;
        break;
      case 10:
        flash_count_c(10);
        if (button_press == 1) EEPROM.write(key_address, 10), x = false;
        break;
      case 11:
        flash_count_c(11);
        if (button_press == 1) EEPROM.write(key_address, 11), x = false;
        break;
      case 12:
        flash_count_c(12);
        if (button_press == 1) EEPROM.write(key_address, 12), x = false;
        break;
      case 13:
        flash_count_c(13);
        if (button_press == 1) EEPROM.write(key_address, 13), x = false;
        break;
      case 14:
        flash_count_c(14);
        if (button_press == 1) EEPROM.write(key_address, 14), x = false;
        break;
      case 15:
        flash_count_c(15);
        if (button_press == 1) EEPROM.write(key_address, 15), x = false;
        break;
      case 16:
        flash_count_c(16);
        if (button_press == 1) EEPROM.write(key_address, 16), x = false;
        break;
      case 17:
        flash_count_c(17);
        if (button_press == 1) EEPROM.write(key_address, 17), x = false;
        break;
      case 18:
        flash_count_c(18);
        if (button_press == 1) EEPROM.write(key_address, 18), x = false;
        break;
      case 19:
        flash_count_c(19);
        if (button_press == 1) EEPROM.write(key_address, 19), x = false;
        break;
      case 20:
        flash_count_c(20);
        if (button_press == 1) EEPROM.write(key_address, 20), x = false;
        break;
      case 21:
        flash_count_c(21);
        if (button_press == 1) EEPROM.write(key_address, 21), x = false;
        break;
      case 22:
        flash_count_c(22);
        if (button_press == 1) EEPROM.write(key_address, 22), x = false;
        break;
      case 23:
        flash_count_c(23);
        if (button_press == 1) EEPROM.write(key_address, 23), x = false;
        break;
      case 24:
        flash_count_c(24);
        if (button_press == 1) EEPROM.write(key_address, 24), x = false;
        break;
      case 25:
        flash_count_c(25);
        if (button_press == 1) EEPROM.write(key_address, 25), x = false;
        break;
      case 26:
        flash_count_c(26);
        if (button_press == 1) EEPROM.write(key_address, 26), x = false;
        break;
      case 27:
        flash_count_c(27);
        if (button_press == 1) EEPROM.write(key_address, 27), x = false;
        break;
      case 28:
        flash_count_c(28);
        if (button_press == 1) EEPROM.write(key_address, 28), x = false;
        break;
    }
    deBounce();
  }
  read_config();
  lights_off();
}


void Hard_Reset() {
  // Reset all EEPROM values to 255 (defalut)
  for(int i = 0; i < 1025; i++) {
    if (EEPROM.read(i) != 255) EEPROM.write(i, 255);
  }
  // Flash 3 times to indicate Hard Reset Complete.
  for (byte i = 0; i < 3; i++) {
    flash_ALL();
  }
}


void set_device_name() {
  flash_ALL();
  deBounce();
  
  // Check EEPROM first and set menu_item to current config.
  byte menu_item = EEPROM.read(device_name_address);
  byte last_item;
  bool x = true;
  while(x) {
    byte button_press = check_buttons();
    if(button_press == 0){
      menu_item++;
    }
  
    if (menu_item > 10) menu_item = 0; // Set menu_item to valid value if out of range.
  
    if (menu_item == 0) {
      // flip green/Yellow
      if(last_item != menu_item) {
        last_item = menu_item;
        lights_off();
      }
      green_yellow();
    }
    else {
      if(last_item != menu_item) {
        last_item = menu_item;
        lights_off();
      }
      flash_count_c(menu_item);
    }
  
    if (button_press == 1) {
      if (menu_item != EEPROM.read(device_name_address)){
        EEPROM.write(device_name_address, menu_item);
      }
      x = false;
    }
    deBounce();
  }
}


String get_device_name(byte device_name) {
  switch(device_name) {
    case 0:
      // Admin
      return (F("Admin Remote"));
    case 1:
      // Remote 1
      return (F("Remote 1"));
    case 2:
      // Remote 2
      return (F("Remote 2"));
    case 3:
      // Remote 3
      return (F("Remote 3"));
    case 4:
      // Remote 4
      return (F("Remote 4"));
    case 5:
      // Remote 5
      return (F("Remote 5"));
    case 6:
      // Remote 6
      return (F("Remote 6"));
    case 7:
      // Remote 7
      return (F("Remote 7"));
    case 8:
      // Remote 8
      return (F("Remote 8"));
    case 9:
      // Remote 9
      return (F("Remote 9"));
    case 10:
      // Remote 10
      return (F("Remote 10"));
    case 255:
      // Default Name
      return (F("Remote"));
  }
}

