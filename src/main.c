/*----------------------------------------------------------------------------
Student ID: ec18446
Lab 6: Activity 2 and 3

Features:
- Use button to switch between different tones
- Use PIT counts derived from MIDI notes 60-71 and loop
- Use button to control volume
- Use 1 thread to switch tone 
- Use 1 thread to poll button used to switch tone
- Use 1 thread to control volume 
- Use 1 thread to poll button used to control volume

Additional libraries used: 
- math.h: to use pow function and increase volume by powers of 2 (non-linearly).

Added functions in main.c:
- toneTask: This thread sets a timer channel with a new PIT count as its LDVAL when an explicit button is pressed.
- volumeTask: This thread increases the volume by setting a PWM duty cycle value when an explicit button is pressed.
- toneButtonTask: This thread polls a button and sets a flag used to switch tone
- volumeButtonTask: This thread polls a button and sets a flag used to control volume

Added GPIO pin and flag to poll a button to control volume in gpio.h:
- VOLUME_BUTTON_POS (7)
- VOLUME_EVT (1)
- Button is on port D - bit 7, J2 Pin 19 

Added function prototype in gpio.h and definition in gpio.c to check if volume button is pressed
- isVolumePressed: This function checks if volume button (switch) is pressed
*---------------------------------------------------------------------------*/

#include "cmsis_os2.h"
#include <MKL25Z4.H>
#include <math.h>	
#include "../include/gpio.h"
#include "../include/pit.h"
#include "../include/tpmPwm.h"


osEventFlagsId_t toneFlags ; 	// audio button event flags
osEventFlagsId_t volumeFlags;	// volume button event flags

// Flag TONE_EVT is set when the tone button is pressed
// Flag VOLUME_EVT is set when the volume button is pressed

/*--------------------------------------------------------------
 *     Tone task - switch tone on and off
 *--------------------------------------------------------------*/
// Audio tone states
#define TONE_ON (1)
#define TONE_OFF (0)

osThreadId_t t_tone;        /*  task id of task to switch tone on/off */

void toneTask (void *arg) {
	
		// Array of PIT counts derived from MIDI notes 60-71
		const uint32_t pitCounts [12] = 
		{20040 // Frequency for MIDI 60 - middle 	C 	  261.625565300599
		,18915 // Frequency for MIDI 61 - middle 	C#/Db 277.182630976872
		,17853 // Frequency for MIDI 62 - middle 	D 		293.664767917408
		,16851 // Frequency for MIDI 63 - middle 	D#/Eb 311.126983722081
		,15905 // Frequency for MIDI 64 - middle 	E 		329.62755691287
		,15013 // Frequency for MIDI 65 - middle 	F 		349.228231433004
		,14170 // Frequency for MIDI 66 - trebble F#/Gb 369.994422711634
		,13375 // Frequency for MIDI 67 - trebble G 		391.995435981749
		,12624 // Frequency for MIDI 68 - trebble G#/Ab 415.304697579945
		,11916 // Frequency for MIDI 69 - trebble A 		440
		,11247 // Frequency for MIDI 70 - trebble A#/Bb 466.16376151809
		,10616 // Frequency for MIDI 71 - trebble B 		493.883301256124
		};
		
    int audioState = TONE_OFF ;		// audio is off at program start
		uint32_t loadValueIndex = 0;	// index to iterate in PIT counts array
    uint32_t maxPITCounts = 12;		// total number of PIT counts in array
		
		while (1) {
        osEventFlagsWait (toneFlags, MASK(TONE_EVT), osFlagsWaitAny, osWaitForever);
				switch (audioState) {
            case TONE_OFF:
								// When program starts audio tone is off. After tone button press, audio tone is turned on.
								// This case only executes once
                audioState = TONE_ON;
								// Set timer with a LDVAL from the PIT counts array
								setTimer(0, *(&pitCounts[loadValueIndex]));
								loadValueIndex++;	// Increase index to get next PIT count
								startTimer(0);		// Start timer 
                break ;
						
            case TONE_ON:
                if (loadValueIndex < maxPITCounts) 
								{
									// Use next PIT count value from array to set timer
									setTimer(0, *(&pitCounts[loadValueIndex]));
									loadValueIndex++;	// Increase index to get next PIT count					
								}
								else
								{
									loadValueIndex = 0; // Reset index to loop in PIT counts array
									// Set timer with a LDVAL from the PIT counts array
									setTimer(0, *(&pitCounts[loadValueIndex])); 
									loadValueIndex++;	// Increase index to get next PIT count
								}
								startTimer(0); // Start timer in channel 0
                break ;
        }
    }
}


osThreadId_t t_volume;			/*  task id of task to control volume */

void volumeTask (void *arg) {
	
		unsigned int volume_level = 0;	// Store the current volume level
	
		while (1) {
				osEventFlagsWait(volumeFlags, MASK(VOLUME_EVT), osFlagsWaitAny, osWaitForever);
				
			// If volume level exceed maximum volume, then reset volume level
				if (volume_level > 7) 
					volume_level = 0;	// reset volume level
				else
					volume_level++;		// increase volume level
				
				// Set the PWM duty cycle. Volume increases by powers of 2 using current volume level as the exponent
				setPWMDuty((unsigned int)pow(2, volume_level));
		}
}

/*------------------------------------------------------------
 *     Tone button task - poll button and send signal when pressed
 *------------------------------------------------------------*/

osThreadId_t t_toneButton;      /* task id of task to read button */

void toneButtonTask (void *arg) {
    int bState = UP ;
    int bCounter = 0 ;
    
    while (1) {
        osDelay(10) ;
        if (bCounter) bCounter-- ;
        switch (bState) {
            case UP:
                if (isTonePressed()) {
                    osEventFlagsSet(toneFlags, MASK(TONE_EVT));
                    bState = DOWN ;
                }
                break ;
            case DOWN:
                if (!isTonePressed()) {
                    bCounter = BOUNCEP ;
                    bState = BOUNCE ;
                }
                break ;
            case BOUNCE:
                if (isTonePressed()) {
										bCounter = BOUNCEP ;
                    bState = DOWN ;
                } else {
                    if (!bCounter) {
                        bState = UP ;
                    } 
                }
                break ;
        }
    }
}

/*------------------------------------------------------------
 *     Volume button task - poll button and send signal when pressed
 *------------------------------------------------------------*/

osThreadId_t t_volumeButton;      /* task id of task to read button */

void volumeButtonTask (void *arg) {
    int bState = UP ;
    int bCounter = 0 ;
    
    while (1) {
        osDelay(10) ;
        if (bCounter) bCounter-- ;
        switch (bState) {
            case UP:
                if (isVolumePressed()) {
                    osEventFlagsSet(volumeFlags, MASK(VOLUME_EVT));
                    bState = DOWN ;
                }
                break ;
            case DOWN:
                if (!isVolumePressed()) {
                    bCounter = BOUNCEP ;
                    bState = BOUNCE ;
                }
                break ;
            case BOUNCE:
                if (isVolumePressed()) {
										bCounter = BOUNCEP ;
                    bState = DOWN ;
                } else {
                    if (!bCounter) {
                        bState = UP ;
                    } 
                }
                break ;
        }
    }
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
    configureGPIOinput() ;       // Initialise button
    configureGPIOoutput() ;      // Initialise output    
    configurePIT(0) ;            // Configure PIT channel 0
    configureTPM0forPWM() ;
    setPWMDuty(0) ;     // off volume
                        // Max is 128; off is 0 
    SystemCoreClockUpdate() ;

    // Initialize CMSIS-RTOS
    osKernelInitialize();
    
    // Create event flags
    toneFlags = osEventFlagsNew(NULL);
		volumeFlags = osEventFlagsNew(NULL);
	
    // Create threads
    t_tone = osThreadNew(toneTask, NULL, NULL) ;
    t_toneButton = osThreadNew(toneButtonTask, NULL, NULL) ;
		t_volume = osThreadNew(volumeTask, NULL, NULL);	
		t_volumeButton = osThreadNew(volumeButtonTask, NULL, NULL);
    
    osKernelStart();    // Start thread execution - DOES NOT RETURN
    for (;;) {}         // Only executed when an error occurs
}
