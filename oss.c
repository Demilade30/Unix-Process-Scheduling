#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "oss.h"
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <strings.h>
#include <signal.h>

char *log_filename = "logfile.log";
char *prog_name;

// shared memory
static int shmid = -1;
static int queue_id = -1;

static void helpMenu(){
	printf("Usage: ./oss [-h] [-n proc] [-s simul] [-t timelimit] [-f logfile]\n");
        printf("\t\t-h describes how the project should run and then, terminates.\n");
        printf("\t\t-n proc: the number of total children worker processes to launch.\n");
        printf("\t\t-s simul: the number of children processes that can be running simultaneously and.\n");
	printf("\t\t-t timeLimit: ceilling limit of the time interval that the oss will fork worker process\n");
	printf("\t\t-f logfile: contains the output of oss.c\n");
	printf("\tIf any of the parameter above are not defined by user, the default value for them is 1\n");
}


