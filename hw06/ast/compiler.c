#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "compiler.h"

///////////////////////////////////////////////
// "constructors"
struct Stack_t *Stack_Add_new()
{
  struct Stack_Add *p = (Stack_Add)malloc(sizeof(*p));
	p->kind = STACK_ADD;
	return (struct Stack_t *)p;
}

struct Stack_t *Stack_Sub_new()
{
	struct Stack_Sub *p = malloc(sizeof(*p));
	p->kind = STACK_SUB;
	return (struct Stack_t *)p;
}

struct Stack_t *Stack_Times_new()
{
	struct Stack_Times *p = malloc(sizeof(*p));
	p->kind = STACK_TIMES;
	return (struct Stack_t *)p;
}

struct Stack_t *Stack_Div_new()
{
	struct Stack_Div *p = malloc(sizeof(*p));
	p->kind = STACK_DIV;
	return (struct Stack_t *)p;
}


struct Stack_t *Stack_Push_new(int i) // 输入一个int类型，然后就要压入栈，是吧！
{
	struct Stack_Push *p = malloc(sizeof(*p));
	p->kind = STACK_PUSH;
	p->i = i;
	return (struct Stack_t *)p;
}


struct List_t *List_new(struct Stack_t *instr, struct List_t *next)
{
	struct List_t *p = malloc(sizeof(*p));
	p->instr = instr;
	p->next = next;
	return p;
}

// "printer"
void List_reverse_print(struct List_t *list)
{
	// add my code here
	struct List_t *tlist = list;
	while (tlist != NULL)
	{
		switch (tlist->instr->kind) {
		case STACK_ADD:
		  printf("Add\n");
		  break;
		case STACK_PUSH:{
		  struct Stack_Push *p = (struct Stack_Push *)tlist->instr;
		  printf("Push %d\n", p->i);
		  break;
		}
		case STACK_SUB:
		  printf("Sub\n");
		  break;
		case STACK_TIMES:
		  printf("Times\n");
		  break;
		case STACK_DIV:
		  printf("Div\n");
		  break;
		default:
			break;
		}
		tlist = tlist->next;
	}
}

//////////////////////////////////////////////////
// a compiler from Sum to Stack
struct List_t *all = 0;

void emit(struct Stack_t *instr) // emit函数是干什么的？
{
	all = List_new(instr, all);
}

void compile(struct Exp_t *exp)
{
	switch (exp->kind) {
	case EXP_INT: {
		struct Exp_Int *p = (struct Exp_Int *)exp;
		emit(Stack_Push_new(p->n));
		break;
	}
	
	case EXP_ADD: {
	  struct Exp_Add *p = (struct Exp_Add *)exp;
	  compile(p->left);
	  compile(p->right);
	  emit(Stack_Add_new());
	  break;
	}

	case EXP_SUB: {
	  struct Exp_Sub *p = (struct Exp_Sub *)exp;
	  compile(p->left);
	  compile(p->right);
	  emit(Stack_Sub_new());
	  break;
	}


	case EXP_TIMES: {
	  struct Exp_Times *p = (struct Exp_Times *)exp;
	  compile(p->left);
	  compile(p->right);
	  emit(Stack_Times_new());
	  break;
	}
	  
	case EXP_DIV: {
	  struct Exp_Div *p = (struct Exp_Div *)exp;
	  compile(p->left);
	  compile(p->right);
	  emit(Stack_Div_new());
	  break;
	}
	
	default:
		break;
	}
}

