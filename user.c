#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "user.h"
#include "oss.h"
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <strings.h>
#include <signal.h>

static int sid = -1;
static int qid = -1;
static struct shmem *shm = NULL;

char* prog_name;
static int createSHM(){
	sid = shmget(key_shmem, sizeof(struct shmem), 0);
        if(sid < 0){
                fprintf(stderr,"%s: failed to get id for shared memory. ",prog_name);
                perror("Error");
                return -1;
        }

        shm = (struct shmem *)shmat(sid,NULL,0);
        if(shm == (void *)-1){
                fprintf(stderr,"%s: failed to get pointer for shared memory. ",prog_name);
                perror("Error");
                return -1;
        }

        qid = msgget(key_queue,0);
        if(qid == -1){
                fprintf(stderr,"%s: failed to get id for queue. ",prog_name);
                perror("Error");
                return -1;
        }
	return 0;
}

static int deallocateSHM(){
	if(shm != NULL){
                if(shmdt(shm) == -1){
                        fprintf(stderr,"%s: failed to detach shared memory. ",prog_name);
                        perror("Error");
                }
        }
	return 0;
}
static void addTime(struct timespec *a, const struct timespec *b){
	static const unsigned int max_ns = 1000000000;

  	a->tv_sec += b->tv_sec;
  	a->tv_nsec += b->tv_nsec;
  	while(a->tv_nsec > max_ns)
  	{
    		a->tv_sec++;
    		a->tv_nsec -= max_ns;
  	}
}
static void userProcess(){
	int alive = 1;
	//const int io_block_prob = (IObound) ? IO_IO_BLOCK_PROB : CPU_IO_BLOCK_PROB;
	
	while(alive){
		struct ossMsg m;
		
		m.from = getpid();
		if (msgrcv(qid, (void *)&m, MESSAGE_SIZE, m.from, 0) == -1){
			fprintf(stderr,"%s: failed to receive message. ",prog_name);
                	perror("Error");
			break;
		}

		const int timeslice = m.timeslice;
		if(timeslice == 0){
			break; // if it has use up its time slice, then exit
		}
		
		bzero(&m, sizeof(struct ossMsg));

		const int willTerminate = ((rand() % 100) <= TERM_PROB) ? 1 : 0;	
		if (willTerminate) // terminated successfully
		{
			m.timeslice = sTERMINATED;
			m.clock.tv_nsec = rand() % timeslice + 1;
			alive = 0;
		}else{
			const int will_interrupt = ((rand() % 100) < IO_IO_BLOCK_PROB) ? 1 : 0;
			if (will_interrupt){
				int r = rand() % EVENT_R;
				int s = rand() % EVENT_S;
				if(r == PREMPT_R){
					m.timeslice = sREADY;
					int p = rand() % PREMPT_QUANTUM + 1;
					int timespend = timeslice / 100 * p;
					struct timespec temp = {.tv_sec = 0, .tv_nsec = p};
					addTime(&m.clock, &temp); 					
				}else{		
					m.timeslice = sBLOCKED;
					m.clock.tv_nsec = rand() % timeslice + 1;
					m.io.tv_sec = r;
					m.io.tv_nsec = s;
				}
			}else{
				m.timeslice = sREADY;
				m.clock.tv_nsec = timeslice;
			}
		}

		m.mtype = getppid();
		m.from = getpid();
		if (msgsnd(qid, (void *)&m, MESSAGE_SIZE, 0) == -1){
			fprintf(stderr,"%s: failed to send message. ",prog_name);
                        perror("Error");
			break;
		}
	}
}
static void signalHandler(){
	deallocateSHM();
	exit(1);
}
int main(int argc, char** argv){
	prog_name = argv[0];
	if (argc != 1)
	{
		fprintf(stderr, "%s: Invalid number of arguments.\n",prog_name);
		return EXIT_FAILURE;
	}
	
	signal(SIGINT, signalHandler);
		
	srand(getpid()); //seeding off
	
	if(createSHM() == -1)
		return EXIT_FAILURE;
	
	userProcess();

	if(deallocateSHM() == -1)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
