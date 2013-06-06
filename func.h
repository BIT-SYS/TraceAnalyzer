#ifndef PED_FUNC
#define PED_FUNC

#include <iostream>
using namespace std;

class CFunc
{
private:
    const long long _addrs;
    const long long  _addre;
    const unsigned int _size;
    const string _name;
public:
    CFunc(long long addrs, long long addre, unsigned size, string name);
    inline long long getBase() const {
        return _addrs;
    }
    bool contains(long long addr);
    bool contains(long long addr, string &funcName);
    void print(std::ostream& os) const;
    bool operator<(const CFunc& f) const;
    static bool compare(const CFunc* a,const CFunc* b);
	string getName();
};

#endif
