#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semant.h"

#define TODO()\
  do{\
  printf ("TODO: add your code at file: \"%s\", line: %d\n", __FILE__, __LINE__);\
}while(0)

//////////////////////////////////////
// the symbol table
List_t table = 0;

// lookup an "id" in the symbol table.
// return "type" on success; and -1 for failure
Type_t Table_lookup (char *id)
{
  List_t p = table;
  while (p){
    Dec_t d = (Dec_t)p->data;
    if (strcmp(d->id, id)==0)
      return d->type;
    p = p->next;
  }
  return -1;
}

void Table_insert (Dec_t dec)
{
  
  if (Table_lookup (dec->id) != -1){
    fprintf (stdout, "Error: the variable "
	     "\"%s\" has been declared!\n", dec->id);
    exit (0);
  }
  table = List_new (dec, table);
  return;
}

//////////////////////////////////////////
// dec
void check_dec(Dec_t d)
{
  Table_insert (d);
}

void check_decs(List_t decs)
{
  while (decs){
    Dec_t d = (Dec_t)decs->data;
    check_dec(d);
    decs = decs->next;
  }
  return;
}

////////////////////////////////////////
// exp

// Your job:
Type_t check_exp (Exp_t exp)
{
  Type_t t1, t2;
  switch (exp->kind) {
    case EXP_INT:
      return TYPE_INT;
    case EXP_TRUE:
    case EXP_FALSE:
      return TYPE_BOOL;
	case EXP_AND: {
		Exp_And e = (Exp_And)exp;
		t1 = check_exp(e->left);
		t2 = check_exp(e->right);
		if (t1 != TYPE_BOOL || t2 != TYPE_BOOL)
		{
			fprintf(stdout, "Error: variable is not a bool!\n");
			exit(0);
		}
		return TYPE_BOOL;
	}
	case EXP_OR: {
		Exp_Or e = (Exp_Or)exp;
		t1 = check_exp(e->left);
		t2 = check_exp(e->right);
		if (t1 != TYPE_BOOL || t2 != TYPE_BOOL)
		{
			fprintf(stdout, "Error: variable is not a bool!\n");
			exit(0);
		}
		return TYPE_BOOL;
	}
	case EXP_ADD: {
		Exp_Add e = (Exp_Add)exp;
		t1 = check_exp(e->left);
		t2 = check_exp(e->right);
		if (t1 != TYPE_INT || t2 != TYPE_INT)
		{
			fprintf(stderr, "Error: variable is not a int.\n");
			exit(0);
		}
		return TYPE_INT;
	}
	case EXP_SUB: {
		Exp_Sub e = (Exp_Sub)exp;
		t1 = check_exp(e->left);
		t2 = check_exp(e->right);
		if (t1 != TYPE_INT || t2 != TYPE_INT)
		{
			fprintf(stderr, "Error: variable is not a int.\n");
			exit(0);
		}
		return TYPE_INT;
	}
	case EXP_TIMES: {
		Exp_Times e = (Exp_Times)exp;
		t1 = check_exp(e->left);
		t2 = check_exp(e->right);
		if (t1 != TYPE_INT || t2 != TYPE_INT)
		{
			fprintf(stderr, "Error: variable is not a int.\n");
			exit(0);
		}
		return TYPE_INT;
	}
    case EXP_DIVIDE: {
      Exp_Divide e = (Exp_Divide)exp;
      t1 = check_exp(e->left);
      t2 = check_exp(e->right);
      if (t1 != TYPE_INT || t2 != TYPE_INT)
      {
		fprintf (stderr, "Error: variable is not a int.\n");
		exit(0);
      }
      return TYPE_INT;
    }
	case EXP_ID: {
		Exp_Id e = (Exp_Id)exp;
		return Table_lookup(e->id);
	}
    default:{
		fprintf(stderr, "not support yet!\n");
		exit(0);
    }
	}	
}

////////////////////////////////////////
// stm

// Your job:
void check_stm (Stm_t stm)
{
  switch (stm->kind) {
    case STM_ASSIGN:{
		Stm_Assign s = (Stm_Assign)stm;
		Type_t t1 = Table_lookup(s->id);
		Type_t t2 = check_exp(s->exp);
		if (t1 != t2) exit(0);
		break;
	}
	case STM_PRINTB: {
		Stm_Printb s = (Stm_Printb)stm;
		Type_t t1 = check_exp(s->exp);
		if (t1 == -1 && s->exp->kind == EXP_ID)
		{
			fprintf(stdout, "%s is not defined yet!\n", ((Exp_Id)s->exp)->id);
			exit(0);
		}
		else if (t1 != TYPE_BOOL)
		{
			fprintf(stdout, "Printb's argument is not a bool!\n");
			exit(0);
		}
		break;
			
	}
	case STM_PRINTI: {
		Stm_Printi s = (Stm_Printi)stm;
		Type_t t1 = check_exp(s->exp);
		if (t1 == -1 && s->exp->kind == EXP_ID)
		{
			fprintf(stdout, "%s is not defined yet!\n", ((Exp_Id)s->exp)->id);
			exit(0);
		}
		else if (t1 != TYPE_INT)
		{
			fprintf(stdout, "Printi's argument is not a int!\n");
			exit(0);
		}
		break;
	}
					
	}
}

void check_stms(List_t stms)
{
  while (stms){
    Stm_t s = (Stm_t)stms->data;
    check_stm(s);
    stms = stms->next;
  }
  return;
}


void Semant_check(Prog_t prog)
{
  // create table
  check_decs(prog->decs);
  // check stm
  check_stms(prog->stms);
  return;
}
