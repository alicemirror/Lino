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

#define _PROTOTYPE

#include "LiquidCrystal.h"

// *********************
// Pin definitions
// *********************
#define LEFT_LED 11           ///< Left limiter switch signal
#define RIGHT_LED 10          ///< Right limniter switch signal
#define EMERGENCY_LED 24      ///< Emergency LED signaling the status together with the display

#define ANALOG_SETTING A0      ///< Analog input potentiometer for parameters setting

#ifdef _PROTOTYPE
#define LEFT_LIMITER 3        ///< Left limiter switch (interrupt controlled)
#define IRQ_LEFT_LIMITER 1    ///< Left limiter switch interrupt number
#define RIGHT_LIMITER 2       ///< Right limiter switch (interrupt controlled)
#define IRQ_RIGHT_LIMITER 0   ///< Right limiter switch interrupt number
#else
#define LEFT_LIMITER 2        ///< Left limiter switch (interrupt controlled)
#define IRQ_LEFT_LIMITER 0    ///< Left limiter switch interrupt number
#define RIGHT_LIMITER 3       ///< Right limiter switch (interrupt controlled)
#define IRQ_RIGHT_LIMITER 1   ///< Right limiter switch interrupt number
#endif
#define SETTING_BUTTON 18     ///< Options setting button (interrutp controlled)
#define IRQ_SETTING_BUTTON 5  ///< Options setting button interrupt number

#define EMERGENCY_BUTTON 19     ///< Emergency button. Stop immediately every activity (interrupt controlled)
#define IRQ_EMERGENCY_BUTTON 4  ///< Options setting button interrupt number

#ifdef _PROTOTYPE
#define ACTION_BUTTON_1 22    ///< Action button 1
#define ACTION_BUTTON_2 23    ///< Action button 2
#else
#define ACTION_BUTTON_1 23    ///< Action button 1
#define ACTION_BUTTON_2 22    ///< Action button 2
#endif

// *********************
// Display pin settings
// *********************
#define LCD_RS 26  ///< LCD rs signal
#define LCD_EN 27  ///< LCD en (enable)
#define LCD_D4 28  ///< Data bit
#ifdef _PROTOTYPE
#define LCD_D5 31  ///< Data bit
#define LCD_D6 30  ///< Data bit
#define LCD_D7 33  ///< Data bit
#else
#define LCD_D5 29  ///< Data bit
#define LCD_D6 30  ///< Data bit
#define LCD_D7 31  ///< Data bit
#endif
// *********************
// Stepper motor pins
// *********************
#define STEP 4
#define DIR 5
#define ENABLE 6

// *********************
// Stepper motor defaults
// *********************

//! Steps per rotation
#define STEPS 200   
//! Drive gear teeths
#define DRIVE_GEAR 42
//! Driven gear teeths
#define DRIVEN_GEAR 75
//! Gear ratio calculation
#define GEAR_RATIO DRIVEN_GEAR/DRIVE_GEAR
//! Direction right
#define MOVE_RIGHT -1
//! Direction right
#define MOVE_LEFT 1

//! Number of stepd back while searching the limits
#define STEP_BACK 160

//! Fast speed in RPM, used to move the gear when searching
//! for the limiters
#define HIGH_SPEED 150
//! Slow speed in RPM, used to move the gear for limits fine tuning
#define LOW_SPEED 50
//! 1 minut in ms
#define ONE_MINUTE 60000

/**
 * Average expected number of steps to manage the acceleration
 * curve.
 * 
 * During the first phase of the calibration we don't know exactly
 * the number of steps. We only know that with a 1/16 microstepping setup
 * of the Moon's SR4 controller the 90 degrees path is about 6500 steps.
 * Based on this assumption we start the deceleration curve when we are around
 * step 5000. Deceleration should stop when the LOW_SPEED is reached or when
 * the limiter switch is set.
 */
#define DECELERATION_POINT 5000
/**
 * Acceleration/Deceleration increment in RPM
 */
 #define ACCELERATION_STEP 1

// *********************
// General parameters
// *********************

#define LCD_ROWS 2    ///< LCD number of rows
#define LCD_COL 16    ///< LCD number of columns

#define WELCOME_DELAY 5000   ///< Delay (ms) of the welcome message when system starts
#define COMMAND_DELAY 2000   ///< Delay (ms) showing the command execution result
#define BUTTON_DELAY 500     ///< Buttons software debouncing delay
#endif




