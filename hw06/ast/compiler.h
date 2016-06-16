#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "ast.h"
//////////////////////////////////////////////
// Data structures for the Stack language.
enum Stack_Kind_t {
  STACK_ADD,
  STACK_PUSH,
  STACK_SUB,
  STACK_TIMES,
  STACK_DIV};

typedef struct Stack_t *Stack_t;
struct Stack_t
{
  enum Stack_Kind_t kind;
};

typedef struct Stack_Add *Stack_Add;
struct Stack_Add
{
  enum Stack_Kind_t kind;
};

typedef struct Stack_Sub *Stack_Sub;
struct Stack_Sub
{
  enum Stack_Kind_t kind;
};

typedef struct Stack_Times *Stack_Times;
struct Stack_Times
{
  enum Stack_Kind_t kind;
};

typedef struct Stack_Div *Stack_Div;
struct Stack_Div
{
  enum Stack_Kind_t kind;
};

typedef struct Stack_Push *Stack_Push;
struct Stack_Push
{
	enum Stack_Kind_t kind;
	int i;
};

/// instruction list
typedef struct List_t *List_t;
struct List_t
{
	struct Stack_t *instr;
	struct List_t *next;
};


// help functions
void Exp_print(struct Exp_t *exp);
void List_reverse_print(struct List_t *list);
void Compiler(struct Exp_t *exp);
#endif // _COMPILER_H_
