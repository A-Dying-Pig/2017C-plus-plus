/*
* MemoryOutOfBounds.h
*
* WriteCheck类成员函数的声明以及new重载声明
*
* Author:雷怡然
*
* Date:2017/5/27
*/
#ifndef MEMORYOUTOFBOUNDS_H_
#define MEMORYOUTOFBOUNDS_H_

#include <map>
using namespace std;

class WriteCheck
{
public:
	class Entry										 
	//Entry类用来记录分配内存的信息
	{
	public:
		char *File;									 
		//用来记录发生内存泄漏的源文件名称
		int Line;									  
		//用来记录发生写越界对应的new所在行数
		int Length;									  
		//用来记录new参数size_t的大小
		Entry():Line(0),Length(0){}					  
		//默认构造函数
		Entry(char m_file[],int m_line,int m_length)
			:Line(m_line),File(m_file),Length(m_length){}		
		//重载构造函数
		~Entry(){}									  
		//析构函数
	};
private:											
	map<void*,Entry > m_map;						 
	//记录内存分配的map，键值为new分配的内存地址，对应的值为类Entry//
	typedef map<void*,Entry>::iterator m_iterator;	 
	//重命名map<void*,Entry>迭代器的名字，方便使用
public:
	bool on;										 
	/*用来表示WriteOut类的全局对象是否存在,true表示未结束,
	false表示析构函数执行，即主函数结束*/
	void addMap (void *p,char* m_file,int m_line,int m_length);	
	//用来添加map中的信息
	void checkMap ();								 
	/*用来检查map中的Entry元素,如果初始值被修改，
	则发生写越界，否则没有，并进行汇报*/
	WriteCheck():on(true){}						   
	//程序开始后，会将on设为true。
    ~WriteCheck();
};

			
void* operator new[] (size_t size,char *m_file,int m_line);	
 //operator new[]函数,char* m_file是文件名,int m_line是行数
void* operator new (size_t size,char *m_file,int m_line);
//operator new函数只是形式上重载以符合宏定义
#define EXTRALENGTH 100				  
//多分配内存的长度


#endif