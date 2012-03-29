  #include "syscall.h"


  int test()
  {
	int i;
		Print ("forked\n");
		
		 i=0;	 	
		while(i++<1000);
		Print ("forked1\n");
		
	
  }
  void main()
  {
	int fd;    
	Print("Hello World\n");
     //fd = Exec("./test/load");
	Fork(test);
	
		Print("main\n");
Print("main1\n");
	
	Halt();	// Optional. Just print stats
  }
