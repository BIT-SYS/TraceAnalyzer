#include <iostream>

#include "inst.h"

using namespace std;

CInst::CInst(){
	line = 0;
	pc = 0;
	type = INST_OTHER;
	size = 0;
}

CInst::CInst(unsigned l, unsigned long long p, enum instType t, unsigned s){
	line = l;
	pc = p;
	type = t;
	size = s;
}

void CInst::print(){
	cout << line << "," << std::hex <<  pc << std::dec << "," << size << ",";
	if(type == INST_CALL) cout << "call";
	else if(type == INST_RET) cout << "ret";
	else if(type == INST_JUMP) cout << "jump";
	else cout << "other";
	
	cout << endl;
}
