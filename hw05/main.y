%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

  int yylex();
  void yyerror();
%}

%left '+' '-'
%left '*' '/'

%%

lines: line | line lines;

line: exp '\n';

exp:
n
| exp '+' exp
| exp '-' exp
| exp '*' exp
| exp '/' exp
| '(' exp ')'
;

n: '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | '0';

%%

int isdigit(char c)
{
  if (c >= '0' && c <= '9')
    return 1;
  else
    return 0;
}

int yylex()
{
  char c = getchar();
  //printf("1 get %c \n", c);
  char p = c;
  if (isdigit(c))
  {
    c = getchar();
    //printf("2 get %c \n", c);
    while(isdigit(c))
    {
      p = c;
      c = getchar();
      //printf("3 get %c \n", c);
    }
    ungetc(c, stdin); // 将读入的东西放回去
  }
  return p;
 }

void yyerror(char *s)
{
  fprintf(stdout, "%s\n", s);
  return;
}

int main()
{
  yyparse();
  return 0;
}
