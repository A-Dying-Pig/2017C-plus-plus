#include <iostream>
#include <string>
#include "MemoryLeakCheck.h"


int main ()
{
	int* a = new int (5);
	char* b = new char;
	string c("cat");
	delete b;
	return 0;
}