/**
 * Figure 3.34
 */

  #include <stdio.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <semaphore.h>
  #include <fcntl.h>

  volatile sig_atomic_t cleanup = 0; //flag for cleaning up process if SIGINT is called

  // Makes sure to unlink semaphores when program is stopped
  void handle_sigint(int sig) {
      const char msg[] = "\nCaught SIGINT, cleaning up semaphores...\n";
      write(STDOUT_FILENO, msg, sizeof(msg) - 1);

      cleanup = 1;
  }

  int main()
  {
    pid_t pid[5];
    int status, i;

	// Register SIGINT handler
        signal(SIGINT, handle_sigint);

	sem_t *sem[5];
	sem[0] = sem_open("/SEM0", O_CREAT, 0666, 1);
	sem[1] = sem_open("/SEM1", O_CREAT, 0666, 1);
	sem[2] = sem_open("/SEM2", O_CREAT, 0666, 1);
	sem[3] = sem_open("/SEM3", O_CREAT, 0666, 1);
	sem[4] = sem_open("/SEM4", O_CREAT, 0666, 1);

	for(i=0; i<5; i++) {
		pid[i] = fork();
		if (pid[i] < 0) {
		  /* error occurred */
		  fprintf(stderr, "Fork Failed");
		  return 1;
		}
		else if (pid[i] == 0) {
    		   char str[20]; // Ensure the buffer is large enough

		   // Convert the integer to a string and store in str
    		   snprintf(str, sizeof(str), "%d", i);

		  // Pass id to child
		  char *args[] = {"./philosopher", str, NULL};
		  execvp(args[0], args);
		}
		else { /* parent process */
		  printf("created child %d.\n", pid[i]);
		}
	}

	for(i=0; i<5; i++) {

		waitpid(pid[i], &status, 0);  // blocks until child exits or SIGINT
    		
		if (cleanup) {break;} //Catches that SIGINT occured and exits loop

		printf("child finished with status %d.\n", status);
	}

	//Kill and reap children if cleanup requested (SIGINT)
	if (cleanup){
		for (int i = 0; i < 5; i++) {
          	   kill(pid[i], SIGTERM);
        	}
		for (int i = 0; i < 5; i++) {
    		   waitpid(pid[i], NULL, 0);
		}
	}

	for(i=0; i<5; i++) {
		if (sem_close(sem[i]) != 0){
			perror("Parent: sem_close failed");
		}
	}
	

	if (sem_unlink("/SEM0") < 0) {
		perror("Parent: sem_unlink failed");
	}
	if (sem_unlink("/SEM1") < 0) {
		perror("Parent: sem_unlink failed");
	}
	if (sem_unlink("/SEM2") < 0) {
		perror("Parent: sem_unlink failed");
	}
	if (sem_unlink("/SEM3") < 0) {
		perror("Parent: sem_unlink failed");
	}
	if (sem_unlink("/SEM4") < 0) {
		perror("Parent: sem_unlink failed");
	}

	

	
// 			printf("Parent (PID: %d) is waiting for children %d and %d to finish.\n", getpid(), pid1, pid2);
// 			waitpid(pid1, &status, 0);
// 			printf("producer child finished with status %d.\n", status);
// 			waitpid(pid2, &status, 0);
// 			printf("consumer child finished with status %d.\n", status);
// 			printf("Parent is exiting.\n");      
	
	
    return 0;
  }

