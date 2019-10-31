Authors: Kent Zhang, James Abundis
Project: Lab 5 Virtual Memory Manager
Class: CSC 453-11
Quarter: Spring 2019

Implementation notes and assumptions:
	-Assume input file is named "addresses.txt"
	-Output file created is named "output.txt"
	-Translates logical to physical addresses using a TLB and page table.
	
	1. Page number is extracted from logical address and TLB is consulted.
	2. Check for TLB hit --> frame number is obtained from the TLB
	3. Check for TLB miss --> frame is obtained from page table or page fault occurs.


Run:
	-Use Makefile, type make
	-Run ./virtualmem addresses.txt

Testing:
	./diff correct.txt output.txt


