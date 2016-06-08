#include <iostream>
#include <list>
#include "Nfa.h"
using namespace std;

//#define _DEMO_
static list<Node*> ln;
static list<Edge*> le;
////////////////////////////////
// construct functions
static Node * NewNode(int num, Node* next_node) {
	Node *nd = (Node *)malloc(sizeof(Node));
	nd->firstEdge = NULL;
	nd->label = num;
	nd->next = next_node;
	ln.push_back(nd); // record the point of Node
	return nd;
}

static Edge * NewEdge(Node* from, Node* to, char c, Edge* next_edge) {
	Edge *ne = (Edge *)malloc(sizeof(Edge));
	ne->from = from;
	ne->to = to;
	ne->trans_letter = c;
	ne->nextEdge = next_edge;
	le.push_back(ne); // record the point of Edge
	return ne;
}

static Nfa * NewNfa() {
	Nfa *ng = (Nfa *)malloc(sizeof(Nfa));
	ng->accept = NULL;
	ng->head = NULL;
	ng->start = NULL;
	return ng;
}

//////////////////////////////////
// construct NFA

// counter -- we must ensure that each node's label is different
static int counter = 0;
static int NextNum() {
	return counter++;
}

static Nfa* SingleChar(char c) {
	Nfa *g = NewNfa();

	Node *n1 = NewNode(NextNum(), NULL);
	Node *n2 = NewNode(NextNum(), NULL);
	g->head = n1;
	n1->next = n2;

	Edge *e = NewEdge(n1, n2, c, NULL);
	n1->firstEdge = e;

	g->start = n1;
	g->accept = n2;

	return g;
}

static Nfa* Concat(Nfa& a, Nfa& b) {
	Node *aAcc = a.accept;
	Node *bStart = b.start;
	Edge *aEg = aAcc->firstEdge;
	Edge* eg = NewEdge(aAcc, bStart, EPS, aEg);
	aAcc->firstEdge = eg; // insert a edge;
	

	Node *nodeList = a.head;
	Node *record = NULL;
	while (nodeList != NULL) {
		record = nodeList;
		nodeList = nodeList->next;
	}
	record->next = b.head;
	a.accept = b.accept;
	free(&b);
	return &a;
}

static Nfa* Or(Nfa& a, Nfa& b) {
	Node *n1 = NewNode(NextNum(), NULL);
	Node *n2 = NewNode(NextNum(), NULL);
	Node *nodeList = a.head;
	//Node *ahead = a.head;
	Node *record = NULL;
	while (nodeList != NULL) {
		record = nodeList;
		nodeList = nodeList->next;
	}
	record->next = b.head;
	nodeList = b.head;
	while (nodeList != NULL) {
		record = nodeList;
		nodeList = nodeList->next;
	}
	record->next = n1;
	
	n1->next = n2;
	Edge *eg1 = NewEdge(n1, a.start, EPS, NULL);
	n1->firstEdge = eg1;
	Edge *eg2 = NewEdge(n1, b.start, EPS, eg1);
	n1->firstEdge = eg2;

	Edge *eg3 = NewEdge(a.accept, n2, EPS, a.accept->firstEdge);
	a.accept->firstEdge = eg3;
	Edge *eg4 = NewEdge(b.accept, n2, EPS, b.accept->firstEdge);
	b.accept->firstEdge = eg4;

	a.start = n1;
	a.accept = n2;
	free(&b);
	return &a;
}

static Nfa* Kleene(Nfa& a) {
	Node* n1 = NewNode(NextNum(), a.start);
	Node* n2 = NewNode(NextNum(), NULL);
	Node* nodeList = a.head;
	Node* record = NULL;
	while (nodeList != NULL) {
		record = nodeList;
		nodeList = nodeList->next;
	}
	record->next = n1;
	n1->next = n2;
	Edge *eg1 = NewEdge(n1, a.start, EPS, NULL);
	Edge *eg2 = NewEdge(n1, n2, EPS, eg1);
	n1->firstEdge = eg2;
	Edge *eg3 = NewEdge(a.accept, a.start, EPS, a.accept->firstEdge);
	Edge *eg4 = NewEdge(a.accept, n2, EPS, eg3);
	a.accept->firstEdge = eg4;
	a.accept = n2;
	a.start = n1;
	return &a;
}

void DisplayNfa(Nfa& g) {
	Node *node = g.head;
	while (node != NULL) {
		Edge *eg = node->firstEdge;
		while (eg != NULL) {
			cout << "q" << node->label << "--->" << "q" << eg->to->label << "  Symbol : " << eg->trans_letter << endl;
			eg = eg->nextEdge;
		}
		node = node->next;
	}
}

/////////////////////////////////////////
// follow the Re, construct a Nfa
Nfa* Thompson(Re* r) {
	Nfa *g = NULL;
	switch (r->kind) {
	case RE_KIND_CHAR: {
		Re_Char * rc = (Re_Char *)r;
		g = SingleChar(rc->c);
#ifdef  _DEMO_
		cout << "______________________________" << endl;
		DisplayNfa(*g);
#endif //  _DEMO_
		return g;
	}
	case RE_KIND_CONCAT: {
		Re_Concat *rc = (Re_Concat *)r;
		Nfa *l = Thompson(rc->left);
		Nfa *r = Thompson(rc->right);
		g = Concat(*l, *r);
#ifdef  _DEMO_
		cout << "______________________________" << endl;
		DisplayNfa(*g);
#endif //  _DEMO_
		return g;
	}
	case RE_KIND_ALT: {
		Re_Alt *ra = (Re_Alt *)r;
		Nfa *l = Thompson(ra->left);
		Nfa *r = Thompson(ra->right);
		g = Or(*l, *r);
#ifdef  _DEMO_
		cout << "______________________________" << endl;
		DisplayNfa(*g);
#endif //  _DEMO_
		return g;
	}
	case RE_KIND_CLOSURE: {
		Re_Closure *rc = (Re_Closure *)r;
		Nfa *l = Thompson(rc->left);
		g = Kleene(*l);
#ifdef  _DEMO_
		cout << "______________________________" << endl;
		DisplayNfa(*g);
#endif //  _DEMO_
		return g;
	}
	default:
		break;
	}
	return g;
}

void FreeNfa(Nfa* f) {
	list<Node*>::iterator _ln = ln.begin();
	while (_ln != ln.end()) {
		free(*_ln);
		++_ln;
	}
	list<Edge*>::iterator _le = le.begin();
	while (_le != le.end()) {
		free(*_le);
		++_le;
	}
	ln.clear();
	le.clear();
	free(f);
	counter = 0;
}

static int _main() {
	cout << "(a|b)((c|d)*) : \n";
	Re * re = Parse("(a|b)((c|d)*)");
	PrintReTree(re);
	cout << endl;
	Nfa *nfa = Thompson(re);
	system("pause");
	return 0;
}


