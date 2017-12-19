/**
 * \file Lino.h
 * \brief Main sketch sources
 * 
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics <www.balearicdynamics.com> SPAIN
 *  \date Decembre 2017
 *  \version 1.0
 *  Licensed under GNU LGPL 3.0  
 */

#include <EEPROM.h>
#include <Streaming.h>
#include "settings.h"
#include "displaystrings.h"
#include "parameters.h"

// Undef to remove serial debug
#undef _DEBUG

// initialize the library by associating any needed LCD interface pin
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

options sysStatus;
configObject savedParameters;

/**
 * \breif Initialization function
 */
void setup() {

#ifdef _DEBUG
  Serial.begin(38400);
#endif
  lcd.begin(LCD_COL, LCD_ROWS);

  // initialize digital pins
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
  pinMode(EMERGENCY_LED, OUTPUT);

  pinMode(LEFT_LIMITER, INPUT);
  pinMode(LEFT_LIMITER, INPUT);
  pinMode(SETTING_BUTTON, INPUT_PULLUP);
  pinMode(ACTION_BUTTON_1, INPUT);
  pinMode(ACTION_BUTTON_2, INPUT);
  pinMode(EMERGENCY_BUTTON, INPUT);

  lcdWelcome();

  // Initialize the options panel
  sysStatus.optionsLevel = DEFAULTOPTION;
  sysStatus.optionChanged = false;
  sysStatus.motorOn = false;
  sysStatus.emergency = false;
  sysStatus.rightLimit = false;
  sysStatus.leftLimit = false;
  
  // Show the initial option on the LCD
  lcdShowOption();

  // Enable the interrupt vectors
  attachInterrupt(IRQ_SETTING_BUTTON, switchOption, LOW);
  attachInterrupt(IRQ_LEFT_LIMITER, switchLeft, CHANGE);
  attachInterrupt(IRQ_RIGHT_LIMITER, switchRight, CHANGE);
  attachInterrupt(IRQ_EMERGENCY_BUTTON, emergency, CHANGE);
}

/**
 * Main loop
 */
void loop(){

  // Check if the option status has changed and update the display
  if(sysStatus.optionChanged) {
    lcd.clear();
    sysStatus.optionChanged = false;
    lcdShowOption();
    }

    // If we are in emergency status, the setting button is nore reset until
    // the emergency status is disabled
    if(sysStatus.emergency == false) {
      delay(500);
      attachInterrupt(IRQ_SETTING_BUTTON, switchOption, LOW); // re-enable the interrupts
      // Check for command buttons. Command buttons can't be executed
      // when the system is in emergency state
      doCommand(checkCommandButtons());
    }
  } // loop

  // ******************************************************
  // EEPROM functions
  // ******************************************************
  /**
   * Read the last saved configuration stored in the EEPROM
   */
  void loadConfiguration() {
    EEPROM.get(EEPROM_ADDRESS, savedParameters);
  }

  /**
   * Save the current configuration in the EEPROM
   */
   void saveConfiguration() {
      EEPROM.put(EEPROM_ADDRESS, savedParameters);
   }

   /**
    * Reset the configuration to the default parameters
    */
    void resetConfiguration() {
      // Initialize the parameters to the default
      savedParameters.cycleTime = DEFAULTSPEED;
      savedParameters.numCycles = DEFAULTCYCLES;
      EEPROM.put(EEPROM_ADDRESS, savedParameters);
    }

  // ******************************************************
  // Display functions
  // ******************************************************

  /**
   * \brief Start message
   */
  void lcdWelcome() {
    lcd.display();
    lcd.setCursor(0,0);
    lcd.print(START1);
    lcd.setCursor(0,1);
    lcd.print(START2);
    delay(WELCOME_DELAY);
    lcd.clear();
    lcd.noDisplay();
  }

  /**
   * \brief Shows an option panel on the LCD
   */
  void lcdShowOption() {
    lcd.display();

    switch(sysStatus.optionsLevel) {
      case LCD_OPTION1:
        lcd.setCursor(0,0);
        lcd.print(OPTION1);
        lcd.setCursor(0,1);
        lcd.print(OPTION1AB);
        break;
      case LCD_OPTION2:
        lcd.setCursor(0,0);
        lcd.print(OPTION2);
        lcd.setCursor(0,1);
        if(sysStatus.motorOn)
          lcd.print(OPTION2AB_12);
        else
          lcd.print(OPTION2AB_11);
        break;
      case LCD_OPTION3:
        lcd.setCursor(0,0);
        lcd.print(OPTION3);
        lcd.setCursor(0,1);
        lcd.print(OPTION3AB);
        break;
      case LCD_OPTION4:
        lcd.setCursor(0,0);
        lcd.print(OPTION4);
        lcd.setCursor(0,1);
        lcd.print(OPTION4AB);
        break;
      case LCD_OPTION5:     // Emergency STOP
        lcd.setCursor(0,0);
        lcd.print(OPTIONX);
        lcd.setCursor(0,1);
        lcd.print(OPTIONXAB);
#ifdef _DEBUG
      // Test the EEPROM read/write
      _debugSerial("*** Opton button interrupt callback");
      _debugSerialEndl();
#endif
        break;
    }
  }

  // ******************************************************
  // Input buttons functions
  // ******************************************************
  /**
   * Change the current buttons options, depending on the status
   * 
   * If motor is running, the only available option is OPTION1 
   * Start/Stop and Info commands.
   * 
   * Emergency status avoid the controls and display update.
   * 
   * Option button rotates over all the available options excluding the emergency
   * controlled by the emergency button
   */
  void switchOption(){
    if( (sysStatus.motorOn == false) || (sysStatus.emergency) ) {
      detachInterrupt(IRQ_SETTING_BUTTON);
      sysStatus.optionChanged = true;
      sysStatus.optionsLevel++;
      // Check for max number of options reached
      // to make the options button circular
      if(sysStatus.optionsLevel >= MAXOPTIONS){
        sysStatus.optionsLevel = LCD_OPTION1;
      } // cycle options
#ifdef _DEBUG
      _debugSerial("*** Opton button interrupt callback");
      _debugSerial(" - New option number:");
      _debugSerial(String(sysStatus.optionsLevel));
      _debugSerialEndl();
#endif
    } // Motor not running, options cycle
  }

  /**
   * Check if a command button has been pressed and return the corresponding button ID
   * else return 0
   */
  int checkCommandButtons() {
    if(digitalRead(ACTION_BUTTON_1) == false){
      return COMMAND_LEFT;
    } else if(digitalRead(ACTION_BUTTON_2) == false) {
      return COMMAND_RIGHT;
    } else {
      return COMMAND_NOCOMMAND;
    }
  }

  /**
   * Execute the command button depending on the state of the system.
   */
  void doCommand(int side) {
    if(side != COMMAND_NOCOMMAND) {
      lcd.clear();
      switch(sysStatus.optionsLevel) {
        case LCD_OPTION1:             // Reset, Zero
          if(side == COMMAND_LEFT) {
            resetConfiguration();     // Reset the EEPROM configuration to the defaults
            lcd.setCursor(0,0);
            lcd.print(OPTION1A_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION1A_12);
          } // left
          else if(side == COMMAND_RIGHT) {  // Motor auto-zero (both sides)
            lcd.setCursor(0,0);
            lcd.print(OPTION1B_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION1B_12);
          }
          break;
        case LCD_OPTION2:
          // Left command available only when motor is not runnig
          if(side == COMMAND_LEFT) {
            if(sysStatus.motorOn == false) {
              lcd.setCursor(0,0);
              lcd.print(OPTION2A_11);
              lcd.print(String(savedParameters.cycleTime));
              lcd.setCursor(0,1);
              lcd.print(OPTION2A_12);
              lcd.print(String(savedParameters.numCycles));
            }
            else {
              lcd.setCursor(0,0);
              lcd.print(OPTION2C_11);
              lcd.print(String(savedParameters.cycleTime));
              lcd.setCursor(0,1);
              lcd.print(OPTION2C_12);
              lcd.print(String(savedParameters.numCycles));
              sysStatus.isRotating = true;
            }
          }
          else if(side == COMMAND_RIGHT) {
              lcd.setCursor(0,0);
              lcd.print(OPTION2B_11);
              lcd.setCursor(0,1);
              lcd.print(OPTION2B_12);
            sysStatus.motorOn = !sysStatus.motorOn;
            sysStatus.isRotating = false;
          }
          break;
//        case LCD_OPTION3:
//          lcd.setCursor(0,0);
//          lcd.print(OPTION3);
//          lcd.setCursor(0,1);
//          lcd.print(OPTION3AB);
//          break;
//        case LCD_OPTION4:
//          lcd.setCursor(0,0);
//          lcd.print(OPTION4);
//          lcd.setCursor(0,1);
//          lcd.print(OPTION4AB);
//          break;
      } // command processing
      delay(COMMAND_DELAY); // Show the message then continue
      lcd.clear();
      lcdShowOption();
    } // process command side (not 0)
  }
  
  /**
   * Set the emergency button status on change
   */
  void emergency(){
    detachInterrupt(IRQ_EMERGENCY_BUTTON);
    if(digitalRead(EMERGENCY_BUTTON)){
      // If the emergency status off comes from an emergency recovery
      // condition, the button IRQ, status and option should be reset
      if(sysStatus.emergency = true) {
        sysStatus.optionsLevel = LCD_OPTION1; // First option on the display
        sysStatus.optionChanged = true; // Set the option status changed
      }
      sysStatus.emergency = false;
      digitalWrite(EMERGENCY_LED, LOW);
    }
    else {
      // We are in emergency status
      sysStatus.emergency = true; // Set the emergency flag
      digitalWrite(EMERGENCY_LED, HIGH);  // Enable the emergency LED 
      sysStatus.optionsLevel = LCD_OPTION5; // Force the emergency option status ID
      sysStatus.optionChanged = true; // Set the option status changed
    }
    attachInterrupt(IRQ_EMERGENCY_BUTTON, emergency, CHANGE);
#ifdef _DEBUG
    // Test EEPROM
    _debugSerial("*** EEPROM Test, saving default values");
    _debugSerialEndl();
    resetConfiguration();

    _debugSerial("*** Default saved, configuration object set to zero ");
    savedParameters.cycleTime = 0;
    savedParameters.numCycles = 0;
    _debugSerial("*** cycleTime = ");
    _debugSerial(String(savedParameters.cycleTime));
    _debugSerial(", numCycles = ");
    _debugSerial(String(savedParameters.numCycles));
    _debugSerialEndl();

    _debugSerial("*** Load configuration from EEPROM");
    loadConfiguration();
    _debugSerial("*** cycleTime = ");
    _debugSerial(String(savedParameters.cycleTime));
    _debugSerial(", numCycles = ");
    _debugSerial(String(savedParameters.numCycles));
    _debugSerialEndl();
#endif
  }

  // ******************************************************
  // Limiter switches
  // ******************************************************
  /**
   * Set the limiter switch status on change
   */
  void switchLeft(){
    detachInterrupt(IRQ_LEFT_LIMITER);
    if(digitalRead(LEFT_LIMITER)){
      sysStatus.leftLimit = false;
      digitalWrite(LEFT_LED, LOW);
    }
    else {
      sysStatus.leftLimit = true;
      digitalWrite(LEFT_LED, HIGH);
    }
    attachInterrupt(IRQ_LEFT_LIMITER, switchLeft, CHANGE);
  }

  /**
   * Set the limiter switch status on change
   */
  void switchRight(){
    detachInterrupt(IRQ_RIGHT_LIMITER);
    if(digitalRead(RIGHT_LIMITER)){
      sysStatus.rightLimit = false;
      digitalWrite(RIGHT_LED, LOW);
    }
    else {
      sysStatus.rightLimit = true;
      digitalWrite(RIGHT_LED, HIGH);
    }
    attachInterrupt(IRQ_RIGHT_LIMITER, switchRight, CHANGE);
  }

  /**
   * Debug function. Send a debug string to the serial to USB port.
   */
#ifdef _DEBUG
  void _debugSerial(String m) {
    Serial << m;
  }

  void _debugSerialEndl() {
    Serial << endl;
  }
#endif

