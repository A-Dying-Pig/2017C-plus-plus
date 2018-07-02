#include <iostream>
#include "MemoryOutOfBounds.h"
#include "define.h"
using namespace std;

int main ()
{
	char name[20];
	name[0] = 'L';
	name[1] = 'Y';
	name[2] = 'R';
	return 0;
}