/*
* MemoryOutOfBounds.cpp
*
* WriteCheck类成员函数的定义以及new重载定义
*
* Author:雷怡然
*
* Date:2017/5/27
*/
#include "MemoryOutOfBounds.h"
#include <iostream>
#include <fstream>

WriteCheck m_writecheck;									
//类WriteCheck的全局对象

WriteCheck::~WriteCheck()				
{
	checkMap();												
	//执行m_map的检查
	if (on==true)	
		on=false;											
	//主函数结束，置为false
}

void WriteCheck::addMap(void *p,char* m_file,int m_line,int m_length)
{
	m_map[p] = Entry(m_file,m_line,m_length);				
	//向m_map中添加信息
	unsigned char* temp =(unsigned char *)p;				
	//多分配的内存用unsigned char来初始化一个 unsigned char变量占1字节//
	int start = m_length / sizeof (unsigned char),			
	//计算多分配的内存转换成unsigned char后的下标范围//
	end = (m_length+EXTRALENGTH) / sizeof (unsigned char);
	for (int i=start;i<end;i++)		  //初始化多分配的内存，全部置为0
		temp[i] = 0;
}


void WriteCheck::checkMap()
{
	ofstream out("LOG.txt",ofstream::app);	  //输出文件流
	unsigned char *temp=NULL;								
	//用来将void *转换为unsigned char*
	bool first = false;						  //一个输出控制开关
	int start=0,end=0,i=0;
	for (m_iterator it = m_map.begin();it!=m_map.end();it++)
	{
		temp = (unsigned char *)it->first;					
		//获取m_map中Entry存储的内存地址
		start = it->second.Length / sizeof(unsigned char);  
		//计算下标范围
		end = ( it->second.Length + EXTRALENGTH )
			   /sizeof (unsigned char);
		for (i = start;i<end;i++ )			  //检查初始值是否改变
			if (temp[i]!=0)
				break;
		//输出部分
		if (i!= end)
		{
			if (first==false)
				{
					cout<<"Write memory out of bounds DETECTED!"<<endl;
					out<<__DATE__<<"\t"<<__TIME__<<endl<<
						"Write memory out of bounds DETECTED!"<<endl;
					first = true;
				}
			cout<<"file:"<<it->second.File<<"\tline:"
				<<it->second.Line<<endl;
			out<<"file:"<<it->second.File<<"\tline:"
				<<it->second.Line<<endl;
		}
	}
	if (first==false)
	{
		cout<<"Write memory out of bounds NOT DETECTED!"<<endl;
		out<<__DATE__<<"\t"<<__TIME__<<endl<<
			"Write memory out of bounds NOT DETECTED!"<<endl;
	}
	out<<endl;
}


void* operator new[] (size_t size,char *m_file,int m_line)
{
	void* p = malloc (size+EXTRALENGTH);				 
	//多分配EXTRALENGTH字节的长度的内存
	if (m_writecheck.on==true)							 
	//如果main函数没有结束
		m_writecheck.addMap(p,m_file,m_line,size);
	return p;
}

void* operator new (size_t size,char* m_file,int m_line) 
	//形式上的重载为了符合define宏定义
{
	void *p = malloc(size);
	return p;
}