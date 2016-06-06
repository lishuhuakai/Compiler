#pragma once
#include "Nfa.h"
//////////////////////////////
// data structure : 
#include <set>
typedef struct QNode {
	int label;
	std::set<Node*> subStates; // 所包含的那些节点
}QNode;
