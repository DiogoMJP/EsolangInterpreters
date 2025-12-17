#include <stdio.h>
#include "state_machine_loader.h"

state_machine* create_state_machine_loader() {
	#ifdef DEBUG
		printf("Initializing state machine loader...\n");
	#endif
	state_machine *sm_loader = create_state_machine(12);

	#ifdef DEBUG
		printf("Adding states to state machine loader...\n");
	#endif
	transition *t0_0 = create_transition(" \t\n", 3, 0);
	transition *t0_1 = create_transition("0123456789", 10, 1);
	transition *t0_10 = create_transition("#", 1, 10);
	transition **transitions0 = (transition**) malloc(3 * sizeof(transition*));
	transitions0[0] = t0_0;
	transitions0[1] = t0_1;
	transitions0[2] = t0_10;
	add_state(sm_loader, 0, 3, transitions0);

	transition *t1_1 = create_transition("0123456789", 10, 1);
	transition *t1_2 = create_transition("-", 1, 2);
	transition **transitions1 = (transition**) malloc(2 * sizeof(transition*));
	transitions1[0] = t1_1;
	transitions1[1] = t1_2;
	add_state(sm_loader, 1, 2, transitions1);

	transition *t2_3 = create_transition("'", 1, 3);
	transition *t2_6 = create_transition("0123456789", 10, 6);
	transition **transitions2 = (transition**) malloc(2 * sizeof(transition*));
	transitions2[0] = t2_3;
	transitions2[1] = t2_6;
	add_state(sm_loader, 2, 2, transitions2);

	transition *t3_4 = create_transition(
		" !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz~", 92, 4
	);
	transition **transitions3 = (transition**) malloc(1 * sizeof(transition*));
	transitions3[0] = t3_4;
	add_state(sm_loader, 3, 1, transitions3);

	transition *t4_5 = create_transition("'", 1, 5);
	transition **transitions4 = (transition**) malloc(1 * sizeof(transition*));
	transitions4[0] = t4_5;
	add_state(sm_loader, 4, 1, transitions4);

	transition *t5_2 = create_transition(",", 1, 2);
	transition *t5_7 = create_transition("-", 1, 7);
	transition **transitions5 = (transition**) malloc(2 * sizeof(transition*));
	transitions5[0] = t5_2;
	transitions5[1] = t5_7;
	add_state(sm_loader, 5, 2, transitions5);

	transition *t6_2 = create_transition(",", 1, 2);
	transition *t6_6 = create_transition("0123456789", 10, 6);
	transition *t6_7 = create_transition("-", 1, 7);
	transition **transitions6 = (transition**) malloc(3 * sizeof(transition*));
	transitions6[0] = t6_2;
	transitions6[1] = t6_6;
	transitions6[2] = t6_7;
	add_state(sm_loader, 6, 3, transitions6);

	transition *t7_8 = create_transition("0123456789", 10, 8);
	transition **transitions7 = (transition**) malloc(1 * sizeof(transition*));
	transitions7[0] = t7_8;
	add_state(sm_loader, 7, 1, transitions7);

	transition *t8_8 = create_transition("0123456789", 10, 8);
	transition *t8_9 = create_transition(";", 2, 9);
	transition **transitions8 = (transition**) malloc(2 * sizeof(transition*));
	transitions8[1] = t8_8;
	transitions8[2] = t8_9;
	add_state(sm_loader, 8, 2, transitions8);

	transition *t9_0 = create_transition("\n", 1, 0);
	transition *t9_10 = create_transition(" \t", 2, 9);
	transition *t9_11 = create_transition("#", 1, 10);
	transition **transitions9 = (transition**) malloc(3 * sizeof(transition*));
	transitions9[0] = t9_0;
	transitions9[2] = t9_10;
	transitions9[3] = t9_11;
	add_state(sm_loader, 9, 4, transitions9);

	transition *t10_0 = create_transition("\n", 1, 0);
	transition *t10_10 = create_transition(" \t", 2, 10);
	transition *t10_11 = create_transition("#", 1, 11);
	transition **transitions10 = (transition**) malloc(3 * sizeof(transition*));
	transitions10[0] = t10_0;
	transitions10[1] = t10_10;
	transitions10[2] = t10_11;
	add_state(sm_loader, 10, 3, transitions10);

	transition *t11_0 = create_transition("\n", 1, 0);
	transition *t11_11 = create_transition(
		" !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz~", 92, 11
	);
	transition **transitions11 = (transition**) malloc(2 * sizeof(transition*));
	transitions11[0] = t11_0;
	transitions11[1] = t11_11;
	add_state(sm_loader, 11, 2, transitions11);

	return sm_loader;
}


int main() {
	#ifdef DEBUG
		printf("Creating state machine loader...\n");
	#endif
	state_machine *sm_loader = create_state_machine_loader();

	#ifdef DEBUG
		printf("State machine:\n");
		char *sm_str = state_machine_to_string(sm_loader);
		if (sm_str != NULL) {
			printf("%s", sm_str);
			free(sm_str);
		}
	#endif

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