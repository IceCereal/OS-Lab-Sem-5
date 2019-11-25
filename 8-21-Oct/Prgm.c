/*
	Lab 8/Project 2: Simulating Virtual Memory: Page tables with/without TLB
	
	This lab task expands on the previous lab task of simulating page tables. 

	Simulating a TLB. The following code is for TLB implementation from the text book.
	You need to repeat the previous experiment (meaning run the same sequence of virtual addresses)
	assuming that we have different TLBs of sizes, 64, 128 and 256. Measure the times and show the
	difference from the previous scenario for similar access. One way to show the difference is to
	add some sleep code for memory access. The sleep code simulates the delay to retrieve the PTE
	from main memory.

	1 VPN = (VirtualAddress & VPN_MASK) >> SHIFT
	2 (Success, TlbEntry) = TLB_Lookup(VPN)
	3 if (Success == True) // TLB Hit
	4 if (CanAccess(TlbEntry.ProtectBits) == True)
	5 Offset = VirtualAddress & OFFSET_MASK
	6 PhysAddr = (TlbEntry.PFN << SHIFT) | Offset
	7 Register = AccessMemory(PhysAddr)
	8 else
	9 RaiseException(PROTECTION_FAULT)
	10 else // TLB Miss
	11 PTEAddr = PTBR + (VPN * sizeof(PTE))
	12 PTE = AccessMemory(PTEAddr)
	13 if (PTE.Valid == False)
	14 RaiseException(SEGMENTATION_FAULT)
	15 else if (CanAccess(PTE.ProtectBits) == False)
	16 RaiseException(PROTECTION_FAULT)
	17 else
	18 TLB_Insert(VPN, PTE.PFN, PTE.ProtectBits)
	19 RetryInstruction()

	The output of this program is similar to the previous program except you will also include time
	taken to access TLB and time taken during TLB miss.

	Additional Notes:
		1. Using time.h library and functions like timeoftheday, gettime etc can be quite useful for measurement.
		2. Functions like, sleep, rand are useful.
		3. Use the code you wrote for random number generation to simulate the virtual addresses generated.
		4. Use the bit level operations in C to do the shifting and masking operations.
		5. Please note that the sizes of the virtual and physical addresses must be conforming to the specification.
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