#include <stdbool.h>

// Definitions for GPIO
#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

// MASKing
#define MASK(x) (1UL << (x))

// Button is on port D - bit 6, J2 Pin 17
#define TONE_BUTTON_POS (6)// audio tone button pin
#define TONE_EVT (0) // signal number of audio tone button

// Button is on port D - bit 7, J2 Pin 19 
#define VOLUME_BUTTON_POS (7) // volume button pin
#define VOLUME_EVT (1) // signal number of volume button


// Button states
#define UP (0)
#define BOUNCE (1)
#define DOWN (2)

#define BOUNCEP (5)  // x delay give bounce time out

// GPIO output used for the frequency, port A pin 2
#define AUDIO_POS (2)

// Function prototypes
void configureGPIOinput(void) ; 	  // Initialise button
void configureGPIOoutput(void) ;	  // Initialise output	
void audioToggle(void) ;        		// Toggle the output GPIO
bool isTonePressed(void);						// Is tone button pressed
bool isVolumePressed(void);					// Is volume button pressed

#endif
