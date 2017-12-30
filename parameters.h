/**
 * \file parameters.h
 * \brief Global parameters and structuresdefinition
 * 
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics <www.balearicdynamics.com> SPAIN
 *  \date Decembre 2017
 *  \version 1.0
 *  Licensed under GNU LGPL 3.0  
 */

#ifndef _PARAMETERS
#define _PARAMETERS

// Options default values
#define DEFAULTCYCLES 1       ///< Number of cycles by default
#define DEFAULTSPEED 1        ///< Default motion speed in minutes per cycle
#define DEFAULTOPTION 1       ///< Default startup option shown
#define MAXOPTIONS 5          ///< Max number of options (corresponding to emergency button)

#define LCD_OPTION1 1         ///< Menu option #1
#define LCD_OPTION2 2         ///< Menu option #2
#define LCD_OPTION3 3         ///< Menu option #3
#define LCD_OPTION4 4         ///< Menu option #4
#define LCD_OPTION5 5         ///< Menu option #5

#define COMMAND_NOCOMMAND 0   ///< No command button pressed
#define COMMAND_LEFT 1        ///< Left command button ID
#define COMMAND_RIGHT 2       ///< Right command button ID

#define MIN_TIME 1            ///< Minimum one-cycle duration (minutes)
#define MAX_TIME 6            ///< Maximum one-cycle duration (minutes)
#define MIN_CYCLES 1          ///< Minimum number of cycles
#define MAX_CYCLES 51         ///< Maximum number of cycles
#define MIN_ANALOG 0          ///< Minimum analog reading
#define MAX_ANALOG 1023       ///< Maximum analog reading (depends on the analog resolution)

#define APP_SET_TIME 1        ///< Application status: Set time per cycle
#define APP_SET_CYCLES 2      ///< Application status: Set number of cycles

/**
 * State of the enriconment options
 */
struct options {
  volatile int optionsLevel;       ///< Current option level
  volatile boolean optionChanged;  ///< The option button has been pressed and should be processed
  boolean motorOn;        ///< Motor running status
  boolean isRotating;     ///< Motor rotation status
  volatile int motorDir;           ///< Current motor direction of rotation
  volatile boolean emergency;      ///< Emergency status
  volatile boolean rightLimit;     ///< Detected the right endstop switch
  volatile boolean leftLimit;      ///< Detected the left endstop switch
  int appStatus;          ///< Status of the application
  int numSteps;           ///< Max number of steps between limiters
  int baseSpeed;          ///< Motor RPM for 1 minute cycle
};

#define EEPROM_ADDRESS 0X00  ///< EEPROM address to read/write the configuration variables
/**
 * Configuration parameters, saved as single object to the EEPROM
 */
struct configObject {
  int cycleTime;          ///< Motor speed expressed in seconds per cycle
  int numCycles;          ///< Number of cycles
};

#endif

