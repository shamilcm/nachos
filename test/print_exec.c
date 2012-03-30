#include "syscall.h"



void main()
{
  int fd; 
  Print("Hello World\n");
  fd = Exec("./test/load");
  Print("After exec\n");


}
