#include "util.h"
#include "ed.h"
#include <cassert>

CTask::CTask(short type)
{
    taskType = type;
    start1 = 0;
    end1 = 0;
    start2 = 0;
    end2 = 0;
    granularity = 16;//default granularity
    index = 0;
}

void CTask::clear()
{
    for(unsigned i = 0; i < insts11.size(); i++) {
        if(insts11[i] != NULL) delete insts11[i];
    }
    insts11.clear();

    for(unsigned i = 0; i < insts22.size(); i++) {
        if(insts22[i] != NULL) delete insts22[i];
    }
    insts22.clear();
}

void CTask::print()
{
    cout << "task ";
    if(taskType == TASK_TRACE)
        cout << "trace";
    else if(taskType == TASK_CALL)
        cout << "call";
    else if(taskType == TASK_JUMP)
        cout << "jump";
    else if(taskType == TASK_INST)
        cout << "inst";

//if(taskType == TASK_INST && end1 - start1 >= 32768)
    cout << "task :(" << start1 << "," << end1 << ")=" << insts1.size() <<",(" << start2 << "," << end2 << ")=" << insts2.size() << endl;
}

CTaskQueue::CTaskQueue()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
}
CTask* CTaskQueue::getTask(unsigned id)
{
    CTask* result = NULL;
    pthread_mutex_lock(&mutex);
    if(tasks.size() > 0) {
        CComparer::getComparer()->setBusy(id);
        result = tasks.front();
        tasks.pop_front();
        //tasks.erase(tasks.begin());
    }
    pthread_mutex_unlock(&mutex);
    return result;
}


void CTaskQueue::putTaskFront(CTask* pTask)
{
    pthread_mutex_lock(&mutex);
    tasks.push_front(pTask);
    pthread_mutex_unlock(&mutex);
}

void CTaskQueue::putTask(CTask* pTask)
{
    pthread_mutex_lock(&mutex);
    tasks.push_back(pTask);
    pthread_mutex_unlock(&mutex);
}

bool CTaskQueue::empty()
{
    bool result = true;
    pthread_mutex_lock(&mutex);
    if(tasks.size() > 0) {
        result = false;
    }
    pthread_mutex_unlock(&mutex);

    return result;
}

