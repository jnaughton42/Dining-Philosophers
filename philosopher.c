/**
 * Figure 3.34
 */

  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <string.h>
  #include <semaphore.h>
  #include <fcntl.h>
  #include <signal.h>

  
  int main(int argc, char *argv[])
  {
	printf("child running %s.\n", argv[1]);

	char name1[32];
  	char name2[32];

	sem_t *sem1;
	sem_t *sem2;

	// Convert passed id to an int and then add 1 to define which semaphores to grab
	int id = atoi(argv[1]);
	int next_id = (id + 1) % 5;

	// Concatenate ids to names to open semaphores
	snprintf(name1, sizeof(name1), "/SEM%d", id);
    	snprintf(name2, sizeof(name2), "/SEM%d", next_id);

	sem1 = sem_open(name1, 0);
	sem2 = sem_open(name2, 0);

	if (sem1 == SEM_FAILED || sem2 == SEM_FAILED) {
	        perror("sem_open failed");
	        exit(1);
	   }

	for (int i = 0; i < 1000; i++){
		sem_wait(sem1);
		//printf("Child: %s grabbed chopstick %d\n", argv[1], id);

		sem_wait(sem2);
		//printf("Child: %s grabbed chopstick %d\n", argv[1], next_id);

		printf("Child: %s has chopsticks\n", argv[1]);

		sem_post(sem2);
		sem_post(sem1);
	}

	sem_close(sem1);
	sem_close(sem2);

	printf("Child: %s complete\n", argv[1]);

	exit(0);

    return 0;
  }
