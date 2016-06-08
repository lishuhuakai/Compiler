#pragma once
#include <list>
#include <set>
using namespace std;

typedef struct _myset {
	int label;
	set<int> sub_state;
	_myset(int lb, set<int>& s): label(lb), sub_state(s) {}
	_myset(int lb): label(lb) {}
}Subset;

Subset* NewSubset(set<int>& s);
Subset* NewSubset();
void DeleteAllSubset();
int get_belong_set_label(int stat, list<Subset*>& l);
Subset* get_belong_set_p(int stat, list<Subset*>& l);

