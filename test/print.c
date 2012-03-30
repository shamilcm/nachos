  #include "syscall.h"

int a()
{
	int i;
	Print("XABC");
	Exit(i);
}
  int test()
  {
	
	int i;
	
		Print ("forked\n");
		Fork(a);
		 i=0;	 	
		while(i++<1000);
		Print ("forked1\n");
		Exit(i);
	
  }
  void main()
  {
	int fd;    
	Print("Hello World\n");
     //fd = Exec("./test/load");
	Fork(test);
	
		Print("main\n");

	
	Halt();	// Optional. Just print stats
  }
 
