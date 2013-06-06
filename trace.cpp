#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include "trace.h"
#include "gzstream.h"
#include "func.h"

vector<CFunc*>  CTrace::funcs;

CTrace::CTrace(){
	instNum = 0;
	callNum = 0;
	diffNum = 0;
}

CTrace::CTrace(string traceFile){
	instNum = readTrace(traceFile);
	diffNum = 0;
	callNum = 0;
}

unsigned CTrace::readTrace(string traceFile){
	igzstream inFile;
	string temp;
	string temp2;
	string op;
	unsigned long long pc;
	unsigned long long lastPc;
	unsigned line = 1;
	unsigned size = 1;

	inFile.open(traceFile.c_str());
	
	while(getline(inFile, temp)){
		/*cout << "#" << std::dec << line << "# " ;
		cout << temp << endl;
		
		if(line > 100){
			for( int i = 0; i < bbs.size(); i++)
				bbs[i]->print();

			exit(0);
		}
		
		if(temp.substr(0, 3) == "0xf") {
			temp = temp.substr(2, temp.size() - 2);
		}*/

		//get pc and op
		std::stringstream instline(temp);
		instline >> std::hex >> pc >> op;

		//skip "hint-taken" in the line
		if (op == "hint-taken") {
			instline >> op;
		}
		
		/*if(line >= 442619  && line < 442620) {
			cout << line << ":" << temp << endl;
			cout << "op=" << op << ",pc=" << std::hex << pc << std::dec  << endl;
			
		}*/
		
		//call instruction
		if(op == "call"){
			bbs.push_back(new CInst(line, pc, INST_CALL, size));
			size = 0;
		}
		else if(op == "syscall"){
			bbs.push_back(new CInst(line, pc, INST_CALL, size));
			size = 0;
		}
		//return instruction
		else if(op == "ret"){
			bbs.push_back(new CInst(line, pc, INST_RET, size));
			size = 0;
		}
		// sysret is a special case, we seperate one bbs into two bbs here
		else if(op == "sysret"){
			bbs.push_back(new CInst(line - 1, lastPc, INST_RET, size - 1));
			bbs.push_back(new CInst(line, pc, INST_RET, 1));
			size = 0;
		}
		//jump instruction
		else if(op[0] == 'j'){
			bbs.push_back(new CInst(line, pc, INST_JUMP, size));
			size = 0;
		}
		size++;
		line++;
		lastPc = pc;
	}
	
	inFile.close();
	return line - 1;
}

void CTrace::print(){
	for(unsigned i = 0; i < bbs.size(); i++)
		bbs[i]->print();
}

int CTrace::readDump(string file)
{
    ifstream inFile;
    string temp;
    size_t index;
    size_t index2;
    long long addr;
    unsigned int size;

    inFile.open(file.c_str());
    if (!inFile) {
        cerr << "Unable to open file: " << file << endl;
        exit(1); // terminate with error
    }

    while(getline(inFile, temp)) {
        //0000000000422340 g     F .text	00000000000000a3 _IO_old_init
        index = temp.find(".text");
        if(index != string::npos) { // found '.text' str
            index = temp.find("0", index + 1);
            index2 = temp.find(" ", index + 1);
            sscanf(temp.substr(index, index2 - index).c_str(), "%x", &size);
            if(size > 0) {
                index = temp.find(" ");
                sscanf(temp.substr(0, index).c_str(), "%llx", &addr);
                CFunc* fi = new CFunc(addr, addr + size - 1, size, temp.substr(index2 + 1));
                funcs.push_back(fi);
            }
        }
    }
    inFile.close();
    sort(funcs.begin(),funcs.end(),CFunc::compare);
    
    //cout <<funcs.size() << " functions found in dump file!" << endl;
    return 0;
}

string CTrace::getFuncName(unsigned pc){
	for(unsigned i = 0; i < funcs.size(); i++){
		if(funcs[i]->contains(pc))
			return funcs[i]->getName();
	}
	return "";
}

void CTrace::printInfo(){
	
	cout << bbs.size() << " basic blocks, ";
	cout << instNum << " instructions, ";
	cout <<instNum/bbs.size() << " insts/bbs" << endl;
	
	cout << bbUsage.size() << " unique basic blocks" << endl;	
	
	int num = 1;
	float ratio = 0.0;
	map<unsigned long long, unsigned>::iterator it;
	for(it = bbUsage.begin(); it != bbUsage.end(); it++, num++){
		ratio = it->second * 100.0 / bbs.size();
		if(ratio > 1.0){
			cout << num << ":pc=" << it->first << ", num=" << it->second << ", ratio=" << ratio << "%" << endl;
		}
	}
}

void CTrace::printAll(){
	for(unsigned i = 0; i < bbs.size(); i++)
		bbs[i]->print();	
}

void CTrace::collectUsage(){
	map<unsigned long long, unsigned>::iterator it;
	
	bbUsage.clear();
	for(unsigned i = 0; i < bbs.size(); i++){
		it = bbUsage.find(bbs[i]->pc);
		if(it == bbUsage.end()){
			bbUsage[bbs[i]->pc] = 1;
		}
		else{
			it->second++;
		}
	}
}

void CTrace::intersectTrace(CTrace* right){
	map<unsigned long long, unsigned>& rUsage = right->getBbUsage();
	vector<CInst*>& rBbs = right->getBbs();
	map<unsigned long long, unsigned>::iterator it;
	
	for(unsigned i = 0; i < bbs.size(); i++)
	{
		it = rUsage.find(bbs[i]->pc);
		if(it == rUsage.end()){
			this->AddDiff(bbs[i]);
			//bbsDiff.push_back(bbs[i]);
			bbs.erase(bbs.begin() + i);
			i--;
		}
	}
	for(unsigned i = 0; i < rBbs.size(); i++)
	{
		it = bbUsage.find(rBbs[i]->pc);
		if(it == bbUsage.end()){
			right->AddDiff(rBbs[i]);
			rBbs.erase(rBbs.begin() + i);
			i--;
		}
	}
	
	cout << "t1_bbs_size " << bbs.size() << endl;
	cout << "t2_bbs_size " << rBbs.size() << endl;
	//cout << "t2 bbs size = " << right->getBbsNum() << endl;
}

map<unsigned long long, unsigned>& CTrace::getBbUsage(){
	return bbUsage;
}

vector<CInst*>& CTrace::getBbs(){
	return bbs;
}

void CTrace::AddDiff(CInst* diff){
	bbsDiff.push_back(diff);
	diffNum += diff->size;
}

unsigned CTrace::getDiffSize(){
	//return bbsDiff.size();
	return diffNum;
}

//generate the call trace
void CTrace::getCallTrace(vector<CInst*>& t){
	for(unsigned i = 0; i < bbs.size(); i++){
		if(bbs[i]->type == INST_CALL)
			t.push_back(bbs[i]);
		else if(bbs[i]->type == INST_RET)
			t.push_back(bbs[i]);
	}		
	callNum = t.size();
}

//get partial bb trace
void CTrace::getBbsTrace(vector<CInst*>& t, unsigned startLine, unsigned endLine){
	//cout << "getBbsTrace, bbs.size= " << bbs.size() << ", start=" << startLine << ", end=" << endLine << ":";
	for(unsigned i = 0; i < bbs.size(); i++){
		//bbs[i]->print();
		if(bbs[i]->line < startLine)
			continue;
		
		if(bbs[i]->line > endLine){
		 	//cout << t.size() << endl;  
			return;
		}
		//cout << "push inst into q" << endl;
		t.push_back(bbs[i]);
	}
}

unsigned CTrace::getBbsNum(){
	return bbs.size();
}

unsigned CTrace::getInstNum(){
	return instNum;
}

unsigned CTrace::getCallNum(){
	return callNum;
}
