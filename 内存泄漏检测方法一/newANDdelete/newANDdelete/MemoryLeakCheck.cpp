/*
* MemoryLeakCheck.cpp
*
* Trace类中成员函数的定义,重载new,delete的定义
*
* Author:雷怡然
*
* Date:2017/5/25
*/

#include "MemoryLeakCheck.h"
#include <fstream>								
//将检测的结果输入到了文件log.txt中
#include <iostream>


Trace m_trace;									
//类m_trace的全局实例对象

Trace::~Trace()									
//类Trace的析构函数
{
	checkMap();					                
//在析构函数执行的时候调用checkMap()函数，检查是否发生内存泄漏
	if (on==true)				                
//当析构函数执行的时候，即主函数结束的时候，将on设为false
		on=false;
}

void Trace::lock ()								
/*类Lock的构造函数执行的函数lock函数。即Lock类建立时,
m_locktimes会加1*/
{
	m_locktimes++;
}

void Trace::unlock ()				          
/*类Lock的析构函数执行的函数unlock函数。即Lock类被析构时
,m_locktimes会减1*/
{
	m_locktimes--;
}


void Trace::addMap(void *p,char *m_file,int m_line)	
//向数据结构map中添加新的数据
{  
	    if(m_locktimes>0)						
//正常情况下，一开始由构造函数 初始化使得m_locktimes==0
			return;
		Trace::Lock locker(*this);			   
/*如果陷入了迭代循环,每次迭代会使m_locktimes++，那么上面的
如果(m-locktimes>0)为真，则表明陷入了迭代，为避免程序崩溃，
直接return*/
		m_map[p] = Entry (m_file,m_line);
}

void Trace::deleteMap(void *p)
{
	if (m_locktimes>0)							
/*我们在抹去m_map中的数据时，erase函数会调用delete来删除数据，
那么就可能产生一个死循环的迭代。*/
		return ;
	Trace:: Lock locker(*this);			        
/*当用户使用delete函数时，通过if判断(没有直接return)执行
deleteMap函数，此时m_locktimes==0,但是但此处使得m_locktimes==1，
即下一次迭代会退出*/
	cout<<"delete memory,"<<m_map[p].File<<",line "
		<<m_map[p].Line<<endl;
	m_iterator temp= m_map.find(p);
	m_map.erase(temp);						    
/*然后调用erase函数，erase函数会再次调用delete函数，但如果
再次进入deleteMap函数，由于m_locktimes==1会直接返回,然后到
上一层执行free函数，然后完成erase函数，然后再返回原来的
delete函数，完成内存空间的释放*/
}

void Trace::checkMap()
{
	void *temp=NULL;
	ofstream out("log.txt",ofstream::app);		
//建立文件输出流，将每次内存泄漏的检测结果记录到文件log.txt中
	out<<__DATE__<<"\t"<<__TIME__<<endl;		
//输出程序运行的具体时间
	if (m_map.size())						    
/*在程序结束时，如果m_map没有清空，那么说明有用户分配的
内存空间没有被释放，即发生了内存泄漏*/
	{
		cout<<"Memory Leak Detected!"<<endl;
		out<<"Memory Leak Detected!"<<endl;
		for(m_iterator it =m_map.begin();it!=m_map.end();
			it=m_map.begin())	
/*检测到程序发生了内存泄漏，则自动释放用户忘记释放的内存*/
		{
			cout<<"file:"<<it->second.File<<",line:"
				<<it->second.Line<<endl;
			out<<"file:"<<it->second.File<<",line:"
				<<it->second.Line<<endl;
			temp=it->first;															
			deleteMap(temp);					
			//删除m_map中数据
			free(temp);			                
			//删除用户分配的空间
		}
		cout<<"Leak memory released!"<<endl;
		out<<"Leak memory released!"<<endl;
	}
	else								        
		//如果m_map为空，那么说明没有发生内存泄漏
		{
			cout<<"Memory Leak Not Detected!"<<endl;
			out<<"Memory Leak Not Detected!"<<endl;
		}
	out<<endl;
}

int Trace::getMapSize()
{
	return m_map.size();					   
	/*为了数据安全，将m_map类型设为private，
	用public的getMapSize函数来获取m_map的长度*/
}

void* operator new (size_t size,char *m_file,int m_line)
{
	void* p = malloc (size);
	if (m_trace.on==true)				       
	/*on==true的时候，表示主函数正在运行；
	如果on变为false，则跳过if中的语句。
	（可能程序在main之后还会调用new，这时需
	要使new恢复原来的样子）*/
		m_trace.addMap(p,m_file,m_line);		
	//在m_map中插入数据
	return p;
}

void* operator new[] (size_t size,char *m_file,int m_line)		 
	//数组版的new
{
	void* p = malloc (size);
	if (m_trace.on==true)
		m_trace.addMap(p,m_file,m_line);
	return p;
}

void operator delete (void *p)																
{
	if (m_trace.on==true&&m_trace.getMapSize())		
		/*当m_map的长度为0时，不用调用我们自己
		的deleteMap函数。程序可能会在main函数结
		束后调用delete,这种情况m_map中没有存有
		相关的信息，因此不能执行deleteMap函数。
		同时，这样的条件判断又可以保证用户分配的内
		存都可以被执行到*/
		m_trace.deleteMap (p);
	free(p);
}
 
void operator delete[] (void *p)				  
	//delete数组
{
	if (m_trace.on==true&&m_trace.getMapSize())
		m_trace.deleteMap(p);
	free(p);
}

void* operator new (size_t size)
//一些容器会调用这个new（即使new重载也不会调用重载版本的new）
{
	void* p = malloc (size);
	if (m_trace.on==true)			
		m_trace.addMap(p,"?",0);		
	return p;
}

