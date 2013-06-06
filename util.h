#ifndef PED_UTIL
#define PED_UTIL
/*定义指令类，任务类*/
#include <deque>
#include <vector>
#include <pthread.h>

#include "inst.h"

using namespace std;

enum TASK_TYPE {
    TASK_TRACE,//整个文件
    TASK_CALL,//
    TASK_JUMP,//函数过滤后按jump区分
    TASK_INST,
    TASK_INIT
};
/*并行调度单位，按段分配*/
class CTask
{
public:
    short taskType;
    long long start1;
    long long end1;
    long long start2;
    long long end2;
    long long granularity;
    unsigned index;
    vector<CInst*> insts1;
    vector<CInst*> insts2;
    vector<CInst*> insts11;
    vector<CInst*> insts22;
public:
    CTask(short type);
    void clear();
    void print();
};

/*按段分配*/
class CTaskQueue
{
private:
    pthread_mutex_t mutex;
    deque<CTask*> tasks;
public:
    CTaskQueue();
    CTask* getTask(unsigned id);
    void putTask(CTask* pTask);
    void putTaskFront(CTask* pTask);
    bool empty();
};

#define MY_DEBUG 1

#endif
