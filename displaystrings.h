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
#undef ENGLISH "EN"
#define ITALIAN "IT"
#undef SPANISH "ES"

#ifdef ITALIAN

#define START1 "BalearicDynamics"
#define START2 "Lino 1.0.0 IT"

// Confirm-cancel option
#define OPTION_OK_NO "   OK   Annulla"

// --- Emergency message
#define OPTIONX "   Emergenza!"
#define OPTIONXAB "  STOP Manuale"

// --- Option1 level 1
#define OPTION1 "Inizio attivita'"
#define OPTION1AB "  Reset   Zero"

// --- Option2 level 1
#define OPTION2 "  Esegui azione"
// Depending on the machine status, it is shown one of
// the two alternative options (start / stop)
#define OPTION2AB_11 "  Info    Avvia"
#define OPTION2AB_12 "  Info    STOP"

// --- Option3 level 1
#define OPTION3 "  Set parametri"
#define OPTION3AB "Tempo  Rotazioni"

// --- Option3.A level 2
#define OPTION3A_11 "Tempo 1 ciclo:"
#define OPTION3A_12 "Ruotare Manopola"

// --- Option3.B level 2
#define OPTION3B_11 "Num. cicli:"
#define OPTION3B_12 "Ruotare Manopola"

// --- Option4 level 1
#define OPTION4 "Tempo/ciclo.:"
#define OPTION4AB "N.Cicli:"

// --- Option5 level 1
#define OPTION5 "Salva impostaz."
#define OPTION5AB OPTION_OK_NO

// --- Option5.A level 2
#define OPTION5A_11 "  Impostazioni"
#define OPTION5AB_12 " Dati salvati

#endif

#ifdef ENGLISH
// Not yet translated
#endif

#ifdef SPANISH
// Not yet translated
#endif

#endif

