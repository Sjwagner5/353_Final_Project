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

//Timer 4 ISR will check the ADC value
void TIMER4A_Handler(void) {
	return;
}

//Timer 5 ISR will move the orange
void TIMER5A_Handler(void) {
	ALERT_ORANGE = true;
	TIMER5->ICR |= TIMER_ICR_TATOCINT;
	return;
}
