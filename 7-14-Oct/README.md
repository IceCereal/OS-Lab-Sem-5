# Lab 7: Simulating Virtual Memory: Page tables
	
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
		
To compile:<br>
```gcc -o output Prgm.c -Wall```

To run:<br>
```./output```

Enter a 16-bit decimal number (0 - 65535) and depending on the `validity` & `protectedness` of the address, the program tells the 
physical address of it.
