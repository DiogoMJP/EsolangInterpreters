#include "state_machine_loader.h"


state_machine* create_state_machine_loader() {
	#ifdef DEBUG
		printf("Initializing state machine loader...\n");
	#endif
	state_machine *sm_loader = create_state_machine(13);

	#ifdef DEBUG
		printf("Adding states to state machine loader...\n");
	#endif
	transition *t0_0 = create_transition(" \t\n", 3, 0);
	transition *t0_1 = create_transition("0123456789", 10, 1);
	transition *t0_12 = create_transition("#", 1, 12);
	transition **transitions0 = (transition**) malloc(3 * sizeof(transition*));
	transitions0[0] = t0_0;
	transitions0[1] = t0_1;
	transitions0[2] = t0_12;
	add_state(sm_loader, 0, 3, transitions0);

	transition *t1_1 = create_transition("0123456789", 10, 1);
	transition *t1_2 = create_transition("-", 1, 2);
	transition **transitions1 = (transition**) malloc(2 * sizeof(transition*));
	transitions1[0] = t1_1;
	transitions1[1] = t1_2;
	add_state(sm_loader, 1, 2, transitions1);

	transition *t2_3 = create_transition("'", 1, 3);
	transition *t2_7 = create_transition("0123456789", 10, 7);
	transition **transitions2 = (transition**) malloc(2 * sizeof(transition*));
	transitions2[0] = t2_3;
	transitions2[1] = t2_7;
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

	transition *t5_6 = create_transition(",", 1, 6);
	transition *t5_8 = create_transition("-", 1, 8);
	transition **transitions5 = (transition**) malloc(2 * sizeof(transition*));
	transitions5[0] = t5_6;
	transitions5[1] = t5_8;
	add_state(sm_loader, 5, 2, transitions5);

	transition *t6_3 = create_transition("'", 1, 3);
	transition *t6_7 = create_transition("0123456789", 10, 7);
	transition **transitions6 = (transition**) malloc(2 * sizeof(transition*));
	transitions6[0] = t6_3;
	transitions6[1] = t6_7;
	add_state(sm_loader, 6, 2, transitions6);

	transition *t7_6 = create_transition(",", 1, 6);
	transition *t7_7 = create_transition("0123456789", 10, 7);
	transition *t7_8 = create_transition("-", 1, 8);
	transition **transitions7 = (transition**) malloc(3 * sizeof(transition*));
	transitions7[0] = t7_6;
	transitions7[1] = t7_7;
	transitions7[2] = t7_8;
	add_state(sm_loader, 7, 3, transitions7);

	transition *t8_9 = create_transition("0123456789", 10, 9);
	transition **transitions8 = (transition**) malloc(1 * sizeof(transition*));
	transitions8[0] = t8_9;
	add_state(sm_loader, 8, 1, transitions8);

	transition *t9_9 = create_transition("0123456789", 10, 9);
	transition *t9_10 = create_transition(";", 1, 10);
	transition **transitions9 = (transition**) malloc(2 * sizeof(transition*));
	transitions9[0] = t9_9;
	transitions9[1] = t9_10;
	add_state(sm_loader, 9, 2, transitions9);

	transition *t10_0 = create_transition("\n", 1, 0);
	transition *t10_11 = create_transition(" \t", 2, 11);
	transition *t10_12 = create_transition("#", 1, 12);
	transition **transitions10 = (transition**) malloc(3 * sizeof(transition*));
	transitions10[0] = t10_0;
	transitions10[1] = t10_11;
	transitions10[2] = t10_12;
	add_state(sm_loader, 10, 3, transitions10);

	transition *t11_0 = create_transition("\n", 1, 0);
	transition *t11_11 = create_transition(" \t", 2, 11);
	transition *t11_12 = create_transition("#", 1, 12);
	transition **transitions11 = (transition**) malloc(3 * sizeof(transition*));
	transitions11[0] = t11_0;
	transitions11[1] = t11_11;
	transitions11[2] = t11_12;
	add_state(sm_loader, 11, 3, transitions11);

	transition *t12_0 = create_transition("\n", 1, 0);
	transition *t12_12 = create_transition(
		" !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz~", 92, 12
	);
	transition **transitions12 = (transition**) malloc(2 * sizeof(transition*));
	transitions12[0] = t12_0;
	transitions12[1] = t12_12;
	add_state(sm_loader, 12, 2, transitions12);

	return sm_loader;
}


state_machine *load_state_machine_from_file(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Error opening file: %s\n", filename);
		return NULL;
	}

	state_machine *loader = create_state_machine_loader();

	state_machine_buffer *smb = NULL;
	int src_node = 0;
	char cond_char = 0;
	cond_buffer *cb = NULL;
	int dest_node = 0;
	for (char c = getc(file); c != EOF; c = getc(file)) {
		char curr_state = state_transition(loader, c);
		if (curr_state == -1) {
			#ifdef DEBUG
				printf("ERROR: Invalid character: %c\n", c);
			#endif
			delete_state_machine(loader);
			fclose(file);
			return NULL;
		}

		switch (curr_state) {
			case 1:
				src_node = src_node * 10 + c - '0';
				break;
			case 4:
				cond_char = c;
				break;
			case 6:
				cb = add_to_cond_buffer(cb, cond_char);
				cond_char = 0;
				break;
			case 7:
				cond_char = cond_char * 10 + c - '0';
				break;
			case 8:
				cb = add_to_cond_buffer(cb, cond_char);
				break;
			case 9:
				dest_node = dest_node * 10 + c - '0';
				break;
			case 10:
				smb = add_to_state_machine_buffer(smb, src_node, cb, dest_node);
				/* Reset all temporary variables */
				src_node = 0;
				cond_char = 0;
				cb = NULL;
				dest_node = 0;
				break;
			default:
				break;
		}
	}

	delete_state_machine(loader);
	fclose(file);

	state_machine *sm = create_state_machine_from_buffer(smb);

	delete_state_machine_buffer(smb);

	return sm;
}


state_machine *load_state_machine_from_string(const char *input_string) {
	state_machine *loader = create_state_machine_loader();

	state_machine_buffer *smb = NULL;
	int src_node = 0;
	char cond_char = 0;
	cond_buffer *cb = NULL;
	int dest_node = 0;
	for (int i = 0; input_string[i] != '\0'; i++) {
		char c = input_string[i];
		char curr_state = state_transition(loader, c);
		if (curr_state == -1) {
			#ifdef DEBUG
				printf("ERROR: Invalid character: %c\n", c);
			#endif
			delete_state_machine(loader);
			return NULL;
		}

		switch (curr_state) {
			case 1:
				src_node = src_node * 10 + c - '0';
				break;
			case 4:
				cond_char = c;
				break;
			case 6:
				cb = add_to_cond_buffer(cb, cond_char);
				cond_char = 0;
				break;
			case 7:
				cond_char = cond_char * 10 + c - '0';
				break;
			case 8:
				cb = add_to_cond_buffer(cb, cond_char);
				break;
			case 9:
				dest_node = dest_node * 10 + c - '0';
				break;
			case 10:
				smb = add_to_state_machine_buffer(smb, src_node, cb, dest_node);
				/* Reset all temporary variables */
				src_node = 0;
				cond_char = 0;
				cb = NULL;
				dest_node = 0;
				break;
			default:
				break;
		}
	}

	delete_state_machine(loader);

	state_machine *sm = create_state_machine_from_buffer(smb);

	delete_state_machine_buffer(smb);

	return sm;
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

	#ifdef DEBUG
		printf("Loading state machine from file...\n");
	#endif

	state_machine *sm = load_state_machine_from_file("state_machines/loader.sm");
	if (sm == NULL) {
		fprintf(stderr, "Failed to load state machine from file.\n");
		return -1;
	}

	#ifdef DEBUG
		printf("Loaded state machine:\n");
		sm_str = state_machine_to_string(sm);
		if (sm_str != NULL) {
			printf("%s", sm_str);
			free(sm_str);
		}
	#endif

	#ifdef DEBUG
		printf("Testing state machine loader transitions...\n");
	#endif
	printf("Current state: %d\n", sm->curr_state);
	while (sm->curr_state != -1) {
		input = getchar();
		getchar(); // consume newline
		state_transition(sm, input);
		printf("Current state: %d\n", sm->curr_state);
	}
	
	#ifdef DEBUG
		printf("Deleting loaded state machine...\n");
	#endif
	delete_state_machine(sm);
	
	return 0;
}