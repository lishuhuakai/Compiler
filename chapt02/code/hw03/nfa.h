#ifndef NFA_H
#define NFA_H

typedef struct _Node_t *Node_t;
typedef struct _Edge_t *Edge_t;

struct _Node_t
{
  int num; // 用来标识节点
  int visited; // 这个玩意表示什么含义
  Edge_t edges; // 边的指针
  Node_t next; // 下一个节点
};

struct _Edge_t
{
  int c; // 转换的字符
  Node_t from; // 用指针确实比用数字更加高明,可以减少很多额外的开销
  Node_t to; 
  Edge_t next; // 下一条edge
};

typedef struct _Nfa_t *Nfa_t;
struct _Nfa_t // 这里其实非常有意思，确实可以参考这里的代码，这里不是邻接表，而是稍微做了一下变化
{
  int start;     // start node of an NFA
  int accept;    // accept node of an NFA
  Node_t nodes;  // a list of all nodes
};

Nfa_t Nfa_new ();
void Nfa_addEdge (Nfa_t nfa, int from, int to, int c);
void Nfa_print (Nfa_t nfa);
Node_t Nfa_lookupOrInsert (Nfa_t nfa, int num);

#endif
