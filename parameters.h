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
#define DEFAULTSPEED 30       ///< Default motion speed in seconds per cycle
#define DEFAULTOPTION 1       ///< Default startup option shown
#define MAXOPTIONS 5          ///< Max number of options (corresponding to emergency button)

#define LCD_OPTION1 1
#define LCD_OPTION2 2
#define LCD_OPTION3 3
#define LCD_OPTION4 4
#define LCD_OPTION5 5

/**
 * State of the enriconment options
 */
struct options {
  volatile int optionsLevel;       ///< Current option level
  volatile boolean optionChanged;  ///< The option button has been pressed and should be processed
  int cycleTime;          ///< Motor speed expressed in seconds per cycle
  int numCycles;          ///< Number of cycles
  boolean motorOn;        ///< Motor running status
  volatile boolean emergency;      ///< Emergency status
  volatile boolean rightLimit;     ///< Detected the right endstop switch
  volatile boolean leftLimit;      ///< Detected the left endstop switch
};

#endif

