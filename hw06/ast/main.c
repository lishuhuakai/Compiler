#include "ast.h"
#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

extern Exp_t tree;
extern List_t all;
void yyparse ();

//////////////////////////////////////////////////
// program entry
int main()
{
  yyparse();
  printf("Compile starting\n");
  printf("the expression is:\n");
  Exp_print(tree);
  printf("\n");
  // compile this tree to Stack machine instructions
  compile(tree);
  
  // print out the generated Stack instructons:
  List_reverse_print(all);
  
  printf("\nCompile finished\n");
  return 0;
}
