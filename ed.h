/*
子任务计算
cl ed 都没有问题
*/
#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <fstream>

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <iomanip>
#include <locale>
#include <sstream>
#include "gzstream.h"

#include "util.h"
#include "trace.h"

using namespace std;

class CMatrix
{
private:
    unsigned int _row;
    unsigned int _col;
    unsigned int _size;
    unsigned char* _data;
public:
    CMatrix(unsigned int row, unsigned int col);
    ~CMatrix();

    unsigned int get(unsigned int row, unsigned int col);
    void set(unsigned int row, unsigned int col, unsigned char val);
    unsigned int row() {
        return _row;
    }
    unsigned int col() {
        return _col;
    }
};

class CTrace;

#define NUMTHREADS  32
#define OPR_NO 0
#define OPR_DEL 1
#define OPR_INS 2
#define OPR_SUB 3

class CComparer
{
private:
//    static unsigned _maxSeqLen;
    static unsigned _maxBlockSize;
    static CTrace* _pT1;
    static CTrace* _pT2;
    static CComparer* _comparer;
    static vector <unsigned> instSim;
    static vector <unsigned> callSim;
    static unsigned _genEs;

    unsigned _thrdNum;
    pthread_t threads[NUMTHREADS];
    unsigned stats[NUMTHREADS];
    unsigned ids[NUMTHREADS];
    pthread_barrier_t bar;

public:
    CComparer();
    CComparer(unsigned maxBlockSize, unsigned thrdNum);
    void compare(CTrace* t1, CTrace* t2);
    void wait();
    void setBusy(unsigned index);
    void setIdle(unsigned index);
    void startThreads();

    static void thread_func(void* tid);
    static CComparer* getComparer();
    
    static void genEs(unsigned flag);
    /*
    void calCallTrace(string file, vector<long long>* callTrace);
    void calJmpTrace(vector <long long>* jmpTrace);
    
    static void reduceCT(vector<CInst*>& insts, long long& funcSize);
    static void reduceJT(vector<CInst*>& insts, long long& funcSize);
    static void reduceIT(CTask* pTask);
	*/
    static CMatrix* calEd(CTask* pTask);
    static unsigned calEs(CMatrix* matrix, CTask* pTask);


    //task handlers
    static void processTraces(CTask* pTask);
    static void processCalls(CTask* pTask, unsigned tid);
    static void processJumps(CTask* pTask, unsigned tid);
    static void processInsts(CTask* pTask);
    static void initTraces(CTask *pTask);

    void printStat();
	static void printCalls(CTask* pTask);
    static void freeTask(CTask * pTask);
};



