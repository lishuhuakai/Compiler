#include <stdio.h>
#include "re.h"
#include "dfa.h"

int main (int argc, char **argv)
{
  // First, we make a data structure for this
  // regular expression:
  //    a b
  Re_t e = Re_Concat_new (Re_Char_new ('a')
                          , Re_Char_new ('b'));
  // and to print e1 out to make sure the
  // data structure is right: (note that
  // ASCII is now integers)
  printf ("\nthe regular expression is:\n");
  Re_print (e);

  // convert regular expressions to NFA:
  Nfa_t nfa = Re_thompson (e);
  // print the NFA out:
  printf ("\nthe NFA is:\n");
  Nfa_print (nfa);

  // convert NFA to DFA:
  Dfa_t dfa = Dfa_build(nfa);
  // print the DFA:
  printf ("\nthe fisrt DFA is:\n");
  Dfa_print(dfa);

  Dfa_t mini_dfa = Dfa_minimize(dfa);
  printf ("\nthe fisrt minimized DFA is:\n");
  Dfa_print(mini_dfa);

  // a second regular expression:
  //    a (b|c)*
  e = Re_Concat_new (Re_Char_new ('a')
                     , Re_Closure_new (Re_Alt_new(Re_Char_new ('b')
                                                  , Re_Char_new ('c'))));
  // print it
  printf ("\nthe second regular expression is:\n");
  Re_print (e);
  // convert it to NFA:
  nfa = Re_thompson (e);
  // print the NFA:
  printf ("\nthe second NFA is:\n");
  Nfa_print (nfa);

  // convert NFA to DFA:
  dfa = Dfa_build(nfa);
  // print the DFA:
  printf ("\nthe second DFA is:\n");
  Dfa_print(dfa);

  mini_dfa = Dfa_minimize(dfa);
  printf ("\nthe second minimized DFA is:\n");
  Dfa_print(mini_dfa);

  
  // the third regular expression:
  //    (a b)* (a*|b*) (b a)*
  Re_t e1 = Re_Closure_new(Re_Concat_new(Re_Char_new('a'), 
                                         Re_Char_new('b')));
  Re_t e2 = Re_Alt_new(Re_Closure_new(Re_Char_new('a')), 
                       Re_Closure_new(Re_Char_new('b')));
  Re_t e3 = Re_Closure_new(Re_Concat_new(Re_Char_new('b'), 
                                         Re_Char_new('a')));
  e = Re_Concat_new(Re_Concat_new(e1, e2), e3);
  printf ("\nthe third regular expression is:\n");
  Re_print (e);

  nfa = Re_thompson (e);
  printf ("\nthe third NFA is:\n");
  Nfa_print (nfa);

  dfa = Dfa_build(nfa);
  printf ("\nthe third DFA is:\n");
  Dfa_print(dfa);

  mini_dfa = Dfa_minimize(dfa);
  printf ("\nthe third minimized DFA is:\n");
  Dfa_print(mini_dfa);

  // the fourth regular expression:
  //    (a|b) ((c|d)*)
  e = Re_Concat_new(Re_Alt_new(Re_Char_new('a'), 
                               Re_Char_new('b')), 
                    Re_Closure_new(Re_Alt_new(Re_Char_new('c'), 
                                              Re_Char_new('d'))));
  printf ("\nthe fourth regular expression is:\n");
  Re_print (e);

  nfa = Re_thompson (e);
  printf ("\nthe fourth NFA is:\n");
  Nfa_print (nfa);

  dfa = Dfa_build(nfa);
  printf ("\nthe fourth DFA is:\n");
  Dfa_print(dfa);

  mini_dfa = Dfa_minimize(dfa);
  printf ("\nthe fourth minimized DFA is:\n");
  Dfa_print(mini_dfa);

  return 0;
}
