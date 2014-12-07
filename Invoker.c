void command_HANDLER(char *args[]){

	int i=0;
	int j=0;
	char *spec_chars[100];

	while(args[i]){
		if((strcmp(args[i],">")==0) || (strcmp(args[i],">>")==0)  ||(strcmp(args[i],"<")==0) || (strcmp(args[i],"|")==0) || (strcmp(args[i],"&")==0) || (strcmp(args[i],"$")==0)){
			spec_chars[j] = args[i];
			j++;
		}
		i++;
	}

	if(j==0){
		if(strcmp(args[0],"exit")==0){
			system("clear"); 
			exit(0);
		}
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
		else if(strcmp(args[0],"history")==0){
			if(args[1]==NULL){
				print_HISTORY('0');
			}
			else{
				print_HISTORY((char)*args[1]);
			}
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

				fileIO_EXECUTER(args,args[i+1],args[i+3],3);
				return;
			}

			// only for output redirection (beta stage)
			else if (strcmp(args[i],">") ==0 ){
			
				if (args[i+1] == NULL){
					printf("Not enough input arguments: output file not specified");
					return;
				}
				args[i] = NULL;
				fileIO_EXECUTER(args,NULL,args[i+1],1);
				return;
			}
			// output redirection with appending (beta stage)
			else if(strcmp(args[i],">>") == 0){

				if(args[i+1] == NULL){
					printf("Not enough input arguments: output file not specified");
					return;
				}
				args[i] = NULL;
				fileIO_EXECUTER(args,NULL,args[i+1],2);
				
			}

			i++;

		}

	}

}