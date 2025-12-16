#ifndef __STATE_MACHINE_LOADER_H__
#define __STATE_MACHINE_LOADER_H__

#include <stdio.h>

#include "state_machine.h"


/* 
	Creates and returns a state machine configured
	for loading source code.
*/
state_machine* create_state_machine_loader();

#endif