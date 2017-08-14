ISR (PCINT0_vect) {
  // handle pin change interrupt for D08 to D13

  // Turn off Interrupts
  PCICR = 0;
  // toggle LED
//  digitalWrite (TEST_LED, !digitalRead (TEST_LED)); // THIS IS FOR TESTING ONLY
}


// ISR for INT0 - INT3
void switch_pressed() {
  // This will turn off interrupts and toggle LED for testing.
//  digitalWrite (TEST_LED, !digitalRead (TEST_LED)); // THIS IS FOR TESTING ONLY
}


void enable_interrupts() {
  for (byte i = 0; i < active_button_count; i++) {
    switch(active_button_pins[i]) {
      case 0:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 1:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 2:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 3:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;  // clear any outstanding interrupts
    }
  }
    
  PCIFR  |= bit (PCIF0);  // for D8 to D13
  
  // Enable Pin change interrupts
  PCICR  |= bit (PCIE0);  // for D8 to D13
}


void disable_interrupts() {
  for (byte i = 0; i < active_button_count; i++) {
    switch(active_button_pins[i]) {
      case 0:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 1:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 2:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 3:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;  // clear any outstanding interrupts
    }
  }
   PCICR = 0;
}


void setup_interrupts() {
  // Build out the Interrupts depending on what button pins are set as active.
  for (byte i = 0; i < active_button_count; i++) {
    switch(active_button_pins[i]) {
      case 0:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 1:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 2:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 3:
        attachInterrupt(digitalPinToInterrupt (active_button_pins[i]), switch_pressed, FALLING);
        break;
      case 10:
        PCMSK0 |= bit (PCINT6); // D10
        break;
      case 11:
        PCMSK0 |= bit (PCINT7); // D11
        break;
    }
  }
}


