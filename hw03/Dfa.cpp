#include "Dfa.h"
#include "Subset.h"
#include <set>
#include <iostream>
#include <map>
#include <queue>
#include <list>
#include <utility>
using namespace std;

//#define _DEMO_
// now we must implement minimum subset algorithm

//////////////////////////////////////////////
// global variables
int table[MAXV][MAXV] = { 0 };
static set<int> accState; // all the accept state
static set<int> nAccState; // all the state excerpt accept state
static set<char> allChars; // contian all the characters

static int counter = 0;

#ifdef _HOP_CRAFT_
int minTable[MAXV][MAXV] = { 0 }; // for Hopcraft 
static set<int> newAccStat;
#endif // _HOP_CRAFT_

//////////////////////////////////////////
// helper functions
static int NextNum() {
	return counter++;
}

static void PrevNum() {
	counter--;
}

static void InitTable() {
	for (int i = 0; i < MAXV; ++i) {
		for (int j = 0; j < MAXV; ++j) {
			table[i][j] = -1;
#ifdef _HOP_CRAFT_
			minTable[i][j] = -1;
#endif // _HOP_CRAFT_
		}
	}
}

static bool IsCharRight(char c) {
	if ((c != '(') && (c != ')') && (c != '|') && (c != '*')) {
		return true;
	}
	return false;
}

void Init(char *str) {
	while (*str != '\0') {
		if (IsCharRight(*str)) {
			allChars.insert(*str);
		}
		++str;
	}
	InitTable();
}

/////////////////////////////
// constructor 
list<QNode*> g_lq;
QNode *NewQNode(int label, set<Node*>& s) {
	QNode* qnd = new QNode;
	qnd->label = label;
	qnd->subStates = s;
	g_lq.push_back(qnd);
	return qnd;
}

void PrintClosure(set<Node*>& closure) {
	set<Node*>::iterator it = closure.begin();
	if (it != closure.end()) {
		cout << (*it)->label; ++it;
	}
	while (it != closure.end()) {
		cout << '-' << (*it)->label;
		++it;
	}
	cout << endl;
}

/////////////////////////////////////////
// minimum subset algorithm

// global viriables
set<QNode*> Q;
queue<QNode*> workList;
set<Node*> closure;
set<Node*> deltaClosure;

void EpsClosure(Node* nd) { // dfs
	// from this node, visit all the nodes
	closure.insert(nd); 
	Edge *arc = nd->firstEdge;
	while (arc != NULL) {
		if (arc->trans_letter == EPS) {
			if (!closure.insert(arc->to).second) { // this elements already exist!
				return;
			}
			EpsClosure(arc->to);
		}
		arc = arc->nextEdge;
	}
}

void EClosure(set<Node*>& deltaClosure) {
	set<Node*>::iterator it = deltaClosure.begin();
	closure.clear();
	while (it != deltaClosure.end()) {
		EpsClosure(*it);
		++it;
	}
}

void Delta(set<Node*>& closure, char c) {
	deltaClosure.clear();
	set<Node*>::iterator it = closure.begin();
	while (it != closure.end()) {
		Edge* eg = (*it)->firstEdge;
		while (eg != NULL) {
			if (eg->trans_letter == c) {
				deltaClosure.insert(eg->to);
			}
			eg = eg->nextEdge;
		}
		++it;
	}
}

pair<int, bool> Contain(set<QNode*>& Q, QNode& nd) {
	pair<int, bool> rs;
	set<QNode *>::iterator it = Q.begin();
	set<Node*>& _s2 = nd.subStates;
	while (it != Q.end()) {
		set<Node*>& _s1 = (*it)->subStates;
		if (_s1.size() != _s2.size()) {
			++it; continue;
		}
		set<Node*>::iterator _i1 = _s1.begin();
		set<Node*>::iterator _i2 = _s2.begin();
		while (_i1 != _s1.end()) {
			if (*_i1 != *_i2) goto end;
			++_i1; ++_i2;
		}
		rs.first = (*it)->label;
		rs.second = true;
		return rs;
	end:
		++it;
	}
	rs.second = false;
	return rs;
}

bool ContainAccState(QNode& qnd, Node *nd) {
	set<Node*>& _s1 = qnd.subStates;
	set<Node*>::iterator it = _s1.begin();
	while (it != _s1.end()) {
		if (*it == nd) return true;
		++it;
	}
	return false;
}

void PrintAccState() {
	cout << "Accept states are as follows :" << endl;
	set<int>::iterator it = accState.begin();
	cout << *it;
	++it;
	while (it != accState.end()) {
		cout << " " << *it;
		++it;
	}
	cout << endl;
}

void BuildDfaTable(Nfa *g) {
	accState.clear();
	nAccState.clear();
	Node *nd = g->start;
	EpsClosure(nd);
#ifdef _DEMO_
	PrintClosure(closure);
#endif // _DEMO_

	QNode *qnd = NewQNode(NextNum(), closure);
	Q.insert(qnd); // insert a QNode point
	nAccState.insert(qnd->label); // start node cann't be a accept node
	workList.push(qnd);
	while (!workList.empty()) {
		QNode *nd = workList.front();
		workList.pop();
		set<char>::iterator it = allChars.begin();
		while (it != allChars.end()) {
			Delta(nd->subStates, *it);
#ifdef _DEMO_
			PrintClosure(deltaClosure);
#endif // _DEMO_

			EClosure(deltaClosure);
#ifdef _DEMO_
			PrintClosure(closure);
#endif // _DEMO_
			
			if (closure.size() == 0) {
				++it; 
				continue;
			}

			qnd = NewQNode(NextNum(), closure);
			 // 填入新的表单
			pair<int, bool> rs = Contain(Q, *qnd);
			if (!rs.second) { // 如果没有包含了这个节点，那么
				workList.push(qnd);
				Q.insert(qnd);
				table[nd->label][*it] = qnd->label;
				if (ContainAccState(*qnd, g->accept)) {
					accState.insert(qnd->label);
				}
				else {
					nAccState.insert(qnd->label); 
				}
#ifdef _DEMO_
				cout << "_________________________________________" << endl;
				cout << "q" << nd->label << "--" << *it << "->q" << qnd->label << endl;
#endif // _DEMO_

			}
			else
			{
				PrevNum();
				table[nd->label][*it] = rs.first;
#ifdef _DEMO_
				cout << "_________________________________________" << endl;
				cout << "q" << nd->label << "--" << *it << "->q" << rs.first << endl;
#endif // _DEMO_
			}
			++it;
		}
	}
}

#ifdef _HOP_CRAFT_
////////////////////////////////////////////
// Hopcraft algorithm

// global variable
list<Subset*> lt;
list<Subset*> lp;

//// helper function
static void InsertElement(map<int, Subset*>& mp, list<Subset*>& s) {
	map<int, Subset*>::iterator _i1 = mp.begin();
	while (_i1 != mp.end()) {
		s.push_back((*_i1).second);
		++_i1;
	}
}

void SplitAppendFind(Subset* se, list<Subset*>& lt, list<Subset*>& lp) { // add something to lt, find someting in lp
	map<int, Subset*> mp;
	set<char>::iterator _i1 = allChars.begin();
	while (_i1 != allChars.end()) {
		set<int>::iterator _i2 = se->sub_state.begin();
		int belong_set;
		while (_i2 != se->sub_state.end()) {
			int next_stat = table[*_i2][*_i1]; // *_i2 --*_i1--> next_state
			if (next_stat == -1) { belong_set = -1; }
			else { belong_set = get_belong_set_label(next_stat, lp); } // to find next_state belongs to which subset

			map<int, Subset*>::iterator _i3 = mp.find(belong_set); // find the correspoding subset
			if (mp.end() != _i3) { // if the subset is founded
				(*_i3).second->sub_state.insert(*_i2);
			}
			else { // otherwise
				Subset* _st = NewSubset();
				(*_st).sub_state.insert(*_i2);
				mp[belong_set] = _st; // add new element into mp
			}
			++_i2; // do not forget this
		}
		if (mp.size() > 1) {
			InsertElement(mp, lt);
			return;
		}
		mp.clear();
		++_i1;
	}
	lt.push_back(se);
}

void Hopcraft() {
	//now we have a table of DNF, accState, nAccState, allChars. It is enough to construct a minimum DNF.
	lt.push_back(NewSubset(nAccState));
	lt.push_back(NewSubset(accState));
	
	while (lt.size() != lp.size()) {
		lp = lt;
		lt.clear();
		list<Subset*>::iterator _it = lp.begin();
		while (_it != lp.end()) {
			SplitAppendFind(*_it, lt, lp);
			++_it;
		}
	}
}

void BuildMinimumTable() {
// After Hopcraft algorithm, we must reconstruct a minimum table
	list<Subset*>::iterator _it = lt.begin();
	int counter = 0;
	// first, rename the subset
	Subset* start_st = get_belong_set_p(0, lt);
	start_st->label = counter++;
	while (_it != lt.end()) {
		if (*_it != start_st) {
			(*_it)->label = counter++;
		}
		++_it;
	}
	// second,construct the table
	_it = lt.begin();
	while (_it != lt.end()) {
		// for all the characters
		set<char>::iterator _i2 = allChars.begin();
		int new_node = (*_it)->label;
		int node = *(*_it)->sub_state.begin();
		while (_i2 != allChars.end()) {
			int next_node = table[node][*_i2];
			if (next_node != -1) {
				minTable[new_node][*_i2] = get_belong_set_label(next_node, lt);
			}
			
			++_i2;
		}
		++_it;
	}
	// third, mark all the accept state
	set<int>::iterator _i3 = accState.begin();
	newAccStat.clear();
	while (_i3 != accState.end()) {
		newAccStat.insert(get_belong_set_label(*_i3, lt));
		++_i3;
	}
	// all is done!
}
#endif

bool Verify(char *str) {
#ifndef _HOP_CRAFT_
	// to verify the string
	int next_state = table[0][*str];
	++str;
	while (*str != '\0') {
		next_state = table[next_state][*str];
		if (next_state == -1) return false;
		++str;
	}
	if (accState.end() != accState.find(next_state)) {
		return true;
	}
	return false;
#endif // !_HOP_CRAFT_

#ifdef _HOP_CRAFT_
	// to verify the string
	int next_state = minTable[0][*str];
	++str;
	while (*str != '\0') {
		next_state = minTable[next_state][*str];
		if (next_state == -1) return false;
		++str;
	}
	if (newAccStat.end() != newAccStat.find(next_state)) {
		return true;
	}
	return false;
#endif
	
}

/////////////////////////////////////
// destructor
void DeleteAllQNode() {
	list<QNode*>::iterator it = g_lq.begin();
	while (it != g_lq.end()) {
		delete *it;
		++it;
	}
	g_lq.clear();
	Q.clear();
	while (!workList.empty()) {
		workList.pop();
	}
	closure.clear();
	deltaClosure.clear();
}

void Clear() {
	//accState.clear();
	//nAccState.clear();
	allChars.clear();
#ifdef _HOP_CRAFT_
	lp.clear();
	lt.clear();
	DeleteAllSubset();
#endif // _HOP_CRAFT_
	DeleteAllQNode();
	counter = 0;
}

void BuildDfa(char *str) {
	Init(str);
	Re * re = Parse(str);
	Nfa *nfa = Thompson(re);
	BuildDfaTable(nfa);
#ifdef _HOP_CRAFT_
	Hopcraft();
	BuildMinimumTable();
#endif
	FreeRe(re);
	FreeNfa(nfa);
	Clear();
}


