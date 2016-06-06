#pragma once
#include "Re.h"
///////////////////////////////
// data structure : use adjacency lists 
#define EPS NULL
struct Node;

typedef struct Edge {
	char trans_letter;
	Node *from;
	Node *to;
	Edge *nextEdge;
}Edge;

typedef struct Node {
	int label;
	Edge *firstEdge;
	Node *next;
}Node;


typedef struct Nfa {
	Node *head;
	Node *start;
	Node *accept;
}Nfa;

Nfa* Thompson(Re* r);
void DisplayNfa(Nfa& g);

