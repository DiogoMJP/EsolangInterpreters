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

int add_state(state_machine *sm, int id, int n_transitions, transition **transitions) {
    if (sm->added_states == sm->n_states) {
        fprintf(stderr, "Cannot add more states than specified at creation.");
        return -1;
    }
    sm->added_states++;

    state *s = (state*) malloc(sizeof(state));
    s->id = id;
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
    t->n_cond = n_cond;
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
        for (int c = 0; c < t->n_cond; c++) {
            if (t->conditions[c] == input_char) {
                sm->curr_state = t->dest_state;
                #ifdef DEBUG
                    printf("Transitioning to state %d on input '%c'.\n", sm->curr_state, input_char);
                #endif
                return sm->curr_state;
            }
        }
    }

    #ifdef DEBUG
        printf("No valid transition found for input '%c' in state %d.\n", input_char, sm->curr_state);
    #endif
    sm->curr_state = -1; // Invalid state
    return -1; // No valid transition found
}

int get_state_machine_string_length(state_machine *sm) {
    if (sm == NULL) {
        fprintf(stderr, "State machine is NULL.\n");
        return -1;
    }
    if (sm->states == NULL) {
        fprintf(stderr, "State machine states are NULL.\n");
        return -1;
    }

    /* Begin counting by taking the string terminator into account */
    int len = 1;
    for (int i = 0; i < sm->n_states; i++) {
        state *s = sm->states[i];
        if (s == NULL) {
            fprintf(stderr, "State %d is NULL.\n", i);
            return -1;
        }
        if (s->transitions == NULL) {
            fprintf(stderr, "State %d transitions are NULL.\n", i);
            return -1;
        }
        /* Begin counting for each line/transition */
        for (int j = 0; j < s->n_transitions; j++) {
            transition *t = s->transitions[j];
            if (t == NULL) {
                fprintf(stderr, "State %d transition %d is NULL.\n", i, j);
                return -1;
            }
            /* Count the digits of the current state ID */
            int id = i;
            if (id == 0) {
                len++;
            } else {
                while (id > 0) {
                    len++;
                    id /= 10;
                };
            }
            /* Count the '-' characters */
            len += 2;
            /* Count the condition characters and commas */
            len += 4 * t->n_cond - 1;
            /* Count the digits of the destination state ID */
            id = t->dest_state;
            if (id == 0) {
                len++;
            } else {
                while (id > 0) {
                    len++;
                    id /= 10;
                };
            }
            /* Count the newline and semi-collon characters */
            len += 2;
        }
    }

    return len;
}

char* state_machine_to_string(state_machine *sm) {
    int len = get_state_machine_string_length(sm);
    if (len == -1) {
        return NULL;
    }
    char *str = (char*) malloc(len * sizeof(char));
    str[0] = '\0';
    
    for (int i = 0; i < sm->n_states; i++) {
        state *s = sm->states[i];
        /* Begin adding each line/transition to the string */
        for (int j = 0; j < s->n_transitions; j++) {
            transition *t = s->transitions[j];
            sprintf(str + strlen(str), "%d-", i);
            for (int c = 0; c < t->n_cond; c++) {
                sprintf(str + strlen(str), "%03d,", t->conditions[c]);
            }
            sprintf(str + strlen(str) - 1, "-%d;\n", t->dest_state);
        }
    }

    return str;
}