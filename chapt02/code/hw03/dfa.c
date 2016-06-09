#include <assert.h>
#include <stdio.h>
#include "dfa.h"
#include "set.h"
#include "new.h"

// 基本上算是看懂了，很好理解，

static int const EPSILON = -2;
static int const MAX_CHAR = 255;   // the symbols of DFA/NFA are ASCII codes

typedef struct _DfaNode_t *DfaNode_t;
typedef struct _DfaEdge_t *DfaEdge_t;

struct _DfaNode_t
{
  int _num; // 表示有多少个节点么？
  DfaEdge_t _edges; // 看来这个玩意是用一张图来表示的
  Set_t _subStates;   // the NFA state or DFA substate，所包含的nfa节点
  DfaNode_t _parent;  // used by hopcroft algorithm
};

struct _DfaEdge_t
{
  int _c;
  DfaNode_t _from; // 从这条边
  DfaNode_t _to;   // 去往另外一条边
  DfaEdge_t _next; // 连接向另外的一条边
};

struct _Dfa_t
{
  int _start;        // position of DFA start state in _states 开始节点
  Set_t _accept;     // positions of DFA accept states in _states // 可以接受的节点
  Set_t _states;     // a set of all DFA state // 全部的DFA节点
};

static DfaNode_t DfaNode_new(Set_t subStates)
{
  DfaNode_t p;
  NEW(p);
  p->_num = -1;
  p->_edges = NULL;
  p->_subStates = subStates;
  p->_parent = NULL;
  return p;
}

static DfaEdge_t DfaEdge_new(  DfaNode_t from
                             , DfaNode_t to
                             , int c
                             , DfaEdge_t edge)
{
  DfaEdge_t p;
  NEW(p);
  p->_c = c;
  p->_from = from;
  p->_to = to;
  p->_next = edge;
  return p;
}

static void Eps_closure_doit(Set_t s, Node_t nfaNode) // 这个东西用的是深度搜索吧？
{
  Set_insertPoint(s, (void *)nfaNode); // 将这个节点率先插入
  nfaNode->visited = 1; // 表示这个节点已经访问过了。
  Edge_t e = nfaNode->edges; // dfa
  while (NULL != e)
  {
    if ((EPSILON == e->c) && (0 == e->to->visited))
    {
      Eps_closure_doit(s, e->to); // 好吧，果然是这样子的
    }
    e = e->next;
  }
}

static void Eps_closure(Set_t s, Nfa_t nfa, Node_t nfaNode)
{
  // clear all visited flags in NFA
  Node_t p = nfa->nodes; // nfa不过是一张类似于邻接表的东东
  while (NULL != p)
  {
    p->visited = 0;
    p = p->next;
  }
 
  Eps_closure_doit(s, nfaNode);
}

static void Nfa_Delta(Set_t nfaStates, Node_t nn, int c) // c是一个字符，其实求的只是从nn这个节点出发，通过c可以到达的地方
{
  Edge_t e = nn->edges;
  while (NULL != e)
  {
    if (c == e->c) { Set_insertPoint(nfaStates, (void *)(e->to)); }
    e = e->next;
  }
}

static Set_t Delta(DfaNode_t src, int c, Nfa_t nfa)
{
  Set_t retVal = Set_new();
  Set_t nfaFromStates = src->_subStates;
  Set_t nfaToStates = Set_new(); // 果然是新建一个set
  int i = nfaFromStates->_size - 1;
  for (; i >= 0; --i)
  {
    Nfa_Delta(nfaToStates, (Node_t)(nfaFromStates->_elem[i]), c);
  }

  i = nfaToStates->_size - 1;
  for (; i >= 0; --i)
  {
    Eps_closure(retVal, nfa, (Node_t)(nfaToStates->_elem[i]));
  }
  Set_free(nfaToStates);

  return retVal;
}

static Dfa_t Dfa_new()
{
  Dfa_t p;
  NEW(p);
  p->_start = -1;
  p->_accept = Set_new();
  p->_states = Set_new();
  return p;
}

static int Dfa_lookupOrInsert(Dfa_t dfa, Set_t nfaStates, DfaNode_t * result) // 如果不在Dfa之中的话，那么就插入
{
  assert (dfa);
  Set_t dfaSet = dfa->_states;
  int i = dfaSet->_size - 1;
  while (i >= 0)
  {
    Set_t ns = ((DfaNode_t) dfaSet->_elem[i])->_subStates;
    if (0 != Set_isEqual(ns, nfaStates))
    {
      *result = (DfaNode_t) dfaSet->_elem[i];
      return 1;
    }
    --i;
  }
  *result = DfaNode_new(nfaStates); // 构建一个DfaNode节点，并且返回这个节点的指针
  (*result)->_num = Set_insertPoint(dfa->_states, (void *)(*result));
  return 0;
}

static void Dfa_addEdge(DfaNode_t from, DfaNode_t to, int c)
{
  from->_edges = DfaEdge_new(from, to, c, from->_edges);
}

// work list for DFA
struct WorkList_t // 居然是所谓的workList，我记得我是用queue实现的吧！
{
  DfaNode_t dfa_node; // 其实也差不太多啦！
  struct WorkList_t *next;
};

static struct WorkList_t * Add_node(DfaNode_t node, struct WorkList_t *next)
{
  struct WorkList_t *p = malloc (sizeof(*p));
  p->dfa_node = node;
  p->next = next;
  return p;
}

static struct WorkList_t * Pop_node(struct WorkList_t *list)
{ // 这里实际是一个小型的queue
  if (NULL == list) { return NULL; }
  struct WorkList_t *retVal = list->next;
  free(list);
  return retVal;
}

Dfa_t Dfa_build(Nfa_t nfa)
{ // 用来构建Dfa
  Dfa_t dfa = Dfa_new();

  Node_t q0 = Nfa_lookupOrInsert(nfa, nfa->start); // 找到一个开始节点
  Node_t qFinal = Nfa_lookupOrInsert(nfa, nfa->accept); // 一个结束的节点
  
  Set_t s0 = Set_new();
  Eps_closure(s0, nfa, q0); // 从开始的节点q0出发，通过epsilon能够到达的所有节点都会被放入s0之中
  
  DfaNode_t nFrom = NULL; 
  DfaNode_t nTo = NULL; // 
  
  Dfa_lookupOrInsert(dfa, s0, &nFrom); // 插入dfa之中，如果nFrom = NULL，表示已经存在了，不为NULL的话，说明插入了，这就是源节点
  dfa->_start = nFrom->_num; // 设立了开始节点
  if (0 != Set_findPoint(s0, qFinal, NULL)) 
  {
    Set_insertInt(dfa->_accept, nFrom->_num);
  }

  struct WorkList_t * workList = Add_node(nFrom, NULL);
  while (NULL != workList)
  {
    nFrom = workList->dfa_node;
    workList = Pop_node(workList); // 出栈
    int c = 0;
    for (; c <= MAX_CHAR; ++c) // 说实话，这种循环实在有点过多
    {
      Set_t s = Delta(nFrom, c, nfa); // 从nFrom出发，通过c可以到达的节点记入s
      if (0 == s->_size)
      {
        Set_free(s); 
        continue;
      }

      if (0 == Dfa_lookupOrInsert(dfa, s, &nTo))  // 这种状态已经存在了
      {
        workList = Add_node(nTo, workList);
        if (0 != Set_findPoint(s, qFinal, NULL)) // 如果在s中找到了终止节点，那么，s是终止状态
        {
          Set_insertInt(dfa->_accept, nTo->_num); // _accept也是一个集合吧！
        }
      }
      else
      {
        Set_free(s);
      }
      Dfa_addEdge(nFrom, nTo, c); // 添加边，从nFrom经过c到达nTo
    }
  }
  return dfa; // 最终构建了一张图
}

void Dfa_print(Dfa_t dfa)
{
  assert (dfa);
  Set_t states = dfa->_states;
  int i = states->_size - 1;
  for (; i >= 0; --i)
  {
    DfaEdge_t edges = ((DfaNode_t)(states->_elem[i]))->_edges;
    while (edges){
      printf ("%d ----(%d)----> %d\n"
              , edges->_from->_num
              , edges->_c
              , edges->_to->_num);
      edges = edges->_next;
    }
  }
  printf("start is (%d)\n", dfa->_start);
  printf("accept is ");
  Set_printInts(dfa->_accept);
}


//
// hopcroft algorithm
//

// 下面的代码才是重头戏

typedef struct _TransitList_t *TransitList_t;
struct _TransitList_t
{
  DfaNode_t toState; 
  Set_t fromStates; // 从这个集合
  TransitList_t next; // 这个东西也构成了一张链表
};

static TransitList_t Add_Transit(TransitList_t list
                                 , DfaNode_t from
                                 , DfaNode_t to) // 好吧，这个函数我还是看懂了！很有意思！
{
  TransitList_t p = list; // _TransitList_t表示连接的关系
  while ((NULL != p) && (to != p->toState)) { p = p->next; } // 这其实很有意思，是吧，找到对应的连接，然后
  // toState这个玩意到底是干什么的，这里在一直搜索
  if (NULL != p)
  {
    Set_insertPoint(p->fromStates, (void *)from); // 插入一个新的节点，有意思
    return list;
  }
  else
  {
    p = malloc (sizeof(*p)); // 构建一个新的划分，是吧！
    p->toState = to;
    p->fromStates = Set_new();
    Set_insertPoint(p->fromStates, (void *)from);
    p->next = list;
    return p;
  }
}

static TransitList_t Pop_transit(TransitList_t list) // 这玩意也构成了queue一样的东西
{
  if (NULL == list) { return NULL; }
  TransitList_t retVal = list->next;
  free(list);
  return retVal;
}

static int Transit_Size(TransitList_t list)
{
  int retVal = 0;
  while (NULL != list)
  {
    ++retVal;
    list = list->next;
  }
  return retVal;
}

static int Is_Accept(Dfa_t dfa, DfaNode_t state)
{
  int num = state->_num;
  return Set_findInt(dfa->_accept, num, NULL);
}

static void Create_relation(DfaNode_t parent, Set_t children)
{ // 这个玩意是干什么的呢？
  int i = children->_size - 1;
  for (; i >=0; --i)
  {
    ((DfaNode_t) children->_elem[i])->_parent = parent;
  } // 我的理解是建立所谓的联系
}

// if c can split dn, this function return 1, otherwise 0
static int SplitDoit(Dfa_t dfa, DfaNode_t dn, int c) // dfa是一个全集,dn是全集中的一个元素
{
  TransitList_t list = NULL; // 转换表
  Set_t states = dn->_subStates; // 得到一个划分中所有的元素组成的set
  int i = states->_size - 1;
  for (; i >= 0; --i)
  {
    DfaNode_t from = (DfaNode_t) states->_elem[i]; // 起始的节点
    DfaEdge_t edges = from->_edges; //边
    int isAdded = 0;
    while (edges)
    {
      if (c == edges->_c) // 从from这个点出发，通过c能够到达的节点
      {
        list = Add_Transit(list, from, edges->_to->_parent);  // 添加所谓的转化吗？这里的parent指的是_to所属的集合吧！
        isAdded = 1; // 这个玩意是一个flag，表示 
      }
      edges = edges->_next;
    }
    if (0 == isAdded) { list = Add_Transit(list, from, dn); } // 没有的话，添加新的东西吧！
  }

  if (Transit_Size(list) <= 1) { return 0; } // 返回0表示不能够被切分，是吧！

  dn->_subStates = list->fromStates; // 出现了划分，所以这里要求
  while ( NULL != (list = Pop_transit(list)) ) // 一个一个弹出来
  {
    DfaNode_t node = DfaNode_new(list->fromStates); // 构建一个新的DfaNode节点
    node->_num = Set_insertPoint(dfa->_states, (void *)node); // 好吧，这里干的事情是将东西插入全集
    Create_relation(node, node->_subStates); // 创建关系，表示
  }

  return 1;
}

// if dn can be split, this function return 1, otherwise 0
static int Split(Dfa_t dfa, DfaNode_t dn) // dfa是一个全集，而dn是一个子集
{
  int retVal = 0; // 这玩意要怎么来切分？
  int c = 0;
  for (; c <= MAX_CHAR; ++c) // 好吧，居然每个字符都要尝试一遍
  {
    if (0 != SplitDoit(dfa, dn, c)) { retVal = 1; }
  }
  return retVal; // 表示集合发生了改变
}

static Dfa_t initMiniDfa(Dfa_t dfa)
{
  Dfa_t retVal = Dfa_new(); // 构建的一个新的Dfa
  DfaNode_t acceptStates = DfaNode_new(Set_new()); // 新的接收节点的集合
  DfaNode_t otherStates = DfaNode_new(Set_new()); // 新的其他节点的集合
  acceptStates->_num = Set_insertPoint(retVal->_states, (void *)acceptStates);
  otherStates->_num = Set_insertPoint(retVal->_states, (void *)otherStates);

  Set_t states = dfa->_states;
  int i = states->_size - 1;
  for (; i >=0; --i)
  {
    DfaNode_t dn = (DfaNode_t)(states->_elem[i]); // 取出每一个元素
    if (0 == Is_Accept(dfa, dn)) // 不是Acc的节点
    {
      Set_insertPoint(otherStates->_subStates, (void *)dn); // 加入otherStates
      dn->_parent = otherStates;
    }
    else
    {
      Set_insertPoint(acceptStates->_subStates, (void *)dn);
      dn->_parent = acceptStates;
    }
  }
  return retVal;
}

static DfaNode_t Get_to_state(DfaNode_t from, int c)
{
  int i = from->_subStates->_size - 1;
  for (; i >=0; --i)
  {
    DfaNode_t dn = (DfaNode_t) from->_subStates->_elem[i];
    DfaEdge_t edges = dn->_edges;
    while (edges)
    {
      if (c == edges->_c) { return edges->_to->_parent; }
      edges = edges->_next;
    }
  }
  return NULL;
}

static void Build_Edges(Dfa_t dfa)  // 这个函数是干什么的呢？
{
  int i = dfa->_states->_size - 1;
  for (; i >=0; --i)
  {
    DfaNode_t dn = (DfaNode_t) dfa->_states->_elem[i];
    DfaNode_t to = NULL;
    int c = 0;
    for (; c <= MAX_CHAR; ++c)
    {
      if ( NULL != (to = Get_to_state(dn, c)) ) // 这里估计查找从dn节点出发，经过字符c可以到达的节点
      {
        Dfa_addEdge(dn, to, c);
      }
    }
  }
}

static void Add_start(Dfa_t miniDfa, Dfa_t orgDfa) // 添加所谓的起始点
{
  DfaNode_t tmp = (DfaNode_t) orgDfa->_states->_elem[orgDfa->_start]; 
  tmp = tmp->_parent;
  int i = miniDfa->_states->_size - 1;
  for (; i >=0; --i)
  {
    if (((void *) tmp) == miniDfa->_states->_elem[i])
    {
      miniDfa->_start = i;
      return;
    }
  }
}

static void Add_accept(Dfa_t miniDfa, Dfa_t orgDfa)
{
  int i = miniDfa->_states->_size - 1;
  for (; i >=0; --i)
  {
    DfaNode_t dn = (DfaNode_t) miniDfa->_states->_elem[i];
    int j = dn->_subStates->_size - 1;
    for (; j >=0; --j)
    {
      if (0 != Is_Accept(orgDfa, (DfaNode_t) dn->_subStates->_elem[j]))
      {
        Set_insertInt(miniDfa->_accept, i);
        break;
      }
    }
  }
}

Dfa_t Dfa_minimize(Dfa_t dfa)
{
  Dfa_t retVal = initMiniDfa(dfa); // 这个就是起始的函数了
  int changed = 1; // 表示这个玩意已经开始分裂了
  while (0 != changed)
  {
    changed = 0;
    int i = 0;
    while (i < retVal->_states->_size)
    {// the size will be change in while loop
      if (0 != Split(retVal, (DfaNode_t) retVal->_states->_elem[i])) // 开始切分
      { // retVal不是一个Dfa_t的类型吗?然后states指的是所有的子状态
        changed = 1;
      }
      ++i;
    }
  }

  Build_Edges(retVal);
  Add_start(retVal, dfa);
  Add_accept(retVal, dfa);
  return retVal;
}
