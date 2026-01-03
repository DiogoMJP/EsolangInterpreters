#include "state_machine_buffer.h"


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

/*
    Counts the total number of condition characters in the
    given condition buffer linked list. If the buffer is NULL,
    returns -1.
    - cb: Pointer to the condition buffer.
    - returns: Total number of condition characters in the buffer.
*/
int count_conditions_in_cb(cond_buffer *cb) {
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

/*
    Counts the total number of states in the given condition
    buffer linked list. If the buffer is NULL, returns -1.
    - smb: Pointer to the state machine buffer.
    - returns: Total number of states in the buffer.
*/
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
				int n_conds = count_conditions_in_cb(curr_transition->cond_buffers);
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