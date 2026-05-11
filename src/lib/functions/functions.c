#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <errno.h>
#include <ctype.h>

volatile sig_atomic_t Signal = 0;

void process_line(char line[256]);
void grep(char input_string[256]);

void signal_handler(int signo) {

  if (signo == SIGINT || signo == SIGQUIT) {
    Signal = 1;
  }

}

int main(int argc, char* argv[]) {

  struct sigaction sa;

  sa.sa_handler = &signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction failed for SIGINT");
    exit(1);
  }
  if (sigaction(SIGQUIT, &sa, NULL) == -1) {
    perror("sigaction failed for SIGQUIT");
    exit(1);
  }

  fd_set read_fds;
  char buffer[256];

  while(1) {
    
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    int input_check = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, NULL);

    if (input_check < 0 && errno == EINTR) {
      if (Signal == 1) {
	printf("signal caught, exiting...\n");
	exit(1);
      }
      continue;
    }

    if (input_check > 0 && FD_ISSET(STDIN_FILENO, &read_fds)) {
      if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

	if (strlen(buffer) == 2){
	  if ((buffer[0] == 'q') || (buffer[0] == 'Q')) {
	    printf("entered 'q', exiting...\n");
	    exit(0);
	  }
	}

	if (strlen(buffer) == 5){

	  //	  buffer[strcspn(buffer, "\n")] = '\0';

	  int i;
	  char temp_buff[5];
	  for (i=0; i < 4; i++) {
	    temp_buff[i] = toupper(buffer[i]);
	  }
	  
	  printf("here: %s\n", temp_buff);

	  if (strcmp(temp_buff, "QUIT") == 0) {
	    printf("entered quit, exiting...\n");
	    exit(0);
	  }

	}
	
	/* passed exit checks, now process input */
	process_line(buffer);

      }
      
    }
  }

  /* end of program, exit */
  exit(0);
}

void process_line(char line[256]) {

  char command[10];
  
  printf("you entered %s", line);

  if (sscanf(line, "%9s", command) == 1) {
    printf("command: %s\n", command);

    if (strcmp(command, "grep") == 0) {
      grep(line);
    }

  }
  else {
    printf("did not find word");
  }

}

void grep(char input_string[256]) {

  printf("time to grep...\n");

}
