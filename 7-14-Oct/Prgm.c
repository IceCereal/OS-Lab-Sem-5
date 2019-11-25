/*
	Lab 7: Simulating Virtual Memory: Page tables

	Task 1: Simulating a Page Table. Look at the following code from Chapter 18 of the textbook.
	This code essentially shows how a page-table entry is processed. Assume that a process virtual address space is 16-bits and the RAM is 16MB. 
	Assume that a process generates a sequence of virtual address, you need to translate them into physical addresses. 

	For this purpose, you need to simulate/implement the following:
		a. A random/deterministic sequence of virtual addresses in the valid format
		b. Page table entries: assume that we implement Valid, Present, Protect-bits (3-bits Read-write-execute), dirty bit, User/kernel mode bit
			Hint: Use a struct to implement these
		c. Page table functions to extract VPN and so on, and exception functions like SEGMENTATION_FAULT/PROTECTION_FAULT, RaiseException and so on.
			Hint: Just print the words "SEGMENTATION_FAULT" or whichever and exit the experiment trial
		d. A timer function to measure the time taken to access
			---a valid page
			---an invalid page
			---a valid page but protection bits do not allow access

		Assume page size is 1kB, i.e., 1024 Bytes or 2^10 Bytes, which gives a 10-bit offset.

	Refer to Section 18.3 to get more help. 
	The output should be a sequence of virtual address, with the correct physical address and the time taken
	for translation and the total memory access time (summation).

	1 //Extract the VPN from the virtual address
	2 VPN = (VirtualAddress & VPN_MASK) >> SHIFT
	3
	4 // Form the address of the page-table entry (PTE)
	5 PTEAddr = PTBR + (VPN * sizeof(PTE))
	6
	7 // Fetch the PTE
	8 PTE = AccessMemory(PTEAddr)
	9
	10 // Check if process can access the page
	11 if (PTE.Valid == False)
	12 RaiseException(SEGMENTATION_FAULT)
	13 else if (CanAccess(PTE.ProtectBits) == False)
	14 RaiseException(PROTECTION_FAULT)
	15 else
	16 // Access is OK: form physical address and fetch it
	17 offset = VirtualAddress & OFFSET_MASK
	18 PhysAddr = (PTE.PFN << PFN_SHIFT) | offset
	19 Register = AccessMemory(PhysAddr)

	Notes:
	1. Using time.h library and functions like timeoftheday, gettime etc can be quite useful for measurement.
	2. Functions like, sleep, rand are useful.
	3. Use the code you wrote for random number generation to simulate the virtual addresses generated.
	4. Use the bit level operations in C to do the shifting and masking operations.
	5. Please note that the sizes of the virtual and physical addresses must be conforming to the specification.

*/

/*
	Calculations:
		16 bit addr space [For each process, given]
		
		2^10 bytes per page (given)

		2^16 / 2^10 = 2^6 = 64 pages per process -- (1)

		----

		16 * 2^20 bytes for RAM -> 2^24 bytes (given)

		Since each process can take 2^16 bytes of space, we can have 2^24 / 2^16 processes = 2^8 processes.

		Each process can have 64 pages (from (1)).
		
		Therefore, there are 2^8 * 64 pages = 2^14 pages in total -- (2)
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/*
	This maps a 6-bit Virtual Page Number (requested by process) to a
	14-bit Physical Frame Number. This PFN added to the offset will give
	the Physical Address
*/

struct PageTableEntry{
	unsigned int pfn : 14;	// Physical Frame Number
	unsigned int offset : 10; // offset
	unsigned int valid : 1;	// Valid
	unsigned int present : 1;	// Present
	unsigned int protect : 3;	// Protect 01R, 10W, 11X
	unsigned int dirty : 1;	// Dirty
	unsigned int supervisor : 1;	// User/Kernel
};

#define PAGES 64 // This comes from Calculations - (1)

int initPageTable(struct PageTableEntry *PTE){

	srand(time(NULL));
	PTE[0].pfn = rand() % 16384;
	PTE[0].valid = rand() % 2;
	PTE[0].present = rand() % 2;
	PTE[0].protect = rand() % 4;
	PTE[0].dirty = rand() % 2;
	PTE[0].supervisor = rand() % 2;

	for (int i = 1; i < PAGES; ++i){
		srand(time(NULL));

		int validPFN_Flag = 0;

		while (!validPFN_Flag){
			PTE[i].pfn = rand() % 16384;

			for (int j = 0; j < i; ++j){
				if (PTE[i].pfn == PTE[j].pfn){
					validPFN_Flag = 0;
					break;
				} else{
					validPFN_Flag = 1;
					PTE[i].valid = rand() % 2;
					PTE[i].present = rand() % 2;
					PTE[i].protect = rand() % 4;
					PTE[i].dirty = rand() % 2;
					PTE[i].supervisor = rand() % 2;

					sleep(rand()%100 * 0.001);
				}
			}
		}
	}

	return 1;
}

int get_PFN(long int request){
	int mask = 0x1C00; // [(111)-0000000000]

	int VPN = (request & mask) >> 10;

	return VPN;
}

int get_Offset(long int request){
	int mask = 0x3FF; // [(000)-1111111111]

	return (request & mask);
}

struct PageTableEntry AccessMemory(long int request, struct PageTableEntry *PTE){
	int PFN = get_PFN(request);

	int offset = get_Offset(request);

	PTE[PFN].offset = offset;

	return (PTE[PFN]);
}


int main(int argc, char *argv[]){
	struct PageTableEntry PTE[PAGES]; // 8 pages per process

	initPageTable(PTE);

	for (int i = 0; i < PAGES; ++i){
		printf("PAGE NUMBER %d\n", i);
		printf("\tPFN %d\n", PTE[i].pfn);
		printf("\tvalid %d\n", PTE[i].valid);
		printf("\tpresent %d\n", PTE[i].present);
		printf("\tprotect %d\n", PTE[i].protect);
		printf("\tdirty %d\n", PTE[i].dirty);
		printf("\tsupervisor %d\n", PTE[i].supervisor);
	}

	long int request;

	// Enter Request HERE
	printf("\nEnter an address (decimal) (16-bit):\t");
	scanf("%ld", &request);

	struct PageTableEntry Req_PTE = AccessMemory(request, PTE);

	if ( (!Req_PTE.valid) || (Req_PTE.offset > 1024) ){
		printf("\nSEGMENTATION_FAULT\n");
		return -1;
	}

	if (Req_PTE.protect < 1){
		printf("\nPROTECTION_FAULT\n");
		return -1;
	}

	int physical_address = Req_PTE.pfn * 1024 + Req_PTE.offset;

	printf("\nPhysical Address:\t%d\n", physical_address);

	return 0;
}