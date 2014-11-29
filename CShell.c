#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		//for pid_t fork() and other system calls
#include <signal.h>		//for signal()
#include <sys/types.h>


void command_EXECUTER(char *args[]);
void command_HANDLER(char *args[]);

int main(int argc, char *argv[], char **envp){

	char shell_INPUT[1024];
	char *tokens[256];
	int tok_counts = 0;

	while(1){

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

void command_EXECUTER(char *args[]){

	pid_t pid;

	if((pid=fork())<0){
		printf("couldn't create the child");
		exit(1);
	}
	else if(pid==0){
		
		if((execvp(args[0],args))==-1) printf("%s: command not found",args[0]); fflush(stdout); kill(getpid(),SIGTERM);
	}
	else wait(pid);

}

void command_HANDLER(char *args[]){

	int i=0;
	int j=0;
	char spec_chars[100];

	while(args[i]){
		if(args[i]==">" || args[i]=="<" || args[i]=="|" || args[i]=="&" || args[i]=="$"){
			spec_chars[j] = *args[i];
			j++;
		}
		i++;
	}

	if(j==0){
		if(strcmp(args[0],"exit")==0) exit(0);
		else if(strcmp(args[0],"clear")==0) system("clear");
		else command_EXECUTER(args);
	}
	else{
		
	}
	

}