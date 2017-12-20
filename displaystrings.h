/**
 * \file displaystrings.h
 * \brief Display strings definitions
 * 
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics <www.balearicdynamics.com> SPAIN
 *  \date Decembre 2017
 *  \version 1.0
 *  Licensed under GNU LGPL 3.0  
 */

#ifndef _STRINGS
#define _STRINGS

// Language settings
#define ENGLISH "EN"
#define ITALIAN "IT"
#define SPANISH "ES"

#ifdef ITALIAN

#define START1 "BalearicDynamics"
#define START2 "Lino 1.0.9 IT"

// Confirm-cancel option
#define OPTION_OK_NO "   OK   Annulla"

// --- Emergency message
#define OPTIONX "   Emergenza!"
#define OPTIONXAB "  STOP Manuale"

// ******************************************* Option 1 - Start activity
// --- Option1 level 1
#define OPTION1 "Inizio attivita'"
#define OPTION1AB "  Reset   Zero"

// --- Option1.A level 2
#define OPTION1A_11 "Reset parametri"
#define OPTION1A_12 "Completato"

// --- Option1.B level 2
#define OPTION1B_11 "Auto-zero"
#define OPTION1B_12 "Completato"

// ******************************************* Option 1 - Execute action
// --- Option2 level 1
#define OPTION2 "  Esegui azione"
// Depending on the machine status, it is shown one of
// the two alternative options (start / stop)
#define OPTION2AB_11 "  Info    Avvia"
#define OPTION2AB_12 " Rotaz.   STOP"

// --- Option2.A level 2
#define OPTION2A_11 "Tempo: "
#define OPTION2A_12 "N.Cicli: "

// --- Option2.B level 2
#define OPTION2B_11 "Start/Stop"
#define OPTION2B_12 "In corso..."

// --- Option2.C level 2
#define OPTION2C_11 "Rotazione motore"
#define OPTION2C_12 "In corso..."

// ******************************************* Option 1 - Parameters setting
// --- Option3 level 1
#define OPTION3 "  Set parametri"
#define OPTION3AB "Tempo  Rotazioni"

// --- Option3.A level 2
#define OPTION3A_11 "Tempo 1 ciclo:"
#define OPTION3A_12 "Ruota      Ok"

// --- Option3.B level 2
#define OPTION3B_11 "Cicli:"
#define OPTION3B_12 "Ruota      Ok"

// --- Option3.C level 3
#define OPTION3C_11 "Impostaz. Tempo"
#define OPTION3C_12 "Completato"

// --- Option3.D level 3
#define OPTION3D_11 "Impostaz. Cicli"
#define OPTION3D_12 "Completato"

// ******************************************* Option 1 - Save settings
// --- Option4 level 1
#define OPTION4 "Salva impostaz."
#define OPTION4AB OPTION_OK_NO

// --- Option4.A level 2
#define OPTION4A_11 "  Impostazioni"
#define OPTION4A_12 "  Dati salvati"

// --- Option4.b level 2
#define OPTION4B_11 "  Salvataggio"
#define OPTION4B_12 "  Annullato"

#endif

#ifdef ENGLISH
// Not yet translated
#endif

#ifdef SPANISH
// Not yet translated
#endif

#endif

