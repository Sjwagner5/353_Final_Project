#include "Project.h"
#include "main.h"

volatile bool ALERT_LED = false;

void project_main(void) {
	
	bool gameOver = false;
	init_hardware();
	while (!gameOver) {
		if (ALERT_LED) {
			ALERT_LED = false;
			lp_io_set_pin(RED_BIT);
		}
	}
}