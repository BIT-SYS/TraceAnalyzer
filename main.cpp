#include <string>
#include <iostream>
#include <vector>

#include "trace.h"
#include "func.h"
#include "ed.h"

using namespace std;

int main(int argc, char** argv){
	if(argc < 6){
		cout << "USAGE: ped file1 file2 dump_file max_len thread_number" << endl;
		return 1;
	}
	string file1 = argv[1];
	string file2 = argv[2];
	string dump  = argv[3];
	unsigned maxLen = atoi(argv[4]);
	unsigned thrdNum = atoi(argv[5]);
	unsigned genEs = 0;
	
	if(argc >= 7){
		genEs = atoi(argv[6]);
	}
	
	vector<CFunc*> funcs;
		
	cout << "diff files <" << file1 << "," << file2 << "> with dump file" << dump << endl;
	//load the traces
	CTrace::readDump(dump);
	CTrace* t1 = new CTrace(file1);
	t1->collectUsage();
	CTrace* t2 = new CTrace(file2);
	t2->collectUsage();
	
	//t1->printInfo();
	//t2->printInfo();
	
	//t1->print();
	//t1->printAll();
	//performe intersection on two traces
	t1->intersectTrace(t2);
	//cout << t1->getBbsNum() << endl;
	//cout << t2->getBbsNum() << endl;

	//compare the two traces
	CComparer* c = new CComparer(maxLen, thrdNum);
	c->genEs(genEs);
	c->compare(t1, t2);
	
	c->printStat();
	
	//clean
	delete t1;
	delete t2;
	delete c;


}
