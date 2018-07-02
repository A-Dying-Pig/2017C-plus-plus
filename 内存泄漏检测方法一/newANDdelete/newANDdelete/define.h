/*
* define.h
*
* new的宏定义
*
* Author:雷怡然
*
* Date:2017/5/25
*/

#define new new(__FILE__, __LINE__)	
/*宏定义,把new替换成我们定义的
operator new (size_t size,char *m_file,int m_line)函数
用到了__FILE__,__LINE__宏来获取当前文件名和当前行数*/
									