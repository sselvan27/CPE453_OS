Authors: Kent Zhang, James Abundis
Class: CSC 453-11
Quarter: Spring 2019

Implementation notes and assumptions:
	-pipeline assumes maximum of 10 commands
	-'&' allows parent and child to run concurrently but causes blocking as expected

Run:
	-Use Makefile
	-Run ./simple-shell

Testing:
	-Command: ls
	-Command: ls -a
	-Command: ls -a > output.txt
	-Command: wc < output.txt
	-Command: ls -l | less
	-Command: exit
	-Command: ls, Command: !!
	-Command (check for no recent command by running at start): !!
	-Command: cat simple-shell.c &