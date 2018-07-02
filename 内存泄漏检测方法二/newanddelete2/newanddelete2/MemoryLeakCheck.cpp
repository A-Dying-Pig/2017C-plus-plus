/*
* MemoryLeakCheck.cpp
*
* Trace类中成员函数的定义,重载new,delete的定义
*
* Author:雷怡然
*
* Date:2017/5/30
*/
#include "MemoryLeakCheck.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
Examine m_exam;

Examine::~Examine()
{
	on = false;
	compareSet();
}

void Examine::compareSet()
{
	int lengthNew = fromNew.size(),
		lengthDelete = fromDelete.size();
	if (lengthNew != lengthDelete)
	{
		reportLeak();
		return;
	}
	m_iterator i,j;
	for (i = fromNew.begin(),j = fromDelete.begin();
		i != fromNew.end(); )
	{
		if (*i != *j)
		{
			reportLeak();
			return;
		}
		i++;
		j++;
	}
	reportNotLeak();
}

void Examine::lock()
{
	locktimes++;
}

void Examine::unlock()
{
	locktimes--;
}


void Examine::reportLeak ()
{
	ofstream out("log.txt",ios::app);
	cout <<"Memory Leak Detected!"<<endl;
	out<<__DATE__<<"  "<<__TIME__<<endl<<"Memory Leak Detected!"<<endl<<endl;
}

void Examine::reportNotLeak()
{
	ofstream out("log.txt",ios::app);
	cout<<"Memory Leak Not Detected!"<<endl;
	out<<__DATE__<<"  "<<__TIME__<<endl<<"Memory Leak Not Detected!"<<endl<<endl;
}

void Examine::addSetFromNew(void* p)
{
	if (m_exam.locktimes>0)
		return;
	Examine::Lock locker(*this);
	if (p == NULL)
		return;
	stringstream stream;
	string name;
	stream<<p;
	stream>>name;
	fromNew.insert (name);
}

void Examine::addSetFromDelete (void *p)
{
	if (m_exam.locktimes>0)
		return;
	Examine::Lock locker(*this);
	if (p == NULL)
		return;
	stringstream stream;
	string name;
	stream<<p;
	stream>>name;
	fromDelete.insert (name);
}

void* operator new (size_t size)
{
	void *p = malloc (size);
	if (m_exam.on == true)
		m_exam.addSetFromNew(p);
	return p;
}

void* operator new [](size_t size)
{
	void *p = malloc(size);
	if (m_exam.on == true)
		m_exam.addSetFromNew(p);
	return p;
}

void operator delete (void* p)
{
	if (m_exam.on == true)
			m_exam.addSetFromDelete(p);
	free(p);
}

void operator delete[] (void* p)
{ 
	m_exam.addSetFromDelete(p);
	free(p);
}