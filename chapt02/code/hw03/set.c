#include <stdlib.h>
#include <stdio.h>
#include "set.h"
#include "new.h"

static int const MIN_CAPACITY = 16;

static void expand(Set_t s)
{
  s->_capacity *= 2;
  void ** p = malloc (s->_capacity * sizeof(void *));
  int i = 0;
  for (; i < s->_size; ++i)
  {
    p[i] = s->_elem[i];
  }
  free (s->_elem);
  s->_elem = p;
}

static void Set_init(Set_t s)
{
  s->_capacity = MIN_CAPACITY;
  s->_size = 0;
  s->_elem = malloc (s->_capacity * sizeof(void *));
}

Set_t Set_new()
{
  Set_t p;
  NEW(p);
  Set_init(p);
  return p;
}

int Set_findInt(Set_t s, int e, int * pos)
{
  return Set_findPoint(s, (void *)e, pos);
}

int Set_findPoint(Set_t s, void * e, int * pos)
{
  int i = 0;
  if (NULL == pos) { pos = &i; }

  *pos = 0;
  for (; *pos < s->_size; ++(*pos))
  {
    if (s->_elem[*pos] == e) 
    {
      return 1; 
    }
    else if (s->_elem[*pos] > e)
    {
      return 0;
    }
  }
  return 0;
}

int Set_insertPoint(Set_t s, void * e)
{
  if (s->_size >= s->_capacity) { expand(s); }
  int retVal = 0;
  if (0 == Set_findPoint(s, e, &retVal))
  {
    int j = s->_size;
    for (; j > retVal; --j)
    {
      s->_elem[j] = s->_elem[j-1];
    }
    ++s->_size;
    s->_elem[retVal] = e;
  }
  return retVal;
}

int Set_insertInt(Set_t s, int elem)
{
  return Set_insertPoint(s, (void *)elem);
}

void Set_free(Set_t s)
{
  s->_capacity = 0;
  s->_size = 0;
  if (NULL != s->_elem)
  {
    free (s->_elem);
  }
  free(s);
}

int Set_isEqual(Set_t lhs, Set_t rhs)
{
  if (lhs->_size != rhs->_size) { return 0; }
  int i = 0;
  while (i < lhs->_size)
  {
    if (lhs->_elem[i] != rhs->_elem[i]) { return 0; }
    ++i;
  }
  return 1;
}

void Set_printInts(Set_t s)
{
  int i = 0;
  for (; i < s->_size; ++i)
  {
    printf("(%d)", (int)(s->_elem[i]));
  }
  printf("\n");
}


