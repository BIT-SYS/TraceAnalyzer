#ifndef PED_INST
#define PED_INST

enum instType {INST_CALL, INST_RET, INST_JUMP, INST_OTHER};

class CInst{
public:
	unsigned line;
	unsigned long long pc;
	enum instType type;
	unsigned size;

public:
	CInst();
	CInst(unsigned l, unsigned long long p, enum instType t, unsigned s);
	
	void print();
};

#endif
