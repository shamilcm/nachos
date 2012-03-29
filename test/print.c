  #include "syscall.h"
  void main()
  {
	int fd;    
	Print("Hello World\n");
     fd = Exec("./test/load");
    Halt();	// Optional. Just print stats
  }
