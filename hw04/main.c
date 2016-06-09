#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void parse_F();
void parse_T();
void parse_E();
void error (char *want, char got);

int i;
char *str = 0;

void error (char *want, char got)
{
  fprintf (stdout, "Compling this expression:\n%s\n", str);
  int j = i;
  while (j--)
    fprintf (stdout, " ");
  fprintf (stdout, "^\n");
  fprintf (stdout, "Syntax error at position: %d\n"
           "\texpecting: %s\n"
           "\tbut got  : %c\n",
           i, want, got);
  exit (0);
  return;
}

void parse_F()
{
  char c = str[i];
  if (isdigit(c)){ // F -> num
    i++;
    return;
  }
  if (c=='('){ // F -> (E)
    i++;
    parse_E();
    c = str[i];
    if (c==')'){
      i++;
      return;
    }
    error ("\')\'", c);
    return;
  }
  error ("\'0-9\' or \'(\'", c);
  return;
}


void parse_T()
{
  parse_F(); // T -> F * F * F...
  char c = str[i];
  while (c=='*' || c == '/'){
    i++;
    parse_F();
    c = str[i];
  }
  return;
}

void parse_E()
{
  parse_T(); // E -> T
  char c = str[i]; // get a char
  while (c=='+' || c=='-'){
    i++;
    parse_T(); // E -> T + T
    c = str[i];
  }
  return;
}

void parse (char *e)
{
  str = e;
  i = 0;
  parse_E(); // start symbol is E
  if (str[i]=='\0')
    return;
  error ("\'+\' or '\\0\'", str[i]);
  return;
}
///////////////////////////////////////////////
//     E -> E + T
//     | E - T
//	   | T
//	   
//     T->T * F
//	   | T / F
//	   | F
//	   
//     F->num
//	   | (E)
//

///////////////////////////////////////////////
// Your job:
// Add some code into the function parse_E() and
// parse_T to parse "-" and "/" correctly.
// When you finish your task, NO error message
// should be generated.
// Enjoy! :-P
int main (char argc, char **argv)
{
  // There are the following rules on an expression:
  //   1. Every expression is represented as a string;
  //   2. integers are non-negative;
  //   3. integers are between 0-9.
  char *e;

  e = "(2)";
  parse(e);

 // e = "(3+4*5))";
 // parse(e);

  e = "(8-2)*3";
  parse(e);

  e = "(8-2)/3";
  parse(e);
  
  system("pause");
  return 0;
}
