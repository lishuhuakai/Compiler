// 构造一棵树
#define _CRT_SECURE_NO_WARNINGS
#include "Re.h"
#include <cstring>
#include <iostream>
using namespace std;

static char leftRe[256];
static char rightRe[256];

static bool isdigit(char c) {
	if ((c >= '0') && (c <= '9'))
		return true;
	else return false;
}

static bool isletter(char c) {
	if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
		return true;
	}
	else {
		return false;
	}
}

static bool isOprand(char c) {
	if ((c != '*') && (c != '|') && (c != '(') && (c != ')')) {
		return false;
	}
	else {
		return true;
	}
}

/////////////////////////////////
// constructor
static Re_Char * NewReChar(char c) {
	Re_Char *nc = (Re_Char *)malloc(sizeof(Re_Char));
	nc->c = c;
	nc->kind = RE_KIND_CHAR;
	return nc;
}

static Re_Concat *NewConcat(Re* l, Re *r) {
	Re_Concat *nc = (Re_Concat *)malloc(sizeof(Re_Concat));
	nc->kind = RE_KIND_CONCAT;
	nc->left = l;
	nc->right = r;
	return nc;
}

static Re_Alt *NewAlt(Re *l, Re *r) {
	Re_Alt *na = (Re_Alt *)malloc(sizeof(Re_Alt));
	na->kind = RE_KIND_ALT;
	na->left = l;
	na->right = r;
	return na;
}

static Re_Closure *NewClosure(Re *l) {
	Re_Closure * nc = (Re_Closure *)malloc(sizeof(Re_Closure));
	nc->kind = RE_KIND_CLOSURE;
	nc->left = l;
	return nc;
}

static bool strcontain(char *str, char c) {
	int len = strlen(str);
	for (int i = 0; i < len; ++i) {
		if (str[i] == c)
			return true;
	}
	return false;
}

char * GetLeftRe(char *re, bool flag = false) { // 找到左边的Re语句
	int index = 0;
	if (*re == '(') {
		int counter = 1;
		re++;
		while (counter != 0) {// 我们应该做的是去掉一层括号
			if (((*re == ')') && counter > 1) || (*re != ')')) 
				leftRe[index++] = *re;
			if (*re == '(') {
				counter++;
			} 
			else if (*re == ')') {
				counter--;
			}
			re++;
		}
	}
	else if (strcontain(re, '|')  || strcontain(re, '*') || flag)
	{
		while (!isOprand(*re) && (*re != '\0')) {
			leftRe[index++] = *re;
			re++;
		}
	}
	else {
		leftRe[index++] = *re;
	}
	leftRe[index] = '\0';
	return leftRe;
}

Re* Parse(char *str) {
	char *re = str;
	char buf[256];
	int len = 0;
	Re *left = NULL;
	
	while (*re != '\0') {
		
		if (strlen(re) == 1 && !isOprand(*re)) { // regular letter
			if (left == NULL) {
				return (Re *)NewReChar(*re);
			}
			else {
				return (Re *)NewConcat(left, (Re *)NewReChar(*re));
			}
			
		}

		if (*re == '|') {
			char *rre = GetLeftRe(re + 1, true);
			strcpy(buf, rre);
			len = strlen(buf); // length of leftRe
			
			left = (Re *)NewAlt(left, Parse(buf));
			if (*(re + 1) == '(') {
				re += (1 + len + 2);
			} 
			else {
				re += (1 + len);
			}
			continue;
		}

		if (*re == '*') {
			left = (Re *)NewClosure(left);
			re += 1;
			continue;
		}

		char *lre = GetLeftRe(re);
		strcpy(buf, lre);
		len = strlen(buf); // length of leftRe
	
		if (left == NULL) {
			left = Parse(buf); // Parse LeftRe of Re
		}
		else {
			left = (Re *)NewConcat(left, Parse(buf));
		}
		
		if (*re == '(') { // we must omit the left parameter
			re += (len + 2);
		}
		else {
			re += len;
		}	
	}
	return left;
}

void PrintReTree(Re *t) {
	switch (t->kind) {
	case RE_KIND_CHAR: {
		Re_Char *r = (Re_Char *)t;
		cout << r->c;
		break;
	}
	case RE_KIND_ALT: {
		Re_Alt *r = (Re_Alt *)t;
		cout << "[";
		PrintReTree(r->left);
		cout << "|";
		PrintReTree(r->right);
		cout << "]";
		break;
	}
	case RE_KIND_CLOSURE: {
		Re_Closure *r = (Re_Closure *)t;
		cout << "[";
		PrintReTree(r->left);
		cout << "*";
		cout << "]";
		break;
	}
	case RE_KIND_CONCAT: {
		Re_Concat *r = (Re_Concat *)t;
		cout << "[";
		PrintReTree(r->left);
		PrintReTree(r->right);
		cout << "]";
		break;
	}
	default:
		break;
	}
}

int main() {
	char re[25] = "a(b|c)*";
	
	Re * r = Parse(re);
	PrintReTree(r);

	system("pause");
	return 0;
}
