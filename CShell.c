#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>		
#include <signal.h>		
#include <sys/types.h>
#include <sys/wait.h>	


//Global Variables
static pid_t PID;
static pid_t PGID;
static pid_t shell_PGID;
static char **ENV;
static struct termios TMODES;
pid_t pid;
static int input = 0;

struct sigaction act_child;
struct sigaction act_int;


//handlers
void child_signal_HANDLER(int p);
void int_signal_HANDLER(int p);

//basic function declarations
void env_MANAGER(char *args[],int act);
void command_EXECUTER(char *args[]);
void command_HANDLER(char *args[]);
void pipe_EXECUTER(char *args[]);
void fileIO_EXECUTER(char *args[], char *inFile, char * outFile, int opt);
void echo_HANDLER(char *args[]);
void back_proc_HANDLER(char *args[],int back_stat);



// signal handlers
void child_signal_HANDLER(int p){

	// wait for all the childs to finish executing
	// return immediately 
	while(waitpid(-1,NULL,WNOHANG)>0){
	}
	printf("command executed; ");

}
void int_signal_HANDLER(int p){

	//handles Ctrl+C 
	
	if(kill(pid,SIGTERM)==0){
		printf("\nprocess %d has been terminated by the user",pid);
	}
	else{
		if(pid<0){
			printf("you pressed CTRL+C\nGood try shell won't close like this!");
		}else{
			printf("process couldn't be terminated");
		}
	}

}

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


	shell_INIT();

	char shell_INPUT[1024];
	char *tokens[256];
	int tok_counts = 0;

	ENV = envp;

	printf("\t============================\n\n");
	printf("\t         LINUX shell        \n\n");
	printf("\t A Shell implementation in C\n\n");
	printf("\t============================\n\n");

	while(1){

		pid = -10;

		memset(shell_INPUT,'\0',sizeof(shell_INPUT));

		printf("\n%s@%s: ",getenv("USER"),getenv("SESSION"));
		fgets(shell_INPUT,1024,stdin);

		if((tokens[0] = strtok(shell_INPUT," \n)\t")) == NULL) continue;
		
		tok_counts = 1;

		while((tokens[tok_counts] = strtok(NULL, " \n\t")) != NULL) tok_counts++;
		/*
		int i=0;
		while(tokens[i]){
			printf("%s ",tokens[i]);
			i++;
		}
		tokens[tok_counts] = NULL;*/
		command_HANDLER(tokens);

	}

	return 0;
}

void back_proc_HANDLER(char *args[],int back_stat){

	if((pid=fork())==-1){
		perror("fork");
		printf("\nexiting");
		exit(1);
	}
	else if(pid==0){
		// ignore any interrupt signals
		signal(SIGINT,SIG_IGN);

		//setenv()

		if(execvp(args[0],args)==-1) printf("%s: command not found",args[0]); fflush(stdout); kill(getpid(),SIGTERM);

	}
	else if(pid>0){
		if(back_stat!=0){
			printf("%d process created succesfully",pid);
			fflush(stdout);
		}
		else{
			waitpid(pid,NULL,0);
		}
	}

}

void env_MANAGER(char *args[],int act){

	char **temp_env;
	temp_env = ENV;
	switch(act){

		// display all envi variables
		case 0:	while(*temp_env!=NULL){
					printf("%s\n",*temp_env);
					temp_env++;
				}
				break;

		// set envi variables
		case 1:	if(args[1]==NULL){
					return;
				}
				// checking for a possibility of already existing env variable.
				else if(getenv(args[1])!=NULL){
					printf("The variable is already present.\nDo you want to overwrite its value(yes:y,no:n)\n");
					char temp;
					scanf("%c",&temp);
					if(temp=='y'){
						setenv(args[1],args[2],1);
					}
					else{
						return;
					}
				}
				// checking whether the value is given by the user of not
				else{
					if(args[2]==NULL){
						printf("Please specify the value!");
						return;
					}else{
						setenv(args[1],args[2],1);
						printf("Variable has been created");
					}
				}
				break;
		
		// unset envi variables
		case 2: if(getenv(args[1])==NULL){
					printf("Sorry variable doesn't exist");
				}else{
					unsetenv(args[1]);
					printf("The variable has been deleted");
				}
				break;

	}


}


void change_DIR(char *args[]){

	if(args[1]==NULL){
		chdir(getenv("HOME"));
		printf("changed directory to: ~/");
	}
	else{
		if(chdir(args[1])==-1){
			printf("%s: no such directory",args[1]);
		}
		else{
			printf("changed directory to: %s",args[1]);
		}
	}

}

void pipe_EXECUTER(char *args[]){


	// File descriptors
	int filedes[2];
	int filedes2[2];
	
	int tot_cmds = 0;
	
	char *command[256];
	
	pid_t pid;
	
	int err = -1;
	int end = 0;
	
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	
	//  number of commands seperated separated by '|')
	while (args[l] != NULL){
		if (strcmp(args[l],"|") == 0){
			tot_cmds++;
		}
		l++;
	}
	tot_cmds++;
	
	while (args[j] != NULL && end != 1){
		k = 0;

		//array of pointers to store the command which will be executed 
		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;	
			if (args[j] == NULL){

				end = 1;
				k++;
				break;
			}
			k++;
		}
		// last argument needs te be null
		command[k] = NULL;
		j++;		
		
		// depending on iteration closing the respective file descriptor
		if (i % 2 != 0){
			pipe(filedes); // for odd i
		}else{
			pipe(filedes2); // for even i
		}
		
		pid=fork();
		
		if(pid==-1){
			//closing pipes on errors			
			if (i != tot_cmds - 1){
				if (i % 2 != 0){
					close(filedes[1]); // for odd i
				}else{
					close(filedes2[1]); // for even i
				} 
			}			
			printf("Child process could not be created\n");
			return;
		}
		if(pid==0){
			// for the first command
			if (i == 0){
				dup2(filedes2[1], STDOUT_FILENO);
			}

			// for the last command 
			else if (i == tot_cmds - 1){
				if (tot_cmds % 2 != 0){ 
					dup2(filedes[0],STDIN_FILENO);
				}else{ 
					dup2(filedes2[0],STDIN_FILENO);
				}
			// for any middle command
			}else{ 
				if (i % 2 != 0){
					dup2(filedes2[0],STDIN_FILENO); 
					dup2(filedes[1],STDOUT_FILENO);
				}else{ 
					dup2(filedes[0],STDIN_FILENO); 
					dup2(filedes2[1],STDOUT_FILENO);					
				} 
			}
			
			if (execvp(command[0],command)==err){
				kill(getpid(),SIGTERM);
			}		
		}
				
		// closring file descriptors on parent
		if (i == 0){
			close(filedes2[1]);
		}
		else if (i == tot_cmds - 1){
			if (tot_cmds % 2 != 0){					
				close(filedes[0]);
			}else{					
				close(filedes2[0]);
			}
		}else{
			if (i % 2 != 0){					
				close(filedes2[0]);
				close(filedes[1]);
			}else{					
				close(filedes[0]);
				close(filedes2[1]);
			}
		}
				
		waitpid(pid,NULL,0);
				
		i++;	
	}

}


void fileIO_EXECUTER(char *args[], char *inFile, char * outFile, int opt){


	int fileDescriptor; // between 0 and 19, describing the output or input file
	
	if((pid=fork())==-1){
		printf("Child process could not be created\n");
		return;
	}
	if(pid==0){

		// for output redirection only
		if (opt == 0){
			
			fileDescriptor = open(outFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			dup2(fileDescriptor, STDOUT_FILENO); 
			close(fileDescriptor);
		
		}
		// for both input and output redirection
		else if (opt == 1){

			//input
			fileDescriptor = open(inFile, O_RDONLY, 0600);  
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			
			//output
			fileDescriptor = open(outFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);		 
		}
		 
		//setenv("parent",getcwd(currentDirectory, 1024),1);
		
		if (execvp(args[0],args)==-1){
			printf("err");
			kill(getpid(),SIGTERM);
		}		 
	}
	waitpid(pid,NULL,0);

}

void command_EXECUTER(char *args[]){

	pid_t pid;

	if((pid=fork())<0){
		printf("couldn't create the child");
		exit(1);
	}
	else if(pid==0){
		
		if((execvp(args[0],args))==-1) printf("%s: command not found",args[0]); fflush(stdout); kill(getpid(),SIGTERM);
	}
	else waitpid(pid,NULL,0);

}

void command_HANDLER(char *args[]){

	int i=0;
	int j=0;
	char *spec_chars[100];

	while(args[i]){
		if((strcmp(args[i],">")==0) || (strcmp(args[i],"<")==0) || (strcmp(args[i],"|")==0) || (strcmp(args[i],"&")==0) || (strcmp(args[i],"$")==0)){
			spec_chars[j] = args[i];
			j++;
		}
		i++;
	}

	if(j==0){
		if(strcmp(args[0],"exit")==0) exit(0);
		else if(strcmp(args[0],"clear")==0) system("clear");
		else if(strcmp(args[0],"penv")==0) env_MANAGER(args,0);
		else if(strcmp(args[0],"unsenv")==0){
			env_MANAGER(args,2);
		}
		else if(strcmp(args[0],"senv")==0){
			env_MANAGER(args,1);
		}
		else if(strcmp(args[0],"cd")==0){
			change_DIR(args);
		}
		else command_EXECUTER(args);
	}
	else{

		i = 0;
		j = 0;
		int k;

		while(args[i]){

			// for putting processes in background
			if(strcmp(args[i],"&")==0){

				args[i]=NULL;
				back_proc_HANDLER(args,1);
				return;
			}

			// for pipe handling (beta stage)
			else if(strcmp(args[i],"|")==0){

				pipe_EXECUTER(args);
				return;
			}

			//for file input and output (beta stage)
			// both redirections
			else if (strcmp(args[i],"<") == 0){

				k = i+1;

				if (args[k] == NULL || args[k+1] == NULL || args[k+2] == NULL ){
					printf("Not enough input arguments");
					return;
				}else{
					if (strcmp(args[k+1],">") != 0){
						printf("Usage: Expected '>' and found %s\n",args[i+1]);
						return;
					}
				}

				args[i] = NULL;

				fileIO_EXECUTER(args,args[i+1],args[i+3],1);
				return;
			}

			// only for output redirection (beta stage)
			else if (strcmp(args[i],">") == 0){

				if (args[i+1] == NULL){
					printf("Not enough input arguments: output file not specified");
					return;
				}
				args[i] = NULL;
				fileIO_EXECUTER(args,NULL,args[i+1],0);
				return;
			}
			else{
				
			}

			i++;

		}

	}

}