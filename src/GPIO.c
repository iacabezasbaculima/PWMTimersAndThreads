#include <MKL25Z4.H>
#include <stdbool.h>
#include "../include/gpio.h"

/*----------------------------------------------------------------------------
  GPIO Input Configuration

  Initialse a Port D pin as an input, with no interrupt
  Bit number given by TONE_BUTTON_POS and VOLUME_BUTTON_POS
 *----------------------------------------------------------------------------*/
void configureGPIOinput(void) {
    SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */

    /* Select GPIO and enable pull-up resistors and no interrupts */
    PORTD->PCR[TONE_BUTTON_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0);
    
    /* Set port D switch bit to inputs */
    PTD->PDDR &= ~MASK(TONE_BUTTON_POS);
		
		// VOLUME //
		/* Select GPIO and enable pull-up resistors and no interrupts */
    PORTD->PCR[VOLUME_BUTTON_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0);
    
    /* Set port D switch bit to inputs */
    PTD->PDDR &= ~MASK(VOLUME_BUTTON_POS);
}

/* ----------------------------------------
     Configure GPIO output for Audio
       1. Enable clock to GPIO port
       2. Enable GPIO port
       3. Set GPIO direction to output
       4. Ensure output low
 * ---------------------------------------- */
void configureGPIOoutput(void) {
    // Enable clock to port A
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    
    // Make pin GPIO
    PORTA->PCR[AUDIO_POS] &= ~PORT_PCR_MUX_MASK;          
    PORTA->PCR[AUDIO_POS] |= PORT_PCR_MUX(1);          
    
    // Set ports to outputs
    PTA->PDDR |= MASK(AUDIO_POS);

    // Turn off output
    PTA->PCOR = MASK(AUDIO_POS);
} ;

/*----------------------------------------------------------------------------
  isTonePressed: test the switch

  Operating the switch connects the input to ground. A non-zero value
  shows the switch is not pressed.
 *----------------------------------------------------------------------------*/
bool isTonePressed(void) {
    if (PTD->PDIR & MASK(TONE_BUTTON_POS)) {
        return false ;
    }
    return true ;
}
/*----------------------------------------------------------------------------
  isVolumePressed: test the switch

  Operating the switch connects the input to ground. A non-zero value
  shows the switch is not pressed.
 *----------------------------------------------------------------------------*/
bool isVolumePressed(void) {
    if (PTD->PDIR & MASK(VOLUME_BUTTON_POS)) {
        return false ;
    }
    return true ;
}
/* ----------------------------------------
     Toggle the Audio Output 
 * ---------------------------------------- */
void audioToggle(void) {
    PTA->PTOR = MASK(AUDIO_POS) ;
}
