#define _CRT_SECURE_NO_WARNINGS
#include "Re.h"
#include <cstring>
#include <cassert>
#include <iostream>
using namespace std;
//#define _DEMO_

static char leftRe[256];
static char rightRe[256];

static bool IsOperator(char c) {
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

static bool StrCont(char *str, char c) {
	int len = strlen(str);
	for (int i = 0; i < len; ++i) {
		if (str[i] == c)
			return true;
	}
	return false;
}

typedef struct Part { // used for re
	int size;
	char re[32];
}Part;

Part GetLeftPart(char *re) { // ÕÒµ½×ó±ßµÄReÓï¾ä
	Part rs;
	int len = 0;
	int index = 0;
	int len_of_re = strlen(re);
	if (*re == '(') { // re start with left parathesis
		int counter = 1;
		leftRe[index++] = '('; 
		re++; 
		len++;
		while (counter != 0) {
			leftRe[index++] = *re;
			if (*re == '(') counter++;
			else if (*re == ')') counter--;
			re++; 
			len++;
		}

		bool flag = (*re == '*');
		if (flag && len < (len_of_re - 1)) { // eg. leftRe = "(b|c)" and re = "(b|c)*xxx"
			leftRe[index++] = '*'; 
			leftRe[index] = '\0'; // now leftRe = "(b|c)*"
			rs.size = len + 1;
			strcpy_s(rs.re, leftRe);
		}
		else if (flag && len == (len_of_re - 1)) { // eg. leftRe = "(b|c)" and re = "(b|c)"
			rs.size = len;
			leftRe[index] = '\0';
			strcpy_s(rs.re, leftRe);
		}
		else { // eg. leftRe = "(b|c)" and re = "(b|c)xxx"
			rs.size = len;
			leftRe[index] = '\0';
			strncpy_s(rs.re, leftRe + 1, len - 2); // remove the parathesis
		}
	}
	else { // re start with regular letter
		while (!IsOperator(*re) && (*re != '\0')) {
			leftRe[index++] = *re;
			re++;
			len++;
		}

		if ((len < len_of_re) && (*re == '|')) { // eg. leftRe = "xyz" and re = "xyz|xxx"
			rs.size = len;
			leftRe[index] = '\0';
			strcpy_s(rs.re, leftRe);
		}
		else if (len == 1 && (*re == '*') && len_of_re > 2) { // eg. leftRe = "x" and re = "x*as"
			rs.size = 2;
			rs.re[0] = leftRe[0]; 
			rs.re[1] = '*'; 
			rs.re[2] = '\0';
		}
		else { // eg. leftRe = 'x' and re = "x*" or leftRe = 'xyze' and re = "xyzekuX"
			rs.size = 1;
			rs.re[0] = leftRe[0];
			rs.re[1] = '\0';
		}
	}
	return rs;
}

Part GetConcatRightPart(char *re) {
	int index = 0;
	int len = 0;
	Part rs;
	if (*re == '(') {
		int counter = 1;
		rightRe[index++] = '(';
		re++;
		len++;
		while (counter != 0) {
			rightRe[index++] = *re;
			if (*re == '(') counter++;
			else if (*re == ')') counter--;
			len++;
			re++;
		}
		if (*re == '*') {
			rightRe[index++] = '*';
			rightRe[index] = '\0'; // now leftRe = "(b|c)*"
			rs.size = len + 1;
			strcpy_s(rs.re, rightRe);
		}
		else { // eg. rightRe = "((b|c)*)" and re = "((b|c)*)"
			rightRe[index] = '\0';
			rs.size = len; // remove the parathesis
			strncpy_s(rs.re, rightRe + 1, len - 2); // remove the parathesis
		}
	}
	else {
		rs.size = 1;
		rs.re[0] = re[0];
		rs.re[1] = '\0';
	}
	return rs;
}

Part GetOrRightPart(char *re) {
	Part rs;
	int len = 0;
	int index = 0;
	int size_of_left_para = 0; // the number of left parathesis
	int size_of_right_para = 0; // the number of right parathesis
	while (*re != '\0') {
		if (*re == '(') size_of_left_para++;
		else if (*re == ')') size_of_right_para++;

		if (size_of_right_para > size_of_left_para) break;
		rightRe[index++] = *re;
		re++; len++;
	}
	rs.size = len;
	rightRe[index] = '\0';
	strcpy_s(rs.re, rightRe);
	return rs;
}

Re* Parse(char *str) {
	char *re = str;
	Re *left = NULL;
	
	while (*re != '\0') {
		// sigle character && not operator
		if (strlen(re) == 1 && !IsOperator(*re)) { // regular letter
			if (left == NULL) {
				return (Re *)NewReChar(*re);
			}
			else {
				return (Re *)NewConcat(left, (Re *)NewReChar(*re));
			}	
		}

		// or
		if (*re == '|') {
			Part rp = GetOrRightPart(re + 1);
			left = (Re *)NewAlt(left, Parse(rp.re));
#ifdef _DEMO_
			PrintReTree(left);
			cout << endl;
#endif // _DEMO_
			re += (1 + rp.size);
			continue;
		}

		// kleene
		if (*re == '*') {
			left = (Re *)NewClosure(left);
#ifdef _DEMO_
			PrintReTree(left);
			cout << endl;
#endif // _DEMO_
			re += 1;
			continue;
		}

		// concat
		if (left == NULL) {
			Part lp = GetLeftPart(re); // left part
			left = Parse(lp.re); // Parse LeftRe of Re
#ifdef _DEMO_
			PrintReTree(left);
			cout << endl;
#endif // _DEMO_
			re += lp.size;
		}
		else {
			Part rp = GetConcatRightPart(re); // right part
			left = (Re *)NewConcat(left, Parse(rp.re));
#ifdef _DEMO_
			PrintReTree(left);
			cout << endl;
#endif // _DEMO_
			re += rp.size;
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

void FreeRe(Re *t) {
	switch (t->kind) {
	case RE_KIND_CHAR: {
		Re_Char *r = (Re_Char *)t;
		free(r);
		break;
	}
	case RE_KIND_ALT: {
		Re_Alt *r = (Re_Alt *)t;
		FreeRe(r->left);
		FreeRe(r->right);
		free(r);
		break;
	}
	case RE_KIND_CLOSURE: {
		Re_Closure *r = (Re_Closure *)t;
		FreeRe(r->left);
		free(r);
		break;
	}
	case RE_KIND_CONCAT: {
		Re_Concat *r = (Re_Concat *)t;
		FreeRe(r->left);
		FreeRe(r->right);
		free(r);
		break;
	}
	default: 
		free(t);
		break;
	}
}
static int _main() {
	char re[25] = "(a|b)((c|d)*)";
	Re * r = Parse(re);
	PrintReTree(r);
	system("pause");
	return 0;
}
