#include "syscall.h"


int f2()
{
  int i;
  Print("Fork f2\n");
  Exit(i);
}

int f1()
{
	int i;
	Print ("forked\n");
	Fork(f2);
	 i=0;	 	
	while(i++<1000);
	Print("Fork f1\n");
	Exit(0);
}

void main()
{
  int fd; 
  Print("Fork\n");
   Fork(f1);
  Print("After Fork\n");


}
