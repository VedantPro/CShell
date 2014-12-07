// signal handlers
void child_signal_HANDLER(int p){

	// wait for all the childs to finish executing
	// return immediately 
	while(waitpid(-1,NULL,WNOHANG)>0){
	}
	//printf("command executed;");

}
void int_signal_HANDLER(int p){

	//handles Ctrl+C 
	if(kill(pid,SIGKILL)==0){
		printf("\nprocess %d has been terminated by the user",pid);
		fflush(stdout);
	}
	else{
		if(pid<0){
			printf("\nyou pressed CTRL+C");
		}else{
			printf("process couldn't be terminated");
		}
	}

}