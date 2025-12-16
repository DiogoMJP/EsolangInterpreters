#include <stdio.h>
#include "state_machine_loader.h"

state_machine* create_state_machine_loader() {
	#ifdef DEBUG
		printf("Initializing state machine loader...\n");
	#endif
	state_machine *sm_loader = create_state_machine(8);

	#ifdef DEBUG
		printf("Adding states to state machine loader...\n");
	#endif
	transition *t0_0 = create_transition(" \t\n", 3, 0);
	transition *t0_1 = create_transition("0123456789", 10, 1);
	transition *t0_7 = create_transition(";", 1, 7);
	transition **transitions0 = (transition**) malloc(3 * sizeof(transition*));
	transitions0[0] = t0_0;
	transitions0[1] = t0_1;
	transitions0[2] = t0_7;
	add_state(sm_loader, 3, transitions0);

	transition *t1_1 = create_transition("0123456789", 10, 1);
	transition *t1_2 = create_transition("-", 1, 2);
	transition **transitions1 = (transition**) malloc(2 * sizeof(transition*));
	transitions1[0] = t1_1;
	transitions1[1] = t1_2;
	add_state(sm_loader, 2, transitions1);

	transition *t2_3 = create_transition(
		" !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz~", 92, 3
	);
	transition **transitions2 = (transition**) malloc(1 * sizeof(transition*));
	transitions2[0] = t2_3;
	add_state(sm_loader, 1, transitions2);

	transition *t3_2 = create_transition(",", 1, 2);
	transition *t3_4 = create_transition("-", 1, 4);
	transition **transitions3 = (transition**) malloc(2 * sizeof(transition*));
	transitions3[0] = t3_2;
	transitions3[1] = t3_4;
	add_state(sm_loader, 2, transitions3);

	transition *t4_5 = create_transition("0123456789", 10, 5);
	transition **transitions4 = (transition**) malloc(1 * sizeof(transition*));
	transitions4[0] = t4_5;
	add_state(sm_loader, 1, transitions4);

	transition *t5_0 = create_transition("\n", 1, 0);
	transition *t5_5 = create_transition("0123456789", 10, 5);
	transition *t5_6 = create_transition(" \t", 2, 6);
	transition *t5_7 = create_transition(";", 1, 7);
	transition **transitions5 = (transition**) malloc(4 * sizeof(transition*));
	transitions5[0] = t5_0;
	transitions5[1] = t5_5;
	transitions5[2] = t5_6;
	transitions5[3] = t5_7;
	add_state(sm_loader, 4, transitions5);

	transition *t6_0 = create_transition("\n", 1, 2);
	transition *t6_6 = create_transition(" \t", 2, 6);
	transition *t6_7 = create_transition(";", 1, 7);
	transition **transitions6 = (transition**) malloc(3 * sizeof(transition*));
	transitions6[0] = t6_0;
	transitions6[1] = t6_6;
	transitions6[2] = t6_7;
	add_state(sm_loader, 3, transitions6);

	transition *t7_0 = create_transition("\n", 1, 0);
	transition *t7_7 = create_transition(
		" !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz~", 92, 7
	);
	transition **transitions7 = (transition**) malloc(2 * sizeof(transition*));
	transitions7[0] = t7_0;
	transitions7[1] = t7_7;
	add_state(sm_loader, 2, transitions7);

	return sm_loader;
}


int main() {
	#ifdef DEBUG
		printf("Creating state machine loader...\n");
	#endif
	state_machine *sm_loader = create_state_machine_loader();

	#ifdef DEBUG
		printf("Testing state machine loader transitions...\n");
	#endif
	printf("Current state: %d\n", sm_loader->curr_state);
	char input;
	while (sm_loader->curr_state != -1) {
		input = getchar();
		getchar(); // consume newline
		state_transition(sm_loader, input);
		printf("Current state: %d\n", sm_loader->curr_state);
	}

	#ifdef DEBUG
		printf("Deleting state machine loader...\n");
	#endif
	delete_state_machine(sm_loader);
	
	#ifdef DEBUG
		printf("Done.\n");
	#endif
	
	return 0;
}