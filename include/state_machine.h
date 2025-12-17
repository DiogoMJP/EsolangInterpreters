#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include <stdlib.h>


/* 
    Struct that associates a set of conditions (characters)
    that, if found, will trigger a transition to another state.
*/
typedef struct transition {
    char *conditions;
    int n_cond;
    int dest_state;
} transition;

/* 
    Struct that represents a state in the state machine.
*/
typedef struct state {
    int id;
    int n_transitions;
    transition **transitions;
} state;

/* 
    Struct that represents the state machine itself.
*/
typedef struct state_machine {
    int curr_state;
    int n_states;
    int added_states;
    state **states;
} state_machine;


/* 
    Allocates memory for a state_machine struct and initializes its values.
    - int n_states: Number of states the state machine will have.
*/
state_machine* create_state_machine(int n_states);

/* 
    Dereferences a pointer to a state_machine and all elements
    it contains.
    - state_machine *sm: Pointer to the state_machine to be deleted.
*/
void delete_state_machine(state_machine* sm);

/*
    Adds a state to the state machine.
    - state_machine *sm: Pointer to the state machine.
    - int n_transitions: Number of transitions the new state will have.
    - transition *transitions: Array of transitions for the new state.
*/
int add_state(state_machine* sm, int n_transitions, transition **transitions);

/*
    Creates a transition with given conditions and destination state.
    - char *conditions: String of characters that trigger the transition.
    - int dest_state: ID of the destination state for this transition.
*/
transition* create_transition(char *conditions, int n_cond, int dest_state);

/*
    Processes an input character and updates the state machine's current state
    based on the defined transitions.
    - state_machine *sm: Pointer to the state machine.
    - char input_char: The input character to process.
    - returns: The new current state after processing the input character.
*/
int state_transition(state_machine *sm, char input_char);

/*
    Returns a pointer to a string containing a representation of a
    state machine. The returned string must be freed by the caller.
    - state_machine *sm: Pointer to the state machine.
*/
char* state_machine_to_string(state_machine *sm);

#endif