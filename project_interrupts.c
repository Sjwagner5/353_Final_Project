// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"
#include "project_interrupts.h"

#define OFFSET 700 //used to increase and decrease the ADC thresholds so that the joystick is not as sensitive

static volatile uint16_t PS2_X_DATA = 0;
static volatile uint16_t PS2_Y_DATA = 0;//store the y data ADC value of the joystick

//*****************************************************************************
// Returns the most current direction that was pressed.
//*****************************************************************************
PS2_DIR_t ps2_get_direction(void){
	
 if (PS2_Y_DATA > PS2_ADC_HIGH_THRESHOLD + OFFSET) {
		return PS2_DIR_UP;
			
	} else if (PS2_Y_DATA < PS2_ADC_LOW_THRESHOLD - OFFSET) {
		return PS2_DIR_DOWN;
			
	} else {
		return PS2_DIR_CENTER;
	}
}


//Timer 1 ISR that blinks the red LED to indicate that the program is running
void TIMER1A_Handler(void) {
	static int i = 0;
	if (i%2 == 0) {//alternate between turning the LED on and off each time there is a TIMER 1 interrupt
		lp_io_set_pin(RED_BIT);
	} else {
		lp_io_clear_pin(RED_BIT);
	}
	i++;
	TIMER1->ICR |= TIMER_ICR_TATOCINT;
}

//Timer 2 ISR will move the apple
void TIMER2A_Handler(void) {
	ALERT_APPLE = true;
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
	return;
}

//Timer 3 ISR will move the banana
void TIMER3A_Handler(void) {
	ALERT_BANANA = true;
	TIMER3->ICR |= TIMER_ICR_TATOCINT;
	return;
}

//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
//*****************************************************************************
void TIMER4A_Handler(void)
{	
	ADC0->PSSI |= ADC_PSSI_SS2;
	// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
}

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void)
{
	PS2_X_DATA = ADC0->SSFIFO2;//Read the x value to get the first value in the fifo out of the way, we will not use it
	PS2_Y_DATA = ADC0->SSFIFO2;
	PS2_DIR =  ps2_get_direction();
  // Clear the interrupt
  ADC0->ISC |= ADC_ISC_IN2;
}

//Timer 5 ISR will move the orange
void TIMER5A_Handler(void) {
	ALERT_ORANGE = true;
	TIMER5->ICR |= TIMER_ICR_TATOCINT;
	return;
}

void GPIOF_Handler(void) {
	ALERT_BUTTON = true;
	GPIOF->ICR |= GPIO_ICR_GPIO_M;
}
