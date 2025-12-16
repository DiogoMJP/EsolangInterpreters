#include <stdio.h>
#include <string.h>

#include "state_machine.h"


state_machine* create_state_machine(int n_states) {
    state_machine *sm = (state_machine*) malloc(sizeof(state_machine));
    sm->curr_state = 0;
    sm->n_states = n_states;
    sm->added_states = 0;
    sm->states = (state**) malloc(n_states * sizeof(state*));
    for (int i = 0; i < n_states; i++) {
        sm->states[i] = NULL;
    }

    return sm;
}

void delete_state_machine(state_machine *sm) {
    if (sm == NULL) return;
    if (sm->states == NULL) {
        free(sm);
        return;
    }

    for (int i = 0; i < sm->n_states; i++) {
        state *s = sm->states[i];
        if (s == NULL) continue;

        if (s->transitions != NULL) {
            for (int j = 0; j < s->n_transitions; j++) {
                if (s->transitions[j] == NULL) continue;
                if (s->transitions[j]->conditions != NULL) {
                    free(s->transitions[j]->conditions);
                }
                free(s->transitions[j]);
            }
            free(s->transitions);
        }
        free(s);
    }

    free(sm->states);
    free(sm);
}

int add_state(state_machine *sm, int n_transitions, transition **transitions) {
    if (sm->added_states == sm->n_states) {
        fprintf(stderr, "Cannot add more states than specified at creation.");
        return -1;
    }

    state *s = (state*) malloc(sizeof(state));
    s->id = sm->added_states++;
    s->n_transitions = n_transitions;
    s->transitions = transitions;
    sm->states[s->id] = s;
    
    return s->id;
}

transition* create_transition(char *conditions, int n_cond, int dest_state) {
    transition *t = (transition*) malloc(sizeof(transition));

    t->conditions = (char*) malloc(n_cond * sizeof(char));
    for (int i = 0; i < n_cond; i++) {
        t->conditions[i] = conditions[i];
    }
    t->dest_state = dest_state;

    return t;
}

int state_transition(state_machine *sm, char input_char) {
    if (sm == NULL) {
        fprintf(stderr, "State machine is NULL.\n");
        return -1;
    }
    if (sm->curr_state == -1) {
        fprintf(stderr, "Current state is invalid.\n");
        return -1;
    }

    state *curr_state = sm->states[sm->curr_state];
    for (int i = 0; i < curr_state->n_transitions; i++) {
        transition *t = curr_state->transitions[i];
        if (strchr(t->conditions, input_char) != NULL) {
            sm->curr_state = t->dest_state;
            #ifdef DEBUG
                printf("Transitioning to state %d on input '%c'.\n", sm->curr_state, input_char);
            #endif
            return sm->curr_state;
        }
    }

    #ifdef DEBUG
        printf("No valid transition found for input '%c' in state %d.\n", input_char, sm->curr_state);
    #endif
    sm->curr_state = -1; // Invalid state
    return -1; // No valid transition found
}