#include "Dfa.h"
#include <set>
#include <iostream>
#include <queue>
using namespace std;
// now we must implement 最小子集算法

#define MAXV (256)
static int table[MAXV][MAXV] = { 0 };
set<char> allChars; // 包含了所有的字符

/////////////////////////////////////////
// init functions
static void InitTable() {
	for (int i = 0; i < MAXV; ++i) {
		for (int j = 0; j < MAXV; ++j) {
			table[i][j] = -1;
		}
	}
}

static bool isCharRight(char c) {
	if ((c != '(') && (c != ')') && (c != '|') && (c != '*')) {
		return true;
	}
	return false;
}

void Init(char *str) {
	while (*str != '\0') {
		if (isCharRight(*str)) {
			allChars.insert(*str);
		}
		++str;
	}
	InitTable();
}

static int counter = 0;
int next_num() {
	return counter++;
}

QNode *NewQNode(int label, set<Node*>& s) {
	QNode* qnd = new QNode;
	qnd->label = label;
	qnd->subStates = s;
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
// 
set<Node*> closure;

void eps_closure(Node* nd) { // 深度搜索
	// from this node, visit all the nodes
	closure.insert(nd); // 先将自己插入
	Edge *arc = nd->firstEdge;
	while (arc != NULL) {
		if (arc->trans_letter == EPS) {
			if (!closure.insert(arc->to).second) { // 该元素已经存在了
				return;
			}
			eps_closure(arc->to);
		}
		arc = arc->nextEdge;
	}
}

void e_closure(set<Node*>& deltaClosure) {
	set<Node*>::iterator it = deltaClosure.begin();
	closure.clear();
	while (it != deltaClosure.end()) {
		eps_closure(*it);
		++it;
	}
}

set<Node*> deltaClosure;
void delta(set<Node*>& closure, char c) {
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

bool contain(set<QNode*>& Q, QNode& nd) {
	set<QNode *>::iterator it = Q.begin();
	while (it != Q.end()) {
		set<Node*>& _s1 = (*it)->subStates;
		set<Node*>& _s2 = nd.subStates;
		if (_s1.size() != _s2.size()) {
			++it; continue;
		}
		set<Node*>::iterator _i1 = _s1.begin();
		set<Node*>::iterator _i2 = _s2.begin();
		while (_i1 != _s1.end()) {
			if (*_i1 != *_i2) goto end;
			++_i1; ++_i2;
		}
		return true;
	end:
		++it;
	}
	return false;
}

set<QNode*> Q;
queue<QNode*> workList;
void build_dfa_table(Nfa *g) {
	Node *nd = g->start;
	eps_closure(nd);
	PrintClosure(closure);
	QNode *qnd = NewQNode(next_num(), closure);
	Q.insert(qnd); // insert a QNode point
	workList.push(qnd);
	while (!workList.empty()) {
		QNode *nd = workList.front();
		workList.pop();
		set<char>::iterator it = allChars.begin();
		while (it != allChars.end()) {
			delta(nd->subStates, *it);
			PrintClosure(deltaClosure);

			e_closure(deltaClosure);
			PrintClosure(closure);
			if (closure.size() == 0) {
				++it; continue;
			}

			qnd = NewQNode(next_num(), closure); // 创建一个新的东西
			table[nd->label][*it] = qnd->label; // 填入新的表单
			if (!contain(Q, *qnd)) { // 如果没有包含了这个节点，那么
				workList.push(qnd);
				Q.insert(qnd);
			}
			++it;
		}
	}
}

static int _main() {
	char rexp[] = "a(b|c)*";
	Init(rexp);
	Re * re = Parse(rexp); 
	PrintReTree(re);
	cout << endl;
	Nfa *nfa = Thompson(re);
	build_dfa_table(nfa);
	system("pause");
	return 0;
}