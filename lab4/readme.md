Authors: Kent Zhang, James Abundis
Project: Lab 4 Contingous Memory Allocation
Class: CSC 453-11
Quarter: Spring 2019

Implementation notes and assumptions:
	-Process name has a maximum of 30 characters

Run:
	-Use Makefile, type make
	-Run ./allocator [intial amount of memory]

Testing:
	./allocator 10000
	Request:
		allocator>RQ P0 2000 W
		allocator>RQ P1 1000 F
		allocator>RQ P2 3000 B
		allocator>RQ P3 2000 W
		allocator>RQ P4 2000 F

	Release:
		allocator>RL P0
		allocator>RL P2
		allocator>RL P4

	Compact:
		allocator>C

	Status report:
		allocator>STAT
		
	Exit:
		allocator>X


