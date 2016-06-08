#include "Dfa.h"
#include <iostream>
using namespace std;

void _demo01() {
	BuildDfa("a(b|c)*");
	cout << "ab -- " << Verify("ab") << endl;
	cout << "abbb -- " << Verify("abbb") << endl;
	cout << "cbbb -- " << Verify("cbbb") << endl;
	cout << "abcb -- " << Verify("abcb") << endl;
}

void _demo02() {
	BuildDfa("(a|b)((c|d)*)");
	cout << "ac -- " << Verify("ac") << endl;
	cout << "bcd -- " << Verify("bcd") << endl;
	cout << "acccddd -- " << Verify("acccddd") << endl;
	cout << "bdddda -- " << Verify("bdddda") << endl;
}

void _demo03() {
	BuildDfa("(a|b)((c|d|e|f)*)");

	cout << "ac -- " << Verify("ac") << endl;
	cout << "bcd -- " << Verify("bcd") << endl;
	cout << "acccdddeeff -- " << Verify("acccdddeeff") << endl;
	cout << "accdeffedc -- " << Verify("accdeffedc") << endl;
	cout << "abccdeffedc -- " << Verify("abccdeffedc") << endl;
	cout << "bdddda -- " << Verify("bdddda") << endl;
}

void _demo04() {
	BuildDfa("(a|b)((ca|d|e|f)*)");

	cout << "ac -- " << Verify("ac") << endl;
	cout << "aca -- " << Verify("aca") << endl;
	cout << "acac -- " << Verify("acac") << endl;
	cout << "acacadf -- " << Verify("acacadf") << endl;
	cout << "acacadfa -- " << Verify("acacadfa") << endl;
}

void _demo05() {
	BuildDfa("fee|fie");
	
	cout << "fee -- " << Verify("fee") << endl;
	cout << "fie -- " << Verify("fie") << endl;
	cout << "fiee -- " << Verify("fiee") << endl;
}

int main() {
	_demo04();
	_demo05();
	system("pause");
}