#ifndef PED_TRACE
#define PED_TRACE

#include <vector>
#include <map>

#include "func.h"
#include "inst.h"

using namespace std;


class CTrace{
private:
	//total lines
	unsigned instNum;
	//basic blocks
	vector<CInst*> bbs;
	//basic blocks diff
	vector<CInst*> bbsDiff;
	//functions
	static vector<CFunc*> funcs;
	//basic block usage
	map<unsigned long long, unsigned> bbUsage;
	//number of instructions in bbsDiff
	unsigned diffNum;
	//number of calls
	unsigned callNum;
public:
	CTrace();
	CTrace(string traceFile);
public:
	//read the content of trace file
	unsigned readTrace(string traceFile);	
	//read the content of dump file;
	static int readDump(string file);
	//the usage of basic blocks
	void collectUsage();
	
	//intersect the two traces
	void intersectTrace(CTrace* right); 
	
	//generate the call trace
	void getCallTrace(vector<CInst*>& t);
	
	//get partial bb trace
	void getBbsTrace(vector<CInst*>& t, unsigned startLine, unsigned endLine);

	//get the total number of bb
	unsigned getBbsNum();
	unsigned getInstNum();
	unsigned getCallNum();
public:
	void printInfo();
	void printAll();
	void print();
public:
	map<unsigned long long, unsigned>& getBbUsage();
	vector<CInst*>& getBbs();
	void AddDiff(CInst* diff);
	unsigned getDiffSize();
	string getFuncName(unsigned pc);
};

#endif

