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

#include "settings.h"
#include "displaystrings.h"
#include "parameters.h"

// Undef to remove serial debug
#define _DEBUG

// initialize the library by associating any needed LCD interface pin
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

options sysStatus;

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
  pinMode(SETTING_BUTTON, INPUT);
  pinMode(ACTION_BUTTON_1, INPUT);
  pinMode(ACTION_BUTTON_2, INPUT);
  pinMode(EMERGENCY_BUTTON, INPUT);

  lcdWelcome();

  // Initialize the options panel
  sysStatus.optionsLevel = DEFAULTOPTION;
  sysStatus.optionChanged = false;
  sysStatus.cycleTime = DEFAULTCYCLES;
  sysStatus.numCycles = DEFAULTSPEED;
  sysStatus.motorOn = false;
  sysStatus.emergency = false;
  sysStatus.rightLimit = false;
  sysStatus.leftLimit = false;
  
  // Show the initial option on the LCD
  lcdShowOption();

  // Enable the interrupt vectors
  attachInterrupt(digitalPinToInterrupt(SETTING_BUTTON), switchOption, LOW);
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
    delay(50);
    }
    interrupts(); // re-enable the interrupts
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
      case LCD_OPTION5:
        lcd.setCursor(0,0);
        lcd.print(OPTION5);
        lcd.setCursor(0,1);
        lcd.print(OPTION5AB);
        break;
      case LCD_OPTION6:     // Emergency STOP
        lcd.setCursor(0,0);
        lcd.print(OPTIONX);
        lcd.setCursor(0,1);
        lcd.print(OPTIONXAB);
        break;
    }
  }

  // ******************************************************
  // Input buttons functions
  // ******************************************************

  /**
   * Change the current buttons options, depending on the status
   * 
   * If motor is running, the onvly available option is OPTION1 
   * Start/Stop and Info commands.
   * 
   * Option button rotates over all the available options excluding the emergency
   * controlled by the emergency button
   */
  void switchOption(){
    // Remporary disable interrupts
    noInterrupts();
    if(sysStatus.motorOn == false) {
      sysStatus.optionChanged = true;
      sysStatus.optionsLevel++;
      // Check for max number of options reached
      // to make the options button circular
      if(sysStatus.optionsLevel >= MAXOPTIONS){
        sysStatus.optionsLevel = LCD_OPTION1;
      } // cycle options
#ifdef _DEBUG
      _debugSerial("*** Opton button interrupt callback");
      _debugSerial("    New option number:");
      _debugSerial(String(sysStatus.optionsLevel));
#endif
    } // Motor not running, options cycle
    else {
      // Nothing done, re-enable the interrupts
      interrupts();
#ifdef _DEBUG
      _debugSerial("*** Motor running, ignore option button");
#endif
    }
  }

  /**
   * Debug function. Send a debug string to the serial to USB port.
   */
#ifdef _DEBUG
  void _debugSerial(String m) {
    Serial.println(m);
  }
#endif

