#include "state_machine_buffer.h"


cond_buffer* add_to_cond_buffer(cond_buffer *cb, char c) {
	/* Case where the cond_buffer has not been initialized */
	if (cb == NULL) {
		#ifdef DEBUG
			printf("Creating new condition buffer with character: %c\n", c);
		#endif
		cb = (cond_buffer*) malloc(sizeof(cond_buffer));
		cb->buffer[0] = c;
		cb->length = 1;
		cb->next = NULL;
		return cb;
	}
	/* Case where the cond_buffer has been initialized but is not full */
	if (cb->length < COND_N_CHARS) {
		#ifdef DEBUG
			printf("Adding to condition buffer with character: %c\n", c);
		#endif
		cb->buffer[cb->length++] = c;
		#ifdef DEBUG
			printf("Condition buffer length is now: %d\n", cb->length);
		#endif
		return cb;
	}
	/* Case where the cond_buffer has been initialized and is full */
	#ifdef DEBUG
		printf("Condition buffer full, creating next buffer.\n");
	#endif
	cb->next = add_to_cond_buffer(cb->next, c);

	return cb;
}

transition_buffer* add_to_transition_buffer(
	transition_buffer *tb, cond_buffer *cb, int dest_id
) {
	/* Case where the transition_buffer has not been initialized */
	if (tb == NULL) {
		#ifdef DEBUG
			printf("Creating new transition buffer to dest state: %d\n", dest_id);
		#endif
		tb = (transition_buffer*) malloc(sizeof(transition_buffer));
		tb->cond_buffers = cb;
		tb->n_conds = 1;
		tb->dest_state = dest_id;
		tb->next = NULL;
		
		return tb;
	}
	/* Case where the transition_buffer has been initialized */
	#ifdef DEBUG
		printf("Adding to existing transition buffer list.\n");
	#endif
	tb->next = add_to_transition_buffer(tb->next, cb, dest_id);

	return tb;
}

state_buffer* add_to_state_buffer(
	state_buffer *sb, int id, cond_buffer *cb, int dest_id
) {
	/* Case where the state_buffer has not been initialized */
	if (sb == NULL) {
		#ifdef DEBUG
			printf("Creating new state buffer with id: %d\n", id);
		#endif
		sb = (state_buffer*) malloc(sizeof(state_buffer));
		sb->id = id;
		sb->transitions = add_to_transition_buffer(NULL, cb, dest_id);
		sb->n_transitions = 1;
		sb->next = NULL;
		return sb;
	}
	/* Case where the state_buffer has been initialized */
	if (sb->id == id) {
		#ifdef DEBUG
			printf("Adding transition to existing state: %d-%d\n", id, dest_id);
		#endif
		sb->transitions = add_to_transition_buffer(sb->transitions, cb, dest_id);
		sb->n_transitions++;
		return sb;
	}
	/* Case where this is not yet the correct state_buffer */
	#ifdef DEBUG
		printf("Traversing to next state buffer to add state: %d\n", id);
	#endif
	sb->next = add_to_state_buffer(sb->next, id, cb, dest_id);

	return sb;
}

state_machine_buffer* add_to_state_machine_buffer(
	state_machine_buffer *smb, int id, cond_buffer *cb, int dest_id
) {
	/* Case where the state_machine_buffer has not been initialized */
	if (smb == NULL) {
        #ifdef DEBUG
            printf("Creating state machine buffer...\n");
        #endif
		smb = (state_machine_buffer*) malloc(sizeof(state_machine_buffer));
		for (int i = 0; i < HASH_MAP_N_BINS; i++) {
			smb->states[i] = NULL;
		}
		smb->n_states = 0;
	}
	/* Add the state to the appropriate bin */
	int bin = id % HASH_MAP_N_BINS;
	smb->states[bin] = add_to_state_buffer(smb->states[bin], id, cb, dest_id);

	return smb;
}

int delete_state_machine_buffer(state_machine_buffer *smb) {
	#ifdef DEBUG
		printf("Deleting state machine buffer...\n");
	#endif
	if (smb == NULL) {
		#ifdef DEBUG
			printf("State machine buffer is NULL, nothing to delete.\n");
		#endif
		return -1;
	}
	for (int i = 0; i < HASH_MAP_N_BINS; i++) {
		state_buffer *curr_sb = smb->states[i];
		while (curr_sb != NULL) {
			transition_buffer *curr_tb = curr_sb->transitions;
			while (curr_tb != NULL) {
				cond_buffer *curr_cb = curr_tb->cond_buffers;
				while (curr_cb != NULL) {
					cond_buffer *next_cb = curr_cb->next;
					free(curr_cb);
					curr_cb = next_cb;
				}

				transition_buffer *next_tb = curr_tb->next;
				free(curr_tb);
				curr_tb = next_tb;
			}

			state_buffer *next_sb = curr_sb->next;
			free(curr_sb);
			curr_sb = next_sb;
		}
	}

	free(smb);
	
	#ifdef DEBUG
		printf("State machine buffer deleted successfully.\n");
	#endif

	return 0;
}

/*
    Counts the total number of condition characters in the
    given condition buffer linked list. If the buffer is NULL,
    returns -1.
    - cb: Pointer to the condition buffer.
    - returns: Total number of condition characters in the buffer.
*/
int count_conditions_in_cb(cond_buffer *cb) {
	if (cb == NULL) {
        #ifdef DEBUG
            printf("Condition buffer is NULL.\n");
        #endif
		return -1;
	}

	int n = 0;
	cond_buffer *curr = cb;
	while (curr != NULL) {
		n += curr->length;
		curr = curr->next;
	}

    #ifdef DEBUG
        printf("Total conditions counted: %d\n", n);
    #endif

	return n;
}

/*
    Counts the total number of states in the given condition
    buffer linked list. If the buffer is NULL, returns -1.
    - smb: Pointer to the state machine buffer.
    - returns: Total number of states in the buffer.
*/
int count_states_in_smb(state_machine_buffer *smb) {
    #ifdef DEBUG
        printf("Counting states in state machine buffer...\n");
    #endif
	if (smb == NULL) {
        #ifdef DEBUG
            printf("State machine buffer is NULL.\n");
        #endif
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

    #ifdef DEBUG
        printf("Total states counted: %d\n", n);
    #endif

	return n;
}


state_machine *create_state_machine_from_buffer(state_machine_buffer *smb) {
	/* Case where the state machine buffer is NULL */
	if (smb == NULL) {
		#ifdef DEBUG
			printf("State machine buffer is NULL.\n");
		#endif
		return NULL;
	}
	
	smb->n_states = count_states_in_smb(smb);
	/* Case where there was an error counting states */
	if (smb->n_states == -1) {
		#ifdef DEBUG
			printf("Error counting states in state machine buffer.\n");
		#endif
		return NULL;
	}
	state_machine *sm = create_state_machine(smb->n_states);
	if (sm == NULL) {
		#ifdef DEBUG
			printf("Error creating state machine.\n");
		#endif
		return NULL;
	}

	/* Iterate through all bins and states to add them to the state machine */
	for (int i = 0; i < HASH_MAP_N_BINS; i++) {
		#ifdef DEBUG
			printf("Processing bin %d...\n", i);
		#endif
		state_buffer *curr_sb = smb->states[i];
		while (curr_sb != NULL) {
			#ifdef DEBUG
				printf("Processing state %d with %d transitions...\n", curr_sb->id, curr_sb->n_transitions);
			#endif
			transition **transitions = (transition**) malloc(
				curr_sb->n_transitions * sizeof(transition*)
			);
			transition_buffer *curr_tb = curr_sb->transitions;
			int t_index = 0;
			while (curr_tb != NULL) {
				#ifdef DEBUG
					printf("Processing transition to state %d...\n", curr_tb->dest_state);
				#endif
				int n_conds = count_conditions_in_cb(curr_tb->cond_buffers);
				/* Case where there was an error counting the conditions */
				if (n_conds == -1) {
					return NULL;
				}
				char *conditions = (char*) malloc(
					n_conds * sizeof(char)
				);
				cond_buffer *curr_cb = curr_tb->cond_buffers;
				int c_index = 0;
				/* Copy all conditions from the buffer */
				#ifdef DEBUG
					printf("Copying %d conditions for transition to state %d...\n", n_conds, curr_tb->dest_state);
				#endif
				while (curr_cb != NULL) {
					for (int j = 0; j < curr_cb->length; j++) {
						conditions[c_index++] = curr_cb->buffer[j];
					}
					curr_cb = curr_cb->next;
				}
				transitions[t_index++] = create_transition(
					conditions, n_conds, curr_tb->dest_state
				);
				curr_tb = curr_tb->next;
			}
			add_state(sm, curr_sb->id, curr_sb->n_transitions, transitions);
			curr_sb = curr_sb->next;
		}
	}

	return sm;
}