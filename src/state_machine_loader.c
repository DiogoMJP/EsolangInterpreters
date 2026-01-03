#include "state_machine_loader.h"

#define COND_N_CHARS 10
#define HASH_MAP_N_BINS 8


typedef struct cond_buffer {
    char buffer[COND_N_CHARS];
    int length;
    struct cond_buffer *next;
} cond_buffer;

typedef struct transition_buffer {
    cond_buffer *cond_buffers;
    int n_conds;
    int dest_state;
    struct transition_buffer *next;
} transition_buffer;

typedef struct state_buffer {
    int id;
    transition_buffer *transitions;
    int n_transitions;
    struct state_buffer *next;
} state_buffer;

typedef struct state_machine_buffer {
	state_buffer *states[HASH_MAP_N_BINS];
	int n_states;
} state_machine_buffer;


cond_buffer* add_to_cond_buffer(cond_buffer *cb, char c) {
	/* Case where the cond_buffer has not been initialized */
	if (cb == NULL) {
		cond_buffer *cb = (cond_buffer*) malloc(sizeof(cond_buffer));
		cb->buffer[0] = c;
		cb->length = 1;
		cb->next = NULL;
		return cb;
	}
	/* Case where the cond_buffer has been initialized but is not full */
	if (cb->length < COND_N_CHARS) {
		cb->buffer[cb->length++] = c;
		return cb;
	}
	/* Case where the cond_buffer has been initialized and is full */
	cb->next = add_to_cond_buffer(cb->next, c);

	return cb;
}

transition_buffer* add_to_transition_buffer(
	transition_buffer *tb, cond_buffer *cb, int dest_id
) {
	if (tb == NULL) {
		transition_buffer *tb = (transition_buffer*) malloc(sizeof(transition_buffer));
		tb->cond_buffers = cb;
		tb->n_conds = 1;
		tb->dest_state = dest_id;
		tb->next = NULL;
		
		return tb;
	}
	tb->next = add_to_transition_buffer(tb->next, cb, dest_id);

	return tb;
}

state_buffer* add_to_state_buffer(
	state_buffer *sb, int id, cond_buffer *cb, int dest_id
) {
	if (sb == NULL) {
		state_buffer *sb = (state_buffer*) malloc(sizeof(state_buffer));
		sb->id = id;
		sb->transitions = add_to_transition_buffer(NULL, cb, dest_id);
		sb->n_transitions = 1;
		sb->next = NULL;
		return sb;
	}
	if (sb->id == id) {
		sb->transitions = add_to_transition_buffer(sb->transitions, cb, dest_id);
		sb->n_transitions++;
		return sb;
	}
	sb->next = add_to_state_buffer(sb->next, id, cb, dest_id);

	return sb;
}

state_machine_buffer* add_to_state_machine_buffer(
	state_machine_buffer *smb, int id, cond_buffer *cb, int dest_id
) {
	if (smb == NULL) {
		state_machine_buffer *smb = (state_machine_buffer*) malloc(sizeof(state_machine_buffer));
		for (int i = 0; i < HASH_MAP_N_BINS; i++) {
			smb->states[i] = NULL;
		}
		smb->n_states = 0;
	}
	int bin = id % HASH_MAP_N_BINS;
	smb->states[bin] = add_to_state_buffer(smb->states[bin], id, cb, dest_id);

	return smb;
}

int cound_conditions_in_cb(cond_buffer *cb) {
	if (cb == NULL) {
		return -1;
	}

	int n = 0;
	cond_buffer *curr = cb;
	while (curr != NULL) {
		n += curr->length;
		curr = curr->next;
	}

	return n;
}

int count_states_in_smb(state_machine_buffer *smb) {
	if (smb == NULL) {
		return -1;
	}

	int n = 0;
	for (int i = 0; i < HASH_MAP_N_BINS; i++) {
		state_buffer *curr = smb->states[i];
		while (curr != NULL) {
			n++;
			curr = curr->next;
		}
	}

	return n;
}


state_machine *create_state_machine_from_buffer(state_machine_buffer *smb) {
	if (smb == NULL) {
		return NULL;
	}
	
	smb->n_states = count_states_in_smb(smb);
	if (smb->n_states == -1) {
		return NULL;
	}
	state_machine *sm = create_state_machine(smb->n_states);

	for (int i = 0; i < HASH_MAP_N_BINS; i++) {
		state_buffer *curr_state = smb->states[i];
		while (curr_state != NULL) {
			transition **transitions = (transition**) malloc(
				curr_state->n_transitions * sizeof(transition*)
			);
			transition_buffer *curr_transition = curr_state->transitions;
			int t_index = 0;
			while (curr_transition != NULL) {
				int n_conds = cound_conditions_in_cb(curr_transition->cond_buffers);
				if (n_conds == -1) {
					// Error handling
					return NULL;
				}
				char *conditions = (char*) malloc(
					n_conds * sizeof(char)
				);
				cond_buffer *curr_cb = curr_transition->cond_buffers;
				int c_index = 0;
				while (curr_cb != NULL) {
					for (int j = 0; j < curr_cb->length; j++) {
						conditions[c_index++] = curr_cb->buffer[j];
					}
				}
				transitions[t_index++] = create_transition(
					conditions, n_conds, curr_transition->dest_state
				);
				curr_transition = curr_transition->next;
			}
			add_state(sm, curr_state->id, curr_state->n_transitions, transitions);
			curr_state = curr_state->next;
		}
	}

	return sm;
}


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
	transition *t9_10 = create_transition(";", 2, 10);
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
			fprintf(stderr, "Invalid character: %c\n", c);
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