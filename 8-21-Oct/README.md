# Lab 8/Project 2: 

	Simulating Virtual Memory: Page tables with/without TLB
	
	This lab task expands on the previous lab task of simulating page tables. 
	
	Simulating a TLB. 
	
	The following code is for TLB implementation from the text book.
	
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

To compile:<br>
```gcc -o output Prgm.c -Wall```

To run:<br>
```./output```

This is an automatic simulation. No inputs are required. 
