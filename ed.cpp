#include "ed.h"
#include "trace.h"
#include "util.h"
#include <algorithm>
#include <stack>
//#define NDEBUG //uncomment this line to disable assert
#include <cassert>
#include <limits.h>

CTaskQueue globalTaskQueue;

CMatrix::CMatrix(unsigned int row, unsigned int col)
{
    _row = row;
    _col = col;
    _size = _row * _col;
    assert(_size < 4 * 10e9);
    _data = (unsigned char*)malloc(_size);

    if(_data == NULL) {
        cerr <<"failed to malloc memory for matrix" << endl;
        exit(-1);
    }
}

CMatrix::~CMatrix()
{
    if(_data) free(_data);
}

unsigned int CMatrix::get(unsigned int row, unsigned int col)
{
    unsigned int index = row * _col + col;
    if(row < _row && col < _col) return _data[index];
    else {
        cerr <<"wrong index for matrix:" << row << "," << col << endl;
        return 0;
    }
}

void CMatrix::set(unsigned int row, unsigned int col, unsigned char val)
{
    unsigned int index = row * _col + col;
    if(row < _row && col < _col) _data[index] = val;
    else {
        cerr <<"wrong index for matrix:" << row << "," << col << endl;
    }
}


//unsigned CComparer::_maxSeqLen = 32768;
unsigned CComparer::_maxBlockSize = 32768;
CTrace* CComparer::_pT1 = NULL;
CTrace* CComparer::_pT2 = NULL;
CComparer* CComparer::_comparer = NULL;
vector <unsigned> CComparer::instSim(32, 0);
vector <unsigned> CComparer::callSim(32, 0);
unsigned CComparer::_genEs = 0;

CComparer::CComparer()
{
    _maxBlockSize = 32768;
    _thrdNum = 1;
    _comparer = this;
}

void CComparer::wait()
{
    while(true) {
        if(globalTaskQueue.empty()) {
            unsigned num = 0;
            for(unsigned i = 0; i < _thrdNum; i++) {
                //cout << stats[i] << ",";
                if(stats[i] == 0) num++;
            }
            //cout << endl;
            if(num == _thrdNum) break;
            else sleep(1);
        }
    }
}

void CComparer::setBusy(unsigned index)
{
    stats[index] = 1;
}
void CComparer::setIdle(unsigned index)
{
    stats[index] = 0;
}

CComparer* CComparer::getComparer()
{
    return _comparer;
}
void CComparer::printCalls(CTask* pTask){
	cout << "call trace for t1----------------" << endl;
	for(unsigned i = 0; i < pTask->insts1.size(); i++){
		cout << "(" << i <<"):" << pTask->insts1[i]->pc << ","  << _pT1->getFuncName(pTask->insts1[i]->pc) << endl;
	}
	cout << "call trace for t2----------------" << endl;
	for(unsigned i = 0; i < pTask->insts2.size(); i++){
		cout << "(" << i <<"):" << pTask->insts2[i]->pc << "," << _pT2->getFuncName(pTask->insts2[i]->pc) << endl;
	}
}
void CComparer::processCalls(CTask* pTask, unsigned tid)
{
    unsigned int i;
    CInst* pInst1;
    CInst* pInst2;
    //step1: get the call trace
	cout << "ProcessCalls:" ;
	pTask->print();
	
	if(pTask->insts1.size() > _maxBlockSize || pTask->insts2.size() > _maxBlockSize){
		cout << "wrong size:" << pTask->insts1.size() << "," << pTask->insts2.size() << endl;
		return;	
	}

	//printCalls(pTask);
    //step2: calculate the ed and es for call trace
    //traces->printCallTrace(pTask->insts1);
    //unsigned size1 = pTask->insts1.size();
    //unsigned size2 = pTask->insts2.size();
	
	//pTask->insts1.erase(pTask->insts1.begin(), pTask->insts1.begin() + 17);
	//pTask->insts2.erase(pTask->insts2.begin(), pTask->insts2.begin() + 42);

    for(i = 0; i < pTask->insts1.size(); i++){
        pTask->insts11.push_back(pTask->insts1[i]);
		//pTask->insts1[i]->print();
	}
	//cout <<"----------------------" << endl;
    for(i = 0; i < pTask->insts2.size(); i++){
		pTask->insts22.push_back(pTask->insts2[i]);
		//pTask->insts2[i]->print();
	}
	//cout <<"---------------------" << endl;
    CMatrix* pMatrix = calEd(pTask);
    calEs(pMatrix, pTask);
    delete pMatrix;
	//printCalls(pTask);	
	/*cout << "insts1 size = " << pTask->insts1.size() << endl;
	cout << "insts2 size = " << pTask->insts2.size() << endl;
	for(i = 0; i < pTask->insts1.size(); i++){
		pTask->insts1[i]->print();
		pTask->insts2[i]->print();
	}
	exit(0);*/
     //cout << "new call trace size:" << pTask->insts1.size() << endl;
     //cout << "new call trace size:" << pTask->insts2.size() << endl;
	 callSim[tid] += pTask->insts2.size();
    
    //step3: generate new tasks
    long long line1 = 0;
    long long line2 = 0;
    for(i = 0; i < pTask->insts1.size(); i++) {
        pInst1 = pTask->insts1[i];
        pInst2 = pTask->insts2[i];

        //if(pInst1->line - line1 <= _maxBlockSize && pInst2->line - line2 <= _maxBlockSize) {
            CTask* pTemp = new CTask(TASK_JUMP);
            pTemp->start1 = line1;
            pTemp->end1 = pInst1->line;
            pTemp->start2 = line2;
            pTemp->end2 = pInst2->line;
            pTemp->index = pTask->index;
            //globalTaskQueue.putTask(pTemp);
            //_pTraces->getInstAndPut(pTemp);
			_pT1->getBbsTrace(pTemp->insts1, pTemp->start1, pTemp->end1);
			_pT2->getBbsTrace(pTemp->insts2, pTemp->start2, pTemp->end2);
			//cout << "new task:";
            //pTemp->print();			
			globalTaskQueue.putTask(pTemp);
        //} else {
		//	cout << "wrong size " << pInst1->line - line1 << endl;
			/*
            CTask* pTemp = new CTask(TASK_JUMP);
            pTemp->start1 = line1;
            pTemp->end1 = pInst1->line;
            pTemp->start2 = line2;
            pTemp->end2 = pInst2->line;
            pTemp->index = pTask->index;
            globalTaskQueue.putTask(pTemp);*/
        //}
        line1 = pInst1->line + 1;
        line2 = pInst2->line + 1;
    }
    //TODO: the last block in the program
    unsigned total1 = _pT1->getInstNum();
    unsigned total2 = _pT2->getInstNum();
    if(line1 < total1 && line2 < total2) {
        CTask* pTemp = new CTask(TASK_JUMP);
        pTemp->start1 = line1;
        pTemp->end1 = total1;
        pTemp->start2 = line2;
        pTemp->end2 = total2;
        pTemp->index = pTask->index;
        //cout << "New inst task:";
        //pTemp->print();
        //_pTraces->getInstAndPut(pTemp);
		_pT1->getBbsTrace(pTemp->insts1, pTemp->start1, pTemp->end1);
		_pT2->getBbsTrace(pTemp->insts2, pTemp->start2, pTemp->end2);
		globalTaskQueue.putTask(pTemp);
    }

    delete pTask;
}


void CComparer::processJumps(CTask* pTask, unsigned tid)
{
    //step1: get the jump trace
    unsigned i;
    //assert(pTask->insts1.size() <= _maxBlockSize);
    //assert(pTask->insts2.size() <= _maxBlockSize);
 	//cout << "ProcessJumps:";
	//pTask->print();
	if(pTask->insts1.size() > _maxBlockSize || pTask->insts2.size() > _maxBlockSize){
		cout << "wrong size:" << pTask->insts1.size() << "," << pTask->insts2.size() << endl;
		return;	
	}
    //调试注释
    //step2: calculate the ed and es for call trace
    //traces->printCallTrace(pTask->insts1);
    unsigned size1 = pTask->insts1.size();
    unsigned size2 = pTask->insts2.size();
	//cout << "size1 = " << size1 << endl;
	//cout << "size2 = " << size2 << endl;

	if(size1 == 0 && size2 == 0) return;
    //assert(size1 > 0 && size2 > 0);
	if(size1 == 0){
		//for(i = 0; i < size2; i++)
		//	instSim[tid] += pTask->insts2[i]->size;
		return;
	}
	else if(size2 == 0){
		//for(i = 0; i < size1; i++)
		//	instSim[tid] += pTask->insts1[i]->size;
		return;
	}

    for(i = 0; i < pTask->insts1.size(); i++)
        pTask->insts11.push_back(pTask->insts1[i]);
    for(i = 0; i < pTask->insts2.size(); i++)
        pTask->insts22.push_back(pTask->insts2[i]);

    CMatrix* pMatrix = calEd(pTask);
    calEs(pMatrix, pTask);
    delete pMatrix;


	for(i = 0; i < pTask->insts2.size(); i++){
    	//instSim[tid] += pTask->insts2[i]->size;
		if(pTask->insts1[i]->size > pTask->insts2[i]->size){
			instSim[tid] += pTask->insts2[i]->size;
			//cout << "size error!" << std::hex << pTask->insts1[i]->pc << "," <<  pTask->insts2[i]->pc << std::dec << endl;
			//cout << pTask->insts1[i]->size << "," << pTask->insts2[i]->size << endl;
		}
		else {
			instSim[tid] += pTask->insts1[i]->size;
		}
	}

	//cout << "instSim[" << tid << "]=" << instSim[tid] << endl;
    //cout << "new inst trace size:" << pTask->insts1.size() << endl;
    //cout << "new inst trace size:" << pTask->insts2.size() << endl;
    /*if(size1 != pTask->insts1.size() || size2 != pTask->insts2.size())
        exit(0);
*/
    freeTask(pTask);
    //delete pTask;
}

void CComparer::freeTask(CTask * pTask)
{
    if(pTask == NULL) return;

#if MY_DEBUG
    // std::cout << "freeing task!" << std::endl;
#endif
    pTask->insts1.clear();
    pTask->insts2.clear();
    delete pTask;
}

void CComparer::thread_func(void * tid)
{
    unsigned int id = *((int*)tid);
    //int i;
    while(true) {
        CTask* pTask = globalTaskQueue.getTask(id);
        if(pTask == NULL) {
            //sleep(0);
            //getComparer()->setIdle(id);
            //assert(_pTraces != NULL);
            //_pTraces->getInstAndPut(NULL);//why doing this??
            pTask = globalTaskQueue.getTask(id);
            if(pTask == NULL) {
                sleep(0);
                getComparer()->setIdle(id);
            } else {
                goto PROC_TASK;
            }
        } else {
PROC_TASK:
            switch(pTask->taskType) {
            /*case TASK_TRACE:
                processTraces(pTask);
                break;*/
            case TASK_CALL:
                processCalls(pTask, id);
                break;
            case TASK_JUMP:
                processJumps(pTask, id);
                break;
            /*case TASK_INST:
                processInsts(pTask);
                break;
            case TASK_INIT:
                initTraces(pTask);*/
            default:
                cerr << "unknown tasktype: " << pTask->taskType << endl;
                exit(-1);
            }
            //getComparer()->setBusy(id);
            //cout <<"Thread " << id << " get one task from the queue" << endl;
            pTask = NULL;
        }
    }
    return;
}


CComparer::CComparer(unsigned maxBlockSize, unsigned thrdNum)
{
    _maxBlockSize = maxBlockSize;
    _thrdNum = thrdNum;
    _comparer = this;

}

void CComparer::startThreads()
{
    for(unsigned int i = 0; i < _thrdNum; i++) {
        ids[i] = i;
        stats[i] = 0;
        asm __volatile__("": : :"memory");
        pthread_create(&threads[i], NULL, (void * (*)(void *))(thread_func), (void*)(&ids[i]));
    }

}


void CComparer::compare(CTrace* t1, CTrace* t2)
{
    _pT1 = t1;
    _pT2 = t2;
    vector <CTask*> newTasks;
    CTask * pTask;

    
	pTask = new CTask(TASK_CALL);
	_pT1->getCallTrace(pTask->insts1);
	_pT2->getCallTrace(pTask->insts2);
	
	if(pTask->insts1.size() > _maxBlockSize ||
	   pTask->insts2.size() > _maxBlockSize){
		cout << "call trace size error: " << pTask->insts1.size() << ", " << pTask->insts2.size() << endl;
		exit(1);
	}
	cout << "call_trace_1_size = " << pTask->insts1.size() << endl;
	cout << "call_trace_2_size = " << pTask->insts2.size() << endl;

	pTask->start1 = 0;
	pTask->end1 = pTask->insts1.size();
	pTask->start2 = 0;
	pTask->end2 = pTask->insts2.size();
	
	globalTaskQueue.putTask(pTask);
	startThreads();
    wait();
}

/*
  确定seg fault原因：内存占用过大导致
*/
CMatrix* CComparer::calEd(CTask* pTask)
{
    unsigned int len1;
    unsigned int len2;
    unsigned int len3;
    unsigned int min;
    vector <unsigned int>* row1;
    vector <unsigned int>* row2;
    vector <unsigned int>* temp;
    unsigned char copr = OPR_NO;
    //vector <unsigned char>* opr;

    //unsigned int ed2 = 0 ;
    CMatrix* pmatrix = new CMatrix(pTask->insts1.size(), pTask->insts2.size());
    row1 = new vector<unsigned int>();
    row2 = new vector<unsigned int>();
    for(unsigned i = 0; i <= pTask->insts2.size(); i++) {
        row1->push_back(i);
        row2->push_back(0);
    }
    for(unsigned i = 1; i <= pTask->insts1.size(); i++) {
        (*row2)[0] = i;
        for(unsigned j = 1; j <= pTask->insts2.size(); j++) {
            min = UINT_MAX;
			//copr = OPR_SUB;

			if(pTask->insts2[j-1]->pc == pTask->insts1[i-1]->pc){
                len3  = (*row1)[j-1];
				//cout << std::hex << pTask->insts1[i-1]->pc << "==" << pTask->insts2[j-1]->pc << std::dec << endl;
				//copr = OPR_NO;
			}
			else{
				len3 = (*row1)[j-1] + 2;
				//copr = OPR_SUB;
			}
		

            //else{
            //    min = (*row1)[j-1] + 1;
				//copr = OPR_SUB;

           		len1 = (*row2)[j-1] +1;
            	len2 = (*row1)[j] + 1;
            	//len3 = (*row1)[j-1] + 1;

            	if(len1 < min) {
                	min = len1;
                	copr=OPR_DEL;
					//cout << "DEL" << endl;
            	}
            	if(len2 < min) {
                	min = len2;
                	copr=OPR_INS;
					//cout << "INS" << endl;
            	}
            	if(len3 < min) {
                	min = len3;
					if(pTask->insts2[j-1]->pc == pTask->insts1[i-1]->pc){
						//cout << "==" << std::hex << pTask->insts2[j-1]->pc << std::dec<< endl;
						copr = OPR_NO;
					}
					else{
						copr = OPR_SUB;
					}
                	//copr=OPR_SUB;
					//cout << "SUB" << endl;
            	}
			//}
			/*
			if(copr == OPR_NO) cout << "ED_NO" << endl;
			else if(copr == OPR_DEL) cout << "ED_DEL" << endl;
			else if(copr == OPR_INS) cout << "ED_INS" << endl;
			else if(copr == OPR_SUB) cout << "ED_SUB" << endl;
			*/
			//cout << "min = " << min << endl;
            //if(flag) pm->set(i-1, j-1, (unsigned char)copr);
            pmatrix->set(i-1, j-1, (unsigned char)copr);
            (*row2)[j] = min;
        }
        temp = row1;
        row1 = row2;
        row2 = temp;
    }

    //cout << "ED=" << (*row1)[row1->size()-1] << endl;

    if(row1) delete row1;
    if(row2) delete row2;
    //return ed2;
    return pmatrix;
}
unsigned  CComparer::calEs(CMatrix* pm, CTask* pTask)
{
	unsigned es = 0;

    if(pm->row() <= 0) return es;

    int i = pm->row() - 1;
    int j = pm->col() - 1;
    unsigned int cvalue;

    while (i >= 0 && j >= 0) {
        cvalue = pm->get(i,j);//(*( matrix[i]))[j];
        if(cvalue == OPR_NO) {
            i--;
            j--;
            //cout <<"OPR_NO"<< endl;
        } else if(cvalue == OPR_DEL) { //delete
            //line2->erase(line2->begin() + j);
			es += pTask->insts2[j]->size;
            pTask->insts2.erase(pTask->insts2.begin() + j);
            j--;
            //cout <<"DEL" <<endl;
        } else if(cvalue == OPR_INS) { //insert
            //line1->erase(line1->begin() + i);
			es += pTask->insts1[i]->size;
            pTask->insts1.erase(pTask->insts1.begin() + i);
            i--;
            //cout <<"INS" << endl;
        } else if(cvalue == OPR_SUB) {
            //line1->erase(line1->begin() + i);
            //line2->erase(line2->begin() + j);
			es += pTask->insts1[i]->size;
			es += pTask->insts2[j]->size;

            pTask->insts1.erase(pTask->insts1.begin() + i);
            pTask->insts2.erase(pTask->insts2.begin() + j);
            i--;
            j--;
            //cout <<"SUB"<<endl;
        }else{
			cout <<"UNKNOWN OPR" << endl;
		}
    }
    //if(i >= 0)  line1->erase(line1->begin(), line1->begin() + i + 1);
    //if(j >= 0)  line2->erase(line2->begin(), line2->begin() + j + 1);
    if(i >= 0){
		for(int k = 0; k < i; k++){
			es += pTask->insts1[k]->size;
		}
	   	pTask->insts1.erase(pTask->insts1.begin(), pTask->insts1.begin() + i + 1);
	}
    if(j >= 0){
		for(int k = 0; k < j; k++){
			es += pTask->insts2[k]->size;
		}
	  	pTask->insts2.erase(pTask->insts2.begin(), pTask->insts2.begin() + j + 1);
	}
	return es;
}

void CComparer::printStat()
{
	unsigned callSimTotal = 0;
	unsigned instSimTotal = 0;
	
	cout << "t1_inst_size " << _pT1->getInstNum() << endl;
	cout << "t2_inst_size " << _pT2->getInstNum() << endl;

    cout << "call trace diff:" << endl;
    for(unsigned  int i = 0; i < callSim.size(); i++) {
        cout << callSim[i] << "," ;
        callSimTotal += callSim[i];
    }
    cout << endl;

    cout << "inst trace diff:" << endl;
    for(unsigned int i = 0; i < instSim.size(); i++) {
        cout << instSim[i] << ",";
        instSimTotal += instSim[i];
    }
    cout << endl;
    
	cout << "t1_unique_size " << _pT1->getDiffSize() <<endl;
	cout << "t2_unique_size " << _pT2->getDiffSize() <<endl;

    cout << "call_trace_sim_size " << callSimTotal << endl;
    cout << "inst_trace_sim_size " << instSimTotal << endl;

	cout << "call_trace_similarity " << callSimTotal * 100.0 / _pT1->getCallNum() << endl;
	cout << "inst_trace_similarity " << instSimTotal * 100.0 / _pT1->getInstNum() << endl;
}

void CComparer::genEs(unsigned flag)
{
	_genEs = flag;
}

