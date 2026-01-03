#ifndef __STATE_MACHINE_BUFFER_H__
#define __STATE_MACHINE_BUFFER_H__

#include <stdio.h>

#include "state_machine.h"

/* Number of chars for each node in the cond_buffer linked list */
#define COND_N_CHARS 10
/* Number of bins in the hash map for the state_machine_buffer */
#define HASH_MAP_N_BINS 8


/*
    Struct that represents a buffer for conditions.
    It consists of a linked list of fixed-size character arrays.
    The number of characters per array is defined by COND_N_CHARS.
*/
typedef struct cond_buffer {
    char buffer[COND_N_CHARS];
    int length;
    struct cond_buffer *next;
} cond_buffer;

/*
    Struct that represents a buffer for transitions.
    It consists of a linked list of transition buffers,
    each containing a pointer to a condition buffer linked list
    and a destination state ID.
*/
typedef struct transition_buffer {
    cond_buffer *cond_buffers;
    int n_conds;
    int dest_state;
    struct transition_buffer *next;
} transition_buffer;

/*
    Struct that represents a buffer for states.
    It consists of a linked list of state buffers,
    each containing a state ID, a pointer to a transition buffer linked list,
    and the number of transitions.
*/
typedef struct state_buffer {
    int id;
    transition_buffer *transitions;
    int n_transitions;
    struct state_buffer *next;
} state_buffer;

/*
    Struct that represents a buffer for the entire state machine.
    It consists of a hash map (array of linked lists) of state buffers
    and the total number of states. The number of bins in the hash map
    is defined by HASH_MAP_N_BINS.
*/
typedef struct state_machine_buffer {
	state_buffer *states[HASH_MAP_N_BINS];
	int n_states;
} state_machine_buffer;


/*
    Adds a character to the condition buffer linked list. If the buffer
    does not exist, it is created.
    - cb: Pointer to the condition buffer.
    - c: Character to be added.
    - returns: Pointer to the updated condition buffer.
*/
cond_buffer* add_to_cond_buffer(cond_buffer *cb, char c);

/*
    Adds a transition to the transition buffer llinked list. If the buffer
    does not exist, it is created.
    - tb: Pointer to the transition buffer.
    - cb: Pointer to the condition buffer for the transition.
    - dest_id: ID of the destination state.
    - returns: Pointer to the updated transition buffer.
*/
transition_buffer* add_to_transition_buffer(
	transition_buffer *tb, cond_buffer *cb, int dest_id
);

/*
    Adds a transition to the state buffer. If the buffer does not exist,
    it is created. If the transition does not exist in the buffer,
    it is created as well.
    - sb: Pointer to the state buffer.
    - id: ID of the state.
    - cb: Pointer to the condition buffer for the transition.
    - dest_id: ID of the destination state.
    - returns: Pointer to the updated state buffer.
*/
state_buffer* add_to_state_buffer(
	state_buffer *sb, int id, cond_buffer *cb, int dest_id
);

/*
    Adds a transition to the state machine buffer. If the buffer
    does not exist, it is created. If the state does not exist in the buffer,
    it is created as well.
    - smb: Pointer to the state machine buffer.
    - id: ID of the source state.
    - cb: Pointer to the condition buffer for the transition.
    - dest_id: ID of the destination state.
    - returns: Pointer to the updated state machine buffer.
*/
state_machine_buffer* add_to_state_machine_buffer(
	state_machine_buffer *smb, int id, cond_buffer *cb, int dest_id
);

/* 
	Creates and returns a state machine from the given state machine buffer.
    - returns: Pointer to the created state machine configured. If there is an error,
    this value is NULL.
*/
state_machine *create_state_machine_from_buffer(state_machine_buffer *smb);

#endif