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

#include "project_hardware_init.h"

#define	TEN_MS	500000 // this will be used to have Timer 4 interrupt every 10 ms	

void config_buttons(void) {
	 
	gpio_enable_port(GPIOF_BASE);
	gpio_config_enable_input(GPIOF_BASE, PF0);
	gpio_config_digital_enable(GPIOF_BASE, PF0);
	gpio_config_enable_pullup(GPIOF_BASE, PF0);
	GPIOF->IS &= ~PF0;
	GPIOF->ICR |= PF0;
	GPIOF->IBE &= ~PF0;
	GPIOF->IEV |= PF0;
	GPIOF->IM |= PF0;
	NVIC_SetPriority(gpio_get_irq_num(GPIOF_BASE), 1);
	NVIC_EnableIRQ(gpio_get_irq_num(GPIOF_BASE));
	
	io_expander_write_reg(MCP23017_IODIRB_R, 0xFF); // configure pins for push buttons on IO expander to be inputs
	io_expander_write_reg(MCP23017_GPPUB_R, 0xFF);// enable pull up registers in IO expander for 4 push buttons
	
	io_expander_write_reg(MCP23017_IPOLB_R, 0x00);
	io_expander_write_reg(MCP23017_IOCONB_R, 0x0);
	
	//set up pins on IO expander for push pins to generate interrupts
	io_expander_write_reg(MCP23017_GPINTENB_R, 0xFF);	
	io_expander_write_reg(MCP23017_DEFVALB_R, 0xFF);
	io_expander_write_reg(MCP23017_INTCONB_R, 0xFF);
	
//	
//	GPIOF->IS &= ~IO_EXPANDER_IRQ_PIN_NUM;

//  // Clear any outstanding interrupts on PF0
//  GPIOF->ICR |= IO_EXPANDER_IRQ_PIN_NUM;

//  // Interrupt is controlled by IEV
//  GPIOF->IBE &= ~IO_EXPANDER_IRQ_PIN_NUM;

//  // Set the interrupts as rising edge interrupts
//  GPIOF->IEV |= IO_EXPANDER_IRQ_PIN_NUM;

//  // Enable the Interrupt Mask for PF0
//  GPIOF->IM |= IO_EXPANDER_IRQ_PIN_NUM;

//  // Set the Priority
//  NVIC_SetPriority(GPIOF_IRQn, 0);

//  // Enable the Interrupt in the NVIC
//  NVIC_EnableIRQ(GPIOF_IRQn);
//	io_expander_write_reg(MCP23017_IODIRA_R, 0x00);		// Set leds to outputs
//	io_expander_write_reg(MCP23017_GPPUA_R, 0x00);		// No pull-up on leds
//	
//	io_expander_write_reg(MCP23017_IODIRB_R, 0xFF);		// Set buttons to inputs
//	io_expander_write_reg(MCP23017_GPPUB_R, 0xFF);		// Pull-up on buttons
//	
//	io_expander_write_reg(MCP23017_IPOLB_R, 0x00);
//	// Initialize interrupts for buttons //
//	io_expander_write_reg(MCP23017_IOCONB_R, 0x0);

//	io_expander_write_reg(MCP23017_GPINTENB_R, 0xFF);	
//	io_expander_write_reg(MCP23017_DEFVALB_R, 0xFF);
//	io_expander_write_reg(MCP23017_INTCONB_R, 0xFF);

}

void init_hardware(void) {
	init_serial_debug(true, true);
	
	lcd_config_gpio();
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
	ps2_initialize();
	lp_io_init();
	ft6x06_init();
	eeprom_init();
	io_expander_init();
	io_expander_write_reg(MCP23017_IODIRA_R, 0x00); //configure the pins for LEDs on the IO Expander to be outputs
	config_buttons();
	gp_timer_config_32(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, 50000000, false, true);
	gp_timer_config_32(TIMER2_BASE, TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
	gp_timer_config_32(TIMER3_BASE, TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
	gp_timer_config_16(TIMER4_BASE, TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
	gp_timer_config_32(TIMER5_BASE, TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
}

