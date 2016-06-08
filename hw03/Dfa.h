#pragma once
#include "Nfa.h"
//////////////////////////////
// data structure 
#include <set>
typedef struct QNode {
	int label;
	std::set<Node*> subStates; // 所包含的那些节点
}QNode;

#define MAXV (256)
extern int table[MAXV][MAXV];

#define _HOP_CRAFT_
#ifdef _HOP_CRAFT_
extern int minTable[MAXV][MAXV]; // for Hopcraft 
#endif // _HOP_CRAFT_

void Init(char *str);
bool Verify(char *str);
void BuildDfa(char *);
