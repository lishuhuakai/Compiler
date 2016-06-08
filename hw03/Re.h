#pragma once

typedef enum RE_KIND {
	RE_KIND_CHAR,
	RE_KIND_CONCAT,
	RE_KIND_ALT,
	RE_KIND_CLOSURE
}reKind;

typedef struct Re {
	reKind kind;
}Re;

typedef struct Re_Char {
	reKind kind;
	char c;
}Re_Char;

typedef struct Re_Concat {
	reKind kind;
	Re *left;
	Re *right;
}Re_Concat;

typedef struct Re_Alt {
	reKind kind;
	Re *left;
	Re *right;
}Re_Alt;

typedef struct Re_Closure {
	reKind kind;
	Re *left;
}Re_Closure;

Re* Parse(char *str);
void PrintReTree(Re *t);
void FreeRe(Re *r);