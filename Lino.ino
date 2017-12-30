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
#include <Stepper.h>
#include "settings.h"
#include "displaystrings.h"
#include "parameters.h"

// Undef to remove serial debug
#define _DEBUG

// initialize the library by associating any needed LCD interface pin
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Create the stepper library instance
Stepper motor = Stepper(STEPS, STEP, DIR);

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
  pinMode(ENABLE, OUTPUT);

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

  // Get the last saved configuration parameters from EEPROM
  loadConfiguration();

  // Enable the interrupt vectors
  attachInterrupt(IRQ_SETTING_BUTTON, switchOption, LOW);
  attachInterrupt(IRQ_LEFT_LIMITER, switchLeft, CHANGE);
  attachInterrupt(IRQ_RIGHT_LIMITER, switchRight, CHANGE);
  attachInterrupt(IRQ_EMERGENCY_BUTTON, emergency, CHANGE);

  // Set stepper
  motor.setSpeed(100); // set base speed in RPM
  // Motor disabled
  digitalWrite(ENABLE, LOW);

  // Initialize the path and position
  sysStatus.calibration = false;
  countSteps();
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

    // If we are in emergency status, the setting button is not reset until
    // the emergency status is disabled
    if(sysStatus.emergency == false) {
      // Check for command buttons. Command buttons can't be executed
      // when the system is in emergency state
      doCommand(checkCommandButtons());
    }
  } // loop

  // ******************************************************
  // Stepper functions
  // ******************************************************
  /**
   * CaLCulate the number of steps between the two limiters
   */
  void countSteps() {
    int stepCount;
    int j;
    float startMillis, endMillis, elapsedMillis;
    
    // First step: rotates to right until the limiter is on
    // at high speed
    motor.setSpeed(HIGH_SPEED);
    // Enable the motor
    digitalWrite(ENABLE, HIGH);
    sysStatus.motorOn = true;
    // Start motion
    sysStatus.motorDir = MOVE_RIGHT;
    // Loop until the limiter switch is activated by interrupt
    // and disble the motor 
    while(sysStatus.rightLimit == false) {
      if(sysStatus.emergency == false) {
        motor.step(sysStatus.motorDir); // One step at a time
      }
      else {
        // Emergency, stop all activity
        digitalWrite(ENABLE, HIGH);
        sysStatus.motorOn = false;
        sysStatus.isRotating = false;
        sysStatus.calibration = false;
        return;
      }
    }
    // Step back steps then move slow to find exactly the right
    // limit position
    for(j = 0; j < STEP_BACK; j++) {
      motor.step(MOVE_LEFT); // One step at a time
    }
    motor.setSpeed(LOW_SPEED);
    sysStatus.rightLimit = false; // reset the limiter flag
    while(sysStatus.rightLimit == false) {
      if(sysStatus.emergency == false) {
        motor.step(sysStatus.motorDir); // One step at a time, slow motion
      }
      else {
        // Emergency, stop all activity
        digitalWrite(ENABLE, LOW);
        sysStatus.motorOn = false;
        sysStatus.isRotating = false;
        sysStatus.calibration = false;
        return;
      }
    }
    // Start counting the steps inverting the direction, 
    // until the opposite limiter is not met and the time needed 
    // on the base od the LOW_SPEED motor RPM
    stepCount = 0;
    sysStatus.motorDir = MOVE_LEFT;
    startMillis = millis();
    while(sysStatus.leftLimit == false) {
      if(sysStatus.emergency == false) {
        motor.step(sysStatus.motorDir); // One step at a time
        stepCount++;
      }
      else {
        // Emergency, stop all activity
        digitalWrite(ENABLE, LOW);
        sysStatus.motorOn = false;
        sysStatus.isRotating = false;
        sysStatus.calibration = false;
        return;
      }
    }
    endMillis = millis();
    // Update the number of steps
    sysStatus.numSteps = stepCount;
    // Calculate the duration in ms
    elapsedMillis = endMillis - startMillis;
    // Calculate the motor RPM speed to cover the entire path in 1 minute (60k ms)
    sysStatus.baseSpeed = LOW_SPEED / (ONE_MINUTE / elapsedMillis);
#ifdef _DEBUG
      _debugSerial("*** countSteps() - Elapsed millis = ");
      _debugSerial(String(elapsedMillis));
      _debugSerialEndl();
      _debugSerial("*** countSteps() - Number of steps = ");
      _debugSerial(String(sysStatus.numSteps));
      _debugSerialEndl();
      _debugSerial("*** countSteps() - 1 minute base speed (RPM) = ");
      _debugSerial(String(sysStatus.baseSpeed));
      _debugSerialEndl();
#endif
    // Step back steps to go out of the limiter switch
    // limit position
    for(j = 0; j < STEP_BACK; j++) {
      motor.step(MOVE_RIGHT);
    }
    // Disable the motor
    digitalWrite(ENABLE, LOW);
    sysStatus.motorOn = false;
    sysStatus.isRotating = false;
    // Set the next valid direction
    sysStatus.motorDir = MOVE_RIGHT;
    sysStatus.calibration = true;
  }

  /**
   * Run a full path to the current direction for the
   * selected duration (minutes)
   */
   void rotatePath() {
#ifdef _DEBUG
    float startMillis, endMillis, elapsedMillis;
#endif
    int j;

    // Check if motor is enabled
    if(sysStatus.motorOn == false)
      return;
    // Check for calibration
    if(sysStatus.calibration == false)
      return;
      
    // Set the RPM motor speed
    motor.setSpeed((int)(sysStatus.baseSpeed/savedParameters.cycleTime));
    // Set rotation flag
    sysStatus.isRotating = true;
    // Motion loop along the entire path
#ifdef _DEBUG
    startMillis = millis(); // Initial time
#endif
    for(j = 0; j < (sysStatus.numSteps -(STEP_BACK * 2)); j++) {
      if(sysStatus.emergency == false) {
        motor.step(sysStatus.motorDir);
      }
      else {
        // Emergency, stop all activity
        digitalWrite(ENABLE, LOW);
        sysStatus.motorOn = false;
        sysStatus.isRotating = false;
        sysStatus.calibration = false;
        return;
      }
    } // Motion loop
#ifdef _DEBUG
    endMillis = millis(); // End time
    elapsedMillis = endMillis - startMillis; // Elapsed time
    _debugSerial("*** rotatePath() - Elapsed millis = ");
    _debugSerial(String(elapsedMillis));
    _debugSerialEndl();
    _debugSerial("*** rotatePath() - Number of steps = ");
    _debugSerial(String(sysStatus.numSteps));
    _debugSerialEndl();
    _debugSerial("*** rotatePath() -  Base speed (RPM per 1 minute) = ");
    _debugSerial(String(sysStatus.baseSpeed));
    _debugSerialEndl();
    _debugSerial("*** rotatePath() -  Cycle duration in minutes = ");
    _debugSerial(String(savedParameters.cycleTime));
    _debugSerialEndl();
    _debugSerial("*** rotatePath() -  Current path speed (RPM) = ");
    _debugSerial(String(sysStatus.baseSpeed/savedParameters.cycleTime));
    _debugSerialEndl();
#endif
    // Disable rotation status
    sysStatus.isRotating = false;
    // Update the drirection
    invertDirection();
   }

  void invertDirection(){
    if(sysStatus.motorDir == MOVE_RIGHT)
      sysStatus.motorDir = MOVE_LEFT;
    else
      sysStatus.motorDir = MOVE_RIGHT;
  }

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
      delay(500);
      attachInterrupt(IRQ_SETTING_BUTTON, switchOption, LOW); // re-enable the interrupts
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
    int j;

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
            // Calculate the number of steps and save it on the global structure
            countSteps();
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(OPTION1C_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION1C_12);
          }
          delay(COMMAND_DELAY); // Show the message then continue
          break;
        case LCD_OPTION2:               // Start, rotation
          if(side == COMMAND_LEFT) {
            if(sysStatus.motorOn == false) {  // Motor stopped, show info
              lcd.setCursor(0,0);
              lcd.print(OPTION2A_11);
              lcd.print(String(savedParameters.cycleTime));
              lcd.setCursor(0,1);
              lcd.print(OPTION2A_12);
              lcd.print(String(savedParameters.numCycles));
              delay(COMMAND_DELAY); // Show the message then continue
            }
            else {                            // Motor running, start rotate
              lcd.setCursor(0,0);
              lcd.print(OPTION2C_10);
              lcd.print(String(savedParameters.cycleTime));
              lcd.print(OPTION2C_11);
              digitalWrite(ENABLE, HIGH);
              // Loop over the number of cycles
              for(j = 0; j < savedParameters.numCycles; j++){
                lcd.setCursor(0,1);
                lcd.print(OPTION2C_12);
                lcd.print(String(j + 1));
                lcd.print(OPTION2C_13);
                lcd.print(String(savedParameters.numCycles));
                rotatePath();
              }
              digitalWrite(ENABLE, LOW);
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
        case LCD_OPTION3:               // Parameters setting
          if(side == COMMAND_LEFT) {
            lcd.setCursor(0,0);
            lcd.print(OPTION3A_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION3A_12);
            // Get value settings
            sysStatus.appStatus = APP_SET_TIME;
            analogSet();
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(OPTION3C_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION3C_12);
          }
          else  if(side == COMMAND_RIGHT) {
            lcd.setCursor(0,0);
            lcd.print(OPTION3B_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION3B_12);
            // Get value settings
            sysStatus.appStatus = APP_SET_CYCLES;
            analogSet();
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(OPTION3D_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION3D_12);
          }
          delay(COMMAND_DELAY); // Show the message then continue
          break;
        case LCD_OPTION4:
          if(side == COMMAND_LEFT) {
            saveConfiguration();
            lcd.setCursor(0,0);
            lcd.print(OPTION4A_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION4A_12);
          }
          else  if(side == COMMAND_RIGHT) {
            lcd.setCursor(0,0);
            lcd.print(OPTION4B_11);
            lcd.setCursor(0,1);
            lcd.print(OPTION4B_12);
          }
          delay(COMMAND_DELAY); // Show the message then continue
          break;
      } // command processing
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
      if(sysStatus.emergency == true) {
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
  }

  // ******************************************************
  // Analog reading and parameters setting
  // ******************************************************
  /**
   * Read the user settings for the current parameter
   */
   void analogSet() {
    int analog;
    char value[4];

    // Wait few ms to avoid that the previous button is read again by
    // the function
    delay(500);

    // Read the analog value from potentiometer until the OK button
    // is not pressed
    while(digitalRead(ACTION_BUTTON_2) == HIGH) {
      lcd.setCursor(14,0);
      analog = analogRead(ANALOG_SETTING);
      if(sysStatus.appStatus == APP_SET_TIME) {
        savedParameters.cycleTime = map(analog, MIN_ANALOG, MAX_ANALOG, MIN_TIME, MAX_TIME);
        lcd.print(String(savedParameters.cycleTime));
      } else if(sysStatus.appStatus == APP_SET_CYCLES) {
        savedParameters.numCycles = map(analog, MIN_ANALOG, MAX_ANALOG, MIN_CYCLES, MAX_CYCLES);
        sprintf(value, "%2d", savedParameters.numCycles);
        lcd.print(value);
      }
    }
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

