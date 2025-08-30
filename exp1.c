#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ms 100
#define mds 100   


int nfa[ms][2][ms];       
int nfa_count[ms][2];             
int n_states;                             


int dfa[mds][2];               
int dfa_states[mds][ms]; 
int dfa_state_size[mds];       
int dfa_count = 0;                        

int find_dfa_state(int *states, int size) {
    for (int i = 0; i < dfa_count; i++) {
        if (dfa_state_size[i] == size) {
            int match = 1;
            for (int j = 0; j < size; j++) {
                if (dfa_states[i][j] != states[j]) {
                    match = 0;
                    break;
                }
            }
            if (match) return i;
        }
    }
    return -1;
}


int add_dfa_state(int *states, int size) {
    

    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
            if (states[i] > states[j]) {
                int t = states[i]; states[i] = states[j]; states[j] = t;
            }

    int idx = find_dfa_state(states, size);
    if (idx != -1) return idx;  

    
    memcpy(dfa_states[dfa_count], states, size * sizeof(int));
    dfa_state_size[dfa_count] = size;
    return dfa_count++; 
}

int main() {
    printf("Enter number of NFA states: ");
    scanf("%d", &n_states);


    for (int i = 0; i < n_states; i++) {
        printf("\nFor NFA state %d:\n", i);

        printf("  Number of states reachable on 0: ");
        scanf("%d", &nfa_count[i][0]);
        if (nfa_count[i][0] > 0) {
            printf("  Enter those state numbers (space-separated): ");
            for (int k = 0; k < nfa_count[i][0]; k++) {
                scanf("%d", &nfa[i][0][k]);
            }
        }

        printf("  Number of states reachable on 1: ");
        scanf("%d", &nfa_count[i][1]);
        if (nfa_count[i][1] > 0) {
            printf("  Enter those state numbers (space-separated): ");
            for (int k = 0; k < nfa_count[i][1]; k++) {
                scanf("%d", &nfa[i][1][k]);
            }
        }
    }

    int start_states[1] = {0};    
    int start = add_dfa_state(start_states, 1);   



    for (int i = 0; i < dfa_count; i++) {
        for (int sym = 0; sym < 2; sym++) {
            int new_states[ms], new_size = 0;


            for (int k = 0; k < dfa_state_size[i]; k++) {
                int nfa_state = dfa_states[i][k];

                for (int t = 0; t < nfa_count[nfa_state][sym]; t++) {
                    int ns = nfa[nfa_state][sym][t];

                    int already = 0;
                    for (int z = 0; z < new_size; z++)
                        if (new_states[z] == ns) already = 1;
                    if (!already) new_states[new_size++] = ns;
                }
            }


            if (new_size == 0) {
                dfa[i][sym] = -1;
            } else {
                dfa[i][sym] = add_dfa_state(new_states, new_size);
            }
        }
    }


    printf("\nDFA Transition Table:\n");
    printf("DFA_State\tOn_0\tOn_1\t(NFA States subset)\n");
    for (int i = 0; i < dfa_count; i++) {
        printf("%d\t\t", i);
        if (dfa[i][0] == -1) printf("-\t");
        else printf("%d\t", dfa[i][0]);
        if (dfa[i][1] == -1) printf("-\t");
        else printf("%d\t", dfa[i][1]);

        printf("{");
        for (int k = 0; k < dfa_state_size[i]; k++) {
            printf("%d", dfa_states[i][k]);
            if (k < dfa_state_size[i] - 1) printf(",");
        }
        printf("}\n");
    }

    return 0;
}