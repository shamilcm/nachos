# Copyright (c) 1992 The Regents of the University of California.
# All rights reserved.  See copyright.h for copyright notice and limitation 
# of liability and disclaimer of warranty provisions.

MAKE = make
LPR = lpr

all: 
	cd threads; $(MAKE) depend
	cd threads; $(MAKE) nachos
	cd userprog; $(MAKE) depend 
	cd userprog; $(MAKE) nachos 
	cd vm; $(MAKE) depend
	cd vm; $(MAKE) nachos 
	cd filesys; $(MAKE) depend
	cd filesys; $(MAKE) nachos 
	cd network; $(MAKE) depend
	cd network; $(MAKE) nachos
	cd bin; make all
	cd test; make all

# don't delete executables in "test" in case there is no cross-compiler
clean:
	rm -f DISK *~ */{core,nachos,DISK,*.o,swtch.s,*~} test/{*.coff} bin/{coff2flat,coff2noff,disassemble,out}

distclean: 
	rm -f threads/*.o userprog/*.o filesys/*.o network/*.o machine/*.o vm/*.o */nachos

print:
	$(LPR) Makefile* */Makefile
	$(LPR) threads/*.h threads/*.cc threads/*.s
	$(LPR) userprog/*.h userprog/*.cc 
	$(LPR) filesys/*.h filesys/*.cc
	$(LPR) network/*.h network/*.cc 
	$(LPR) machine/*.h machine/*.cc
	$(LPR) bin/noff.h bin/coff.h bin/coff2noff.c
	$(LPR) test/*.h test/*.c test/*.s 
