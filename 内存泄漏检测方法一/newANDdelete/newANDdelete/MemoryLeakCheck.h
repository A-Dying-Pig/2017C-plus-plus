 /*
* MemoryLeakCheck.h
*
* Trace类中成员函数的声明,重载new,delete的声明
*
* Author:雷怡然
*
* Date:2017/5/25
*/
#ifndef MEMORYLEAKCHECK_H_
#define MEMORYLEAKCHECK_H_

#include <map>
using namespace std;


class Trace
{
public:
	class Entry														
		//Entry类用来记录分配内存的信息
	{
	public:
		char *File;													
		//用来记录发生内存泄漏的源文件名称
		int Line;													
		//用来记录未调用delete释放内存的new所在行数
		Entry():Line(0){}											
		//默认构造函数
		Entry(char m_file[],int m_line):Line(m_line),File(m_file){}	
		//重载构造函数
		~Entry(){}													
		//析构函数
	};

	class Lock														
	/*Lock类用来避免delete对map在
	清楚数据时使用delete造成干扰*/
	{
	public:
		Lock(Trace &tracer):_trace(tracer){_trace.lock();}
		~Lock(){_trace.unlock();}
	private:
		Trace& _trace;												
		//成员变量是类Trace的引用
	};

private:
	int m_locktimes;												
	//用来判断是不是map调用的delete
	map<void*,Entry > m_map;										
	/*记录内存分配的map，键值为new分配的
	内存地址，对应的值为类Entry，储存new
	所在的文件和行数*/
	typedef map<void*,Entry>::iterator m_iterator;				   
	//重命名map<void*,Entry>迭代器的名字
public:
	int getMapSize();												
	//用来获取m_map的长度
	bool on;														
	/*用来表示Trace的对象是否存在,即
	main函数是否结束,true表示未结束,
	false表示析构函数执行，即主函数结束*/
	void lock();													
	//用来使m_locktimes++
	void unlock();													
	//用来使m_locktimes--
	void addMap (void *p,char* m_file,int m_line);					
	//用来添加map中的信息
	void deleteMap (void *p);										
	//用来删除map中的信息	
	void checkMap ();												
	/*用来检查m_map中的是否存有元素, 
	并进行汇报,如果发生内存泄漏，
	释放占用内存*/
	Trace():on(true),m_locktimes(0){}						
	/*程序开始后，会将on设为true,
	m_locktimes的值设为0。即若on==true，
	则主函数未结束*/
    ~Trace();
};


void* operator new (size_t size,char *m_file,int m_line);			
/*重载new运算符,char* m_file是文件名,int m_line是行数*/
void* operator new[] (size_t size,char *m_file,int m_line);
void* operator new (size_t size);
void operator delete (void *p);
void operator delete[] (void *p);

#endif
