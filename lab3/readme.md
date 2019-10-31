Authors: Kent Zhang, James Abundis
Project: Lab 3 Multithreaded Sort
Class: CSC 453-11
Quarter: Spring 2019

Implementation notes and assumptions:
	-implementation uses merge sort
	-number of integers must match number of integers input file
	-input file assumes an integer per line
	-prints runtime to console
	-provided unsorted file for testing in "nums_100k.txt"
	-creates output file of sorted integers in "sorted.txt"

Run:
	-Use Makefile, type make
	-Run ./multithreadsort [num threads] [num of integers] [input file]

Testing:
	-Command: ./multithreadsort 4 100000 nums_100k.txt

Test results (5):
	Execution time (multithreaded): 0.031872 seconds
	Execution time (sequential): 0.037126

	Execution time (multithreaded): 0.023942 seconds
    Execution time (sequential): 0.039806 seconds

    Execution time (multithreaded): 0.021532 seconds
    Execution time (sequential): 0.037153 seconds

    Execution time (multithreaded): 0.021300 seconds
    Execution time (sequential): 0.031033 seconds

	Execution time (multithreaded): 0.023591 seconds
	Execution time (sequential): 0.038234 seconds


