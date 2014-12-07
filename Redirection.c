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

		switch(opt){

			// for output redirection only
			case 1: fileDescriptor = open(outFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
					dup2(fileDescriptor, STDOUT_FILENO); 
					close(fileDescriptor);
					break;

			// for output redirection with appending
			case 2:	if((fileDescriptor = open(outFile, O_APPEND | O_WRONLY, 0600))>=0){
						dup2(fileDescriptor, STDOUT_FILENO); 
						close(fileDescriptor);
						printf("something cool");
					}
					else{
						printf("%s: doesn't exist",outFile);
					} 
					break;
			// for both input and output redirection
			case 3: //input
					fileDescriptor = open(inFile, O_RDONLY, 0600);  
					dup2(fileDescriptor, STDIN_FILENO);
					close(fileDescriptor);
			
					//output
					fileDescriptor = open(outFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
					dup2(fileDescriptor, STDOUT_FILENO);
					close(fileDescriptor);	
					break;

			default: printf("bad option");
					exit(1);	
		}
		//setenv("parent",getcwd(currentDirectory, 1024),1);
		
		if (execvp(args[0],args)==-1){
			printf("err");
			kill(getpid(),SIGTERM);
		}		 
	}
	waitpid(pid,NULL,0);

}