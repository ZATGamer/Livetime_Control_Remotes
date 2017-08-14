#include "LowPower.h"
#include <EEPROM.h>
#include <SPI.h>

#include "Adafruit_BluefruitLE_SPI.h"
//#include "keycode.h"
#include "BluefruitConfig.h"

#include "EEPROM_locations.h"

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// Bluefruit Pin that is tied to the LiPo Battery.
#define VBATPIN A9

// Button 
int hex_codes[] = {0x00, 
                   0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                   0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                   0x4B, 0x4E, 0x52, 0x51, 0x16, 0x1B, 0x19, 0x28};

byte button_pins[] = {0, 1, 2, 3, 10, 11};
byte potential_buttons[] = {button_a_on, button_b_on, button_c_on,
                            button_d_on, button_e_on, button_f_on};
byte potential_buttons_config[] = {button_a_config, button_b_config, button_c_config,
                                   button_d_config, button_e_config, button_f_config};
byte potential_buttons_count = 6;
byte active_button_pins[6];
byte active_button_config_addresses[6];
byte active_button_count;
int inputKeycodes[6]; // HID CODES per buttons.
// END Button setup

// LED SETUP
const byte GREEN_LED = A3;
const byte YELLOW_LED = A4;
const byte RED_LED = A5;
//const byte TEST_LED = 13;
int LED_colors[3] = {GREEN_LED, YELLOW_LED, RED_LED};
int LED_count = 3;
// END LED SETUP


void setup () {
//  Serial.begin(9600); // For DEBUG ONLY

  // Start up BLE
  ble.begin();  
//  pinMode (TEST_LED, OUTPUT);

  // Setup LED's
  for (int thisLED = 0; thisLED < LED_count; thisLED++) {
    pinMode (LED_colors[thisLED], OUTPUT);
    digitalWrite(LED_colors[thisLED], LOW);  // Make sure the LED is off.
  }
  // END Setup LED's

  // Setup Battery Checking Pin.
  pinMode(VBATPIN, INPUT);
  
  // Setup Buttons
  // if this is the first run, Need to figure out what buttons there are.
  setup_buttons();
  // END Setup Buttons

  // On Boot Buttons to either Hard Reset or enter admin menu's
  if (digitalRead(active_button_pins[0]) == LOW && digitalRead(active_button_pins[1])== LOW) {
    // Factory Reset
    lights_on();
    Hard_Reset();
    ble.factoryReset();
    lights_off();
    deBounce();
    setup_buttons();
  }
  else if (digitalRead(active_button_pins[0]) == LOW) {
    // This will enter the device naming area.
    set_device_name();
  }
  
  bluetooth_setup();

// moved this to the debounce.
//  if (check_buttons() != 255) admin();  // Easter Egg to Enter admin menus at boot.
//   Build what key each button will send.
  read_config();
  
  // pin change interrupts
  setup_interrupts();
  
  lights_on();
  delay(250);
  lights_off();
}


void loop () {
  // Check for button press
  byte button_pressed = check_buttons();  
  
  if (button_pressed != 255) {
    // Send the key commands based on which button is pressed
    send_key_press(button_pressed);
  
    // DeBounce the button to prevent more then 1 key send 
    // And enters menus if 2 buttons pressed for 1 second
    deBounce();
    
  }

  // Put processor back to sleep.
  enable_interrupts();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  disable_interrupts();
}

