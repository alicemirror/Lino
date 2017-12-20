/**
 * \file settings.h
 * \brief Hardware constant and defines
 * 
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics <www.balearicdynamics.com> SPAIN
 *  \date Decembre 2017
 *  \version 1.0
 *  Licensed under GNU LGPL 3.0  
 */

#ifndef _SETTING
#define _SETTING

#include "LiquidCrystal.h"

// *********************
// Pin definitions
// *********************
#define LEFT_LED 10           ///< Left limiter switch signal
#define RIGHT_LED 11          ///< Right limniter switch signal
#define EMERGENCY_LED 24      ///< Emergency LED signaling the status together with the display

#define ANALOG_SETTING A0      ///< Analog input potentiometer for parameters setting

#define LEFT_LIMITER 2        ///< Left limiter switch (interrupt controlled)
#define IRQ_LEFT_LIMITER 0    ///< Left limiter switch interrupt number

#define RIGHT_LIMITER 3       ///< Right limiter switch (interrupt controlled)
#define IRQ_RIGHT_LIMITER 1   ///< Right limiter switch interrupt number

#define SETTING_BUTTON 18     ///< Options setting button (interrutp controlled)
#define IRQ_SETTING_BUTTON 5  ///< Options setting button interrupt number

#define EMERGENCY_BUTTON 19     ///< Emergency button. Stop immediately every activity (interrupt controlled)
#define IRQ_EMERGENCY_BUTTON 4  ///< Options setting button interrupt number

#define ACTION_BUTTON_1 23    ///< Action button 1
#define ACTION_BUTTON_2 22    ///< Action button 2

// *********************
// Display pin settings
// *********************
#define LCD_RS 8  ///< LCD rs signal
#define LCD_EN 9  ///< LCD en (enable)
#define LCD_D4 4  ///< Data bit
#define LCD_D5 5  ///< Data bit
#define LCD_D6 6  ///< Data bit
#define LCD_D7 7  ///< Data bit

#define LCD_ROWS 2    ///< LCD number of rows
#define LCD_COL 16    ///< LCD number of columns

#define WELCOME_DELAY 5000   ///< Delay (ms) of the welcome message when system starts
#define COMMAND_DELAY 2000   ///< Delay (ms) showing the command execution result
#endif




