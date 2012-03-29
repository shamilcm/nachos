// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 100
char buf[BUF_SIZE];
int arg1,arg2,arg3,arg4;
static int pid1=0;
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void StartUserThread(int arg)
{
  machine->Run();
}

void updatePC(){

		// Note that we have to maintain three PC registers, 
		// namely : PCReg, NextPCReg, PrevPCReg. 
		// (See machine/machine.cc, machine/machine.h) for more details.
		int pc, nextpc, prevpc;

		// Read PCs
		prevpc = machine->ReadRegister(PrevPCReg);
		pc = machine->ReadRegister(PCReg);
		nextpc = machine->ReadRegister(NextPCReg);

		// Update PCs
		prevpc = pc;
		pc = nextpc;
		nextpc = nextpc + 4;	// PC incremented by 4 in MIPS

		// Write back PCs
		machine->WriteRegister(PrevPCReg, prevpc);
		machine->WriteRegister(PCReg, pc);
		machine->WriteRegister(NextPCReg, nextpc);
	}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    OpenFile *executable;
    AddrSpace *space;
    switch(which)
    {
        case SyscallException:
            switch(type)
            {
                case SC_Halt:
                         DEBUG('z', "Shutdown, initiated by user program.\n");
                         interrupt->Halt();
                         break;
                
		case SC_Exec:
		{
			DEBUG('a', "Exec system call invoked\n");
			arg1 = machine->ReadRegister(4);
			buf[BUF_SIZE - 1] = '\0';
			int size = 0;
			do
			{
				machine->ReadMem(arg1, sizeof(char), (int*)(buf + size));  
				arg1 += sizeof(char);
				size++;
			}while(size < ( BUF_SIZE - 1) && buf[size - 1] != '\0');
			
			executable = fileSystem->Open(buf);
			if (executable == NULL) {
				printf("Unable to open file %s\n", buf);
				machine->WriteRegister(2, -2);
				bzero(buf, BUF_SIZE);
				updatePC();
				break;
			}
			
			currentThread->space->ReleaseAddrSpace();
			space = new AddrSpace();
			space->AllocateAddrSpace(executable);
			currentThread->space = space;

			delete executable;			// close file

			space->InitRegisters();		// set the initial register values
			space->RestoreState();		// load page table register

			machine->Run();			// jump to the user progam
			ASSERT(FALSE);	
			machine->WriteRegister(2, 1);
			bzero(buf, BUF_SIZE);
			updatePC();
		}
		break;   //SC_Exec                    

		case SC_Exit:
		{

		}
		break;   //SC_Exit                    



		case SC_Fork:
		{
				DEBUG('a', "Fork, initiated by the user program.\n");
				arg1 = machine->ReadRegister(4);
				char buf[20];
				sprintf(buf, "Thread %d", pid1++);
				Thread* thread = new Thread(buf);			
				 thread->pid = pid1++;
				thread->space = currentThread->space;
				//thread->parent = currentThread;
				thread->SaveUserState();
				thread->ChangeUserReg(4,0);
				thread->ChangeUserReg(PCReg, arg1);	
				thread->ChangeUserReg(NextPCReg, arg1 + 4);
				thread->Fork((VoidFunctionPtr)StartUserThread, arg1);			
				machine->WriteRegister(2, pid1-1);
				currentThread->Yield();
				updatePC();
		}
		break;   //SC_Fork                    






        
                case SC_Print:
                {
			DEBUG('z', "Print() system call invoked \n");
			arg1 = machine->ReadRegister(4); //mem addr of string to be print
			// This address (pointer to the string to be printed) is 
			// the address that pointes to the user address space.
			// Simply trying printf("%s", (char*)addr) will not work
			// as we are now in kernel space.

			// Get the string from user space.

			int size = 0;

			buf[BUF_SIZE - 1] = '\0';               // For safety.

			do{
				// Invoke ReadMem to read the contents from user space

				machine->ReadMem(arg1,    // Location to be read
					sizeof(char),      // Size of data to be read
					(int*)(buf+size)   // where the read contents 
					);                 // are stored

				// Compute next address
				arg1+=sizeof(char);    size++;

			} while( size < (BUF_SIZE - 1) && buf[size-1] != '\0');

			size--;
			DEBUG('z', "Size of string = %d\n", size);
			printf("%s\n", buf);
			bzero(buf, sizeof(char)*BUF_SIZE);  // Zeroing the buffer.
			updatePC();
		}
		break; // SC_Print
		
		case SC_Open:
                {
			DEBUG('z', "Open() system call invoked \n");
			arg1 = machine->ReadRegister(4); //mem addr of name of file
			// This address (pointer to the string to be printed) is 
			// the address that pointes to the user address space.
			// Simply trying printf("%s", (char*)addr) will not work
			// as we are now in kernel space.

			// Get the string from user space.

			int size = 0;

			buf[BUF_SIZE - 1] = '\0';               // For safety.

			do{
				// Invoke ReadMem to read the contents from user space

				machine->ReadMem(arg1,    // Location to be read
					sizeof(char),      // Size of data to be read
					(int*)(buf+size)   // where the read contents 
					);                 // are stored

				// Compute next address
				arg1+=sizeof(char);    size++;

			} while( size < (BUF_SIZE - 1) && buf[size-1] != '\0');

			size--;
			DEBUG('z', "Size of string = %d\n", size);
			int fd=open(buf,O_CREAT | O_RDWR,0666);
			if(fd<0)
				printf("Error opening file\n");
			machine->WriteRegister(2,fd);			
			bzero(buf, sizeof(char)*BUF_SIZE);  // Zeroing the buffer.
			updatePC();
		}
		break; // SC_Open
		
		case SC_Read:
                {
			DEBUG('z', "Read() system call invoked \n");
			arg1 = machine->ReadRegister(4); //addr of string to store
			arg2 = machine->ReadRegister(5); //no of charaters to read
			arg3 = machine->ReadRegister(6); //file descriptor
			// This address (pointer to the string to be printed) is 
			// the address that pointes to the user address space.
			// Simply trying printf("%s", (char*)addr) will not work
			// as we are now in kernel space.

			// Get the string from user space.
			int sizeread=read(arg3,buf,arg2);
			if(sizeread<0)
				printf("Error reading from file");

			int size = 0;
			while( size<sizeread && size < (BUF_SIZE - 1))
			{
				// Invoke ReadMem to read the contents from user space

				machine->WriteMem(arg1,    // Location to write
					sizeof(char),      // Size of data to be write
					(int)(*(buf+size))   // where the write contents 
					);                 // are stored

				// Compute next address
				arg1+=sizeof(char);    size++;
			}
			DEBUG('z', "Size of string = %d\n", size);
			machine->WriteRegister(2,sizeread);
			bzero(buf, sizeof(char)*BUF_SIZE);  // Zeroing the buffer.
			updatePC();
		}
		break; // SC_Read		
		
		case SC_Write:
                {
			DEBUG('z', "Write() system call invoked \n");
			arg1 = machine->ReadRegister(4); //addr of string to write
			arg2 = machine->ReadRegister(5); //no of charaters to write
			arg3 = machine->ReadRegister(6); //file descriptor
			// This address (pointer to the string to be printed) is 
			// the address that pointes to the user address space.
			// Simply trying printf("%s", (char*)addr) will not work
			// as we are now in kernel space.

			// Get the string from user space.

			int size = 0;

			buf[BUF_SIZE - 1] = '\0';               // For safety.

			do{
				// Invoke ReadMem to read the contents from user space

				machine->ReadMem(arg1,    // Location to be read
					sizeof(char),      // Size of data to be read
					(int*)(buf+size)   // where the read contents 
					);                 // are stored

				// Compute next address
				arg1+=sizeof(char);    size++;

			} while( size < (BUF_SIZE - 1) && buf[size-1] != '\0');

			size--;
			DEBUG('z', "Size of string = %d\n", size);
			if(arg2>size)
				arg2=size;
			if(write(arg3,buf,arg2)<0)
				printf("Error writing to file\n");
			bzero(buf, sizeof(char)*BUF_SIZE);  // Zeroing the buffer.
			updatePC();
		}
		break; // SC_Write
		
		case SC_Close:
                {
			DEBUG('z', "Close() system call invoked \n");
			arg1 = machine->ReadRegister(4); //filedescriptor of file to be closed
			DEBUG('z', "File Descriptor = %d\n", arg1);
			if(close(arg1)<0)
				printf("Error closing file\n");
			updatePC();
		}
		break; // SC_Close

                default:
                        printf("Unknown/Unimplemented system call %d!", type);
                        ASSERT(FALSE); // Should never happen
                        break;
            } // End switch(type)
	break; // End case SyscallException

        default:
                printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
		break;
    }
}
