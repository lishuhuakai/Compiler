#ifndef SET_H
#define SET_H

typedef struct _Set_t *Set_t;
struct _Set_t
{
  int _size;
  int _capacity;
  void ** _elem;
};

Set_t Set_new();
void Set_free(Set_t s);
int Set_findInt(Set_t s, int e, int * pos);
int Set_findPoint(Set_t s, void * e, int * pos);
int Set_insertInt(Set_t s, int elem);
int Set_insertPoint(Set_t s, void * e);
int Set_isEqual(Set_t lhs, Set_t rhs);
void Set_printInts(Set_t s);

#endif
