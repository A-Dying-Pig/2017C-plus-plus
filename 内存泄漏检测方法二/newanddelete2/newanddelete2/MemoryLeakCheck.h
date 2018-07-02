/*
* MemoryLeakCheck.h
*
* Examine类中成员函数的声明,重载new,delete的声明
*
* Author:雷怡然
*
* Date:2017/5/30
*/
#ifndef MEMORYLEAKCHECK_H_ 
#define MEMORYLEAKCHECK_H_


#include <set>
#include <string>
using namespace std;

class Examine
{
private:
	set<string> fromNew;
	set<string> fromDelete;
	typedef set<string>::iterator m_iterator;
	int locktimes;
public:
	bool on;
	Examine():on(true),locktimes(0){}
	~Examine();
	void compareSet ();
	void addSetFromNew (void *p);
	void addSetFromDelete(void *p);
	void reportLeak();
	void reportNotLeak();
	void lock();
	void unlock();
public:
	class Lock
	{
		Examine& _exam;
	public:
		Lock(Examine&r):_exam(r){_exam.lock();}
		~Lock(){_exam.unlock();}
	};
};

void* operator new (size_t size);
void* operator new[](size_t size);
void operator delete (void* p);
void operator delete[] (void* p); 

#endif