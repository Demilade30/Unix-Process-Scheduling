#Project 4: Process Scheduling


Name:	Divine Akinjiyan
VCS:	git
Date:	04/04/2023

#Files
	oss.c - the main program
	worker.c - the application program
	oss.h, worker.h - header files containing any constant needed for this project
	makefile

##How to run the program
	make
	./oss [-h] [-s t] [-l f]
		-h	describes how the project should be run, and then terminates
		-s t	indicates how many maximum seconds before the system terminates
		-l f	specifies a particular name for the log file

##Remove all executables and object files
	make clean

##Data structures
	message queue: main method to communicate between the oss and worker processes
	timespec: used to simulate a clock
	bit map: used to keep track of the process ID
	shared memory: contains the information about the clock and process control block

##Note
Make sure to use the correct syntax when running the executables and makefile.

##License
This project is licensed under the MIT License

##Acknowledgements
Mark Hauschild, Associate Teaching Professor Ph.D. in Applied Mathematics, University of Missouri, St. Louis
