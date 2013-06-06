#include "func.h"

CFunc::CFunc(long long addrs, long long addre, unsigned size, string name)
    :_addrs(addrs),_addre(addre),_size(size),_name(name)
{
}

bool CFunc::contains(long long addr)
{
    if(addr >= _addrs && addr <= _addre) return true;
    else return false;
}

bool CFunc::contains(long long addr, string &funcName)
{
    if (contains(addr)) {
        funcName = _name;
        return true;
    } else {
        return false;
    }
}

bool CFunc::compare(const CFunc *a,const CFunc *b)
{
    return a->getBase() < b->getBase();
}

void CFunc::print(std::ostream& ofs) const
{
    ofs << "function name: " << _name << "\t"
        << "base: " << std::hex << _addrs << "\t"
        << "end: " << std::hex << _addre << "\t"
        << "size: " << _size << std::endl;
}

inline bool CFunc::operator<(const CFunc& f) const
{
    return _addrs < f.getBase();
}

string CFunc::getName(){
	return _name;
}
