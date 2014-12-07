#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>		
#include <signal.h>		
#include <sys/types.h>
#include <sys/wait.h>	
#include <readline/readline.h>
#include <readline/history.h>
#include "AutoComplete.c"


// Shell Initializor 

void shell_INIT(){

	pid_t PID = getpid();

	int is_interactive = isatty(STDOUT_FILENO);

	if(is_interactive){
		while(tcgetpgrp(STDIN_FILENO) != (PGID=getpgrp())){
			kill(PID,SIGTTIN);
		}

		// set handler for int and child
		act_child.sa_handler = child_signal_HANDLER;
		act_int.sa_handler = int_signal_HANDLER;

		sigaction(SIGCHLD,&act_child,0);
		sigaction(SIGINT,&act_int,0);
		//put shell in its own process group
		shell_PGID = getpid();
		if(setpgid(shell_PGID,PID)<0){
			printf("Couldn't put shell in its own process group");
			exit(1);
		}

		//grab the control of terminal
		tcsetpgrp(STDIN_FILENO,shell_PGID);

		//saving default attributes of terminal to shell
		tcgetattr(STDIN_FILENO,&TMODES);


	}else{
		printf("Sorry Shell is not interactive");
		exit(1);
	}

}

int main(int argc, char *argv[], char **envp){


	//shell initializer 
	shell_INIT();

	char *prompt = malloc(2000);
	char shell_INPUT[1024];
	int hist_count=0;
	char *tokens[256];
	int tok_counts = 0;

	ENV = envp;

	system("clear");
	printf("\n\t\t  ====================================\n\n");
	printf("\t\t               LINUX shell            \n\n");
	printf("\t\t      ----------------------------    \n\n");
	printf("\t\t       A Shell implementation in C    \n\n");
	printf("\t\t  ====================================\n\n");

	
	//declaring custom completion function
	 rl_attempted_completion_function = custom_completion;

	while(1){

		pid = -10;
		
		memset(shell_INPUT,'\0',sizeof(shell_INPUT));

		//char *prompt = malloc();
		//printf("%s@%s: ",getenv("USER"),getenv("SESSION"));
		sprintf(prompt,"%s@%s: ",getenv("USER"),getenv("SESSION"));

		//fgets(shell_INPUT,1024,stdin);
		input = readline(prompt);
		strncpy(shell_INPUT,input,sizeof(shell_INPUT));

		memcpy(HISTORY[hist_count],shell_INPUT,sizeof(shell_INPUT));
		hist_count++;

		//enable auto-complete
	    rl_bind_key('\t',rl_complete);

		if((tokens[0] = strtok(shell_INPUT," \n)\t")) == NULL) continue;

		add_history(input);
		free(input);
		
		tok_counts = 1;

		while((tokens[tok_counts] = strtok(NULL, " \n\t")) != NULL) tok_counts++;

		command_HANDLER(tokens);

	}

	return 0;
}


