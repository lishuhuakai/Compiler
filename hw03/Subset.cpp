#include "SubSet.h"
#include <list>
using namespace std;

static int counter = 0;
static list<Subset*> ls;

static int NextNum() {
	return counter++;
}

Subset* NewSubset(set<int>& s) {
	Subset* _s = new Subset(NextNum(), s);
	ls.push_back(_s);
	return _s;
}

Subset* NewSubset() {
	Subset* _s = new Subset(NextNum());
	ls.push_back(_s);
	return _s;
}

int get_belong_set_label(int stat, list<Subset*>& l) {
	list<Subset*>::iterator it = l.begin();
	while (it != l.end()) {
		set<int> &s = (*it)->sub_state;
		if (s.find(stat) != s.end()) {
			return (*it)->label;
		}
		++it;
	}
	return -1; // not found
}

Subset* get_belong_set_p(int stat, list<Subset*>& l) {
	list<Subset*>::iterator it = l.begin();
	while (it != l.end()) {
		set<int> &s = (*it)->sub_state;
		if (s.find(stat) != s.end()) {
			return *it;
		}
		++it;
	}
	return NULL; // not found
}

void DeleteAllSubset() {
	list<Subset*>::iterator it = ls.begin();
	while (it != ls.end()) {
		delete *it;
		++it;
	}
	ls.clear();
	counter = 0;
}