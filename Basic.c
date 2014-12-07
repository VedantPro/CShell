
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

		if(execvp(args[0],args)==-1) printf("%s: command not found\n",args[0]); fflush(stdout); kill(getpid(),SIGTERM);

	}
	else if(pid>0){
		if(back_stat!=0){
			printf("%d process created succesfully\n",pid);
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
						printf("Please specify the value!\n");
						return;
					}else{
						setenv(args[1],args[2],1);
						printf("Variable has been created\n");
					}
				}
				break;
		
		// unset envi variables
		case 2: if(getenv(args[1])==NULL){
					printf("Sorry variable doesn't exist\n");
				}else{
					unsetenv(args[1]);
					printf("The variable has been deleted\n");
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


void command_EXECUTER(char *args[]){

	pid_t pid;

	if((pid=fork())<0){
		printf("couldn't create the child");
		exit(1);
	}
	else if(pid==0){
		
		if((execvp(args[0],args))==-1) printf("%s: command not found\n",args[0]); fflush(stdout); kill(getpid(),SIGTERM);
	}
	else waitpid(pid,NULL,0);

}

void print_HISTORY(char num){

	int a = num - '0';
	int i=0;
	int j=0;
	if(a==0){
		while(strlen(HISTORY[i])!=0){
			printf("%d  %s\n",i+1,HISTORY[i]);
			i++;
		}
	}
	else{
		while(strlen(HISTORY[i])!=0) i++;

		i = i-a;
		while(strlen(HISTORY[i])!=0){
			printf("%d  %s\n",i+1,HISTORY[i]);
			i++;
		}
	}
}