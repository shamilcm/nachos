  #include "syscall.h"

  void main()
  {
 	int fd,rd;
	char *p = "FILE CONTENT";
	char buff[100];	
	fd = Open("newfile.txt");
	Write(p,32,fd);
	Close(fd);
	fd = Open("newfile.txt");
	rd = Read(buff,12,fd);	
	Print(buff);
	Close(fd);
	Delete("newfile.txt");
	  Halt();	// Optional. Just print stats
  }



