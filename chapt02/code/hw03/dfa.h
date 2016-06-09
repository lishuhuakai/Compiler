#ifndef DFA_H
#define DFA_H

#include "nfa.h"

typedef struct _Dfa_t *Dfa_t;

Dfa_t Dfa_build(Nfa_t nfa);
Dfa_t Dfa_minimize(Dfa_t dfa);
void Dfa_print(Dfa_t dfa);

#endif
