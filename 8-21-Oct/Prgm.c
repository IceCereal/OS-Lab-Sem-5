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

struct TLB_Entry{
	unsigned int active : 1;	// Alive or Not
	unsigned int vpn : 6;	// VPN
	time_t time_used;	// Time of Last Access

	unsigned int pfn : 14;	// Physical Frame Number
	unsigned int offset : 10; // offset
	unsigned int valid : 1;	// Valid
	unsigned int present : 1;	// Present
	unsigned int protect : 3;	// Protect 01R, 10W, 11X
	unsigned int dirty : 1;	// Dirty
	unsigned int supervisor : 1;	// User/Kernel
};

#define PAGES 64 // This comes from Calculations - (1)
#define TLB_SIZE 32

int initPageTable(struct PageTableEntry *PTE){
	// This initializes the Page Table Entries
	// When the process is created, the pages are created

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

int get_VPN(long int request){
	int mask = 0xFC00; // [(111111)-0000000000]

	int VPN = (request & mask) >> 10;

	return VPN;
}

int get_Offset(long int request){
	int mask = 0x3FF; // [(000000)-1111111111]

	return (request & mask);
}

struct PageTableEntry AccessMemory(long int request, struct PageTableEntry *PTE){
	sleep(1); // delay in getting from memory

	int VPN = get_VPN(request);

	int offset = get_Offset(request);

	PTE[VPN].offset = offset;

	return (PTE[VPN]);
}

int initTLB(struct TLB_Entry *TLBE){
	// Make TLB's Cells Inactive
	for (int i = 0; i < TLB_SIZE; ++i){
		TLBE[i].active = 0;
		TLBE[i].vpn = 0;
	}

	return 1;
}

int main(int argc, char *argv[]){
	struct PageTableEntry PTE[PAGES];

	struct TLB_Entry TLBE[TLB_SIZE];

	initPageTable(PTE);
	initTLB(TLBE);

	long int request; // This is what the program requests to see
	int count = -1;

	time_t time_1, time_2;

	while (count < 100){
		count += 1;
		srand(time(NULL));

		request = rand() % 65536; // Our Request can only be 16 bits long

		int VPN = get_VPN(request);

		printf("\n\niter %d | request %ld | VPN %d\nActive:\t", count, request, VPN);
		for (int i = 0; i < TLB_SIZE; ++i){
			printf("%d, ", TLBE[i].active);
		}
		printf("\nVPN:\t");
		for (int i = 0; i < TLB_SIZE; ++i){
			printf("%d, ", TLBE[i].vpn);
		}
		printf("\n");

		int existsTLB = 0;

		time_1 = time(0);

		for (int i = 0; i < TLB_SIZE; ++i){
			// Check whether this cell is active
			if (TLBE[i].active){
				// If the cell exists in The TLB
				if (TLBE[i].vpn == VPN){
					printf("HIT...\n");

					TLBE[i].time_used = time(0);
					existsTLB = 1;

					if (!TLBE[i].valid){
						printf("SEGMENTATION_FAULT\n");

						time_2 = time(0);
						printf("TIME TAKEN:\t%ld", time_2-time_1);

						sleep(1);
						break;
					}

					if (!TLBE[i].protect){
						printf("PROTECTION_FAULT\n");

						time_2 = time(0);
						printf("TIME TAKEN:\t%ld", time_2-time_1);

						sleep(1);
						break;
					}

					printf("Physical Address at:\t%d\n", TLBE[i].pfn * 1024 + get_Offset(request));

					time_2 = time(0);
					printf("TIME TAKEN:\t%ld", time_2-time_1);

					sleep(1);
					break;
				}
			} else{
				// The cell isn't active, i.e., directly goes to the TLB
				printf("Making Active...\n");
				existsTLB = 1;
				struct PageTableEntry Req_PTE = AccessMemory(request, PTE);
				TLBE[i].active = 1;
				TLBE[i].vpn = VPN;
				TLBE[i].time_used = time(0);

				TLBE[i].pfn = Req_PTE.pfn;
				TLBE[i].valid = Req_PTE.valid;
				TLBE[i].present = Req_PTE.present;
				TLBE[i].protect = Req_PTE.protect;
				TLBE[i].dirty = Req_PTE.dirty;

				printf("Physical Address at:\t%d\n", TLBE[i].pfn * 1024 + get_Offset(request));
				
				time_2 = time(0);
				printf("TIME TAKEN:\t%ld", time_2-time_1);

				break;
			}
		}

		// It was a Miss. 
		if (!existsTLB){
			printf("Miss...\n");
			struct PageTableEntry Req_PTE = AccessMemory(request, PTE);

			// to find the Least Recently Used TLB-Entry

			int LRU_TLB_Index = 0;
			for (int i = 0; i < TLB_SIZE; ++i)
				if (TLBE[i].time_used < TLBE[LRU_TLB_Index].time_used)
					LRU_TLB_Index = i;

			// Replace LRU with current

			TLBE[LRU_TLB_Index].vpn = VPN;
			TLBE[LRU_TLB_Index].time_used = time(0);

			TLBE[LRU_TLB_Index].pfn = Req_PTE.pfn;
			TLBE[LRU_TLB_Index].valid = Req_PTE.valid;
			TLBE[LRU_TLB_Index].present = Req_PTE.present;
			TLBE[LRU_TLB_Index].protect = Req_PTE.protect;
			TLBE[LRU_TLB_Index].dirty = Req_PTE.dirty;

			int physical_address = Req_PTE.pfn * 1024 + Req_PTE.offset;

			if (!Req_PTE.valid){
				printf("SEGMENTATION_FAULT\n");

				time_2 = time(0);
				printf("TIME TAKEN:\t%ld", time_2-time_1);

				continue;
			}

			if (Req_PTE.protect < 1){
				printf("PROTECTION_FAULT\n");

				time_2 = time(0);
				printf("TIME TAKEN:\t%ld", time_2-time_1);

				continue;
			}

			printf("Physical Address:\t%d\n", physical_address);

			time_2 = time(0);
			printf("TIME TAKEN:\t%ld", time_2-time_1);
		}
	}

	return 0;
}