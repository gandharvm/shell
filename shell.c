#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>

#define MAX_INPUT_SIZE 150
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int BGRND; // To check if & is present in command

// For each process separated by pipes and stores the computed arguments
struct Process {
	char *name;
	char *argu[20];
	int len;
};

// To store information of the redirections 
struct Redirect
{
    char left[MAX_INPUT_SIZE][20];
    char right[MAX_INPUT_SIZE][20];
    char *op[20];
    int len;
};

// Take input character by character
char * input (void) {
    char * line = (char *) malloc( MAX_INPUT_SIZE );
    memset(line,0,strlen(line));
    char a ='\0', c='\0';

    while (1) {
        scanf("%c", &a );
        if (a != '\n') {
            strncat(line, &a, 1);
        }
        else{
            break;
        }
    }
    strncat(line, &c, 1);
    return line;
}

// Function to split the input array according to pipes and store it in Commands array and return its size
int splitPipe2 (char * command, char Commands[][MAX_INPUT_SIZE]) {

    int i = 0; char *p;

    while ((p = strsep(&command,"|")) != NULL) {
        strcpy(Commands[i], p);
        i += 1;
    }

    // Length of the newly formed array
    int n = i+1;

    return n;
}

// To populate the Redirection structure and store the information into redirection arrays
char * redirection( char * command , struct Redirect *R, char *args[]) {

    char cmd [MAX_INPUT_SIZE];
    int j = 0;

    while (*command == ' ')
    {
        command++;
    }

    strcpy(cmd, command);
    
    for (int i = 0; i < strlen(cmd); ++i)
    {
        if (cmd[i] == '<')
        {
			R->op[j] = "<";
			// printf("%s\n", R->op[j]);
            if (i > 1)
            {
                if (cmd[i-1] == '0' && cmd[i-2] == ' ')
                {
                    R->left[j][0] = '0';
					R->left[j][1] = '\0';
					cmd[i-1] = ' ';
                }
                else
                {
					R->left[j][0] = 'C';
					R->left[j][1] = '\0';
                }
            }

			int flag = 0, l=0;
			for (int k = i+1; k < strlen(cmd) ; k++) {
				// printf("%s\n", "inside");
				if (cmd[k] != ' ') {
					flag=1;
					// printf("%s\n","flag" );
					R->right[j][l] = cmd[k];
					cmd[k] =' ';
					l++;
				}
				else if (flag == 1 && cmd[k] == ' ') {
					R->right[j][l] = '\0';
					break;
				}
			}
			if (flag == 1) {
				R->right[j][l] = '\0';
			}
			
			j++;
			cmd[i] = ' ';
        }

        else if (cmd[i] == '>' && i<strlen(cmd)-1 && cmd[i+1] == '>')
        {
			R->op[j] = ">>";
			// printf("%s\n", R->op[j]);
            if (i > 1)
            {
				if (cmd[i-1] == '1' && cmd[i-2] == ' ')
                {
                    R->left[j][0] = '1';
					R->left[j][1] = '\0';
					cmd[i-1] = ' ';
                }
                else if (cmd[i-1] == '2' && cmd[i-2] == ' ')
                {
                    R->left[j][0] = '2';
					R->left[j][1] = '\0';
					cmd[i-1] = ' ';
                }
				else{
					R->left[j][0] = 'C';
					R->left[j][1] = '\0';
				}

            }

            if (i < strlen(cmd)-2)
            {
                if (cmd[i+2] == '&')
                {
                    if (cmd[i+3] == '1')
                    {
                        R->right[j][0] = '1';
						R->right[j][1] = '\0';
						cmd[i+3] =' ';
                    }
                    else if (cmd[i+3] == '2')
                    {
                        R->right[j][0] = '2';
						R->right[j][1] = '\0';
						cmd[i+3] =' ';
                    }
					cmd[i+2] = ' ';
                }
                else
                {
					int flag = 0, l=0;
					for (int k = i+2; k < strlen(cmd) ; k++) {
						// printf("%s\n", "inside");
						if (cmd[k] != ' ') {
							flag=1;
							// printf("%s\n","flag" );
							R->right[j][l] = cmd[k];
							cmd[k] = ' ';
							l++;
						}
						else if (flag == 1 && cmd[k] == ' ') {
							R->right[j][l] = '\0';
							break;
						}

					}
					if (flag == 1) {
						R->right[j][l] = '\0';
					}
					
                }


            }
			j++;
			cmd[i] = ' ';
			cmd[i+1] = ' ';
			i++;

        }

        else if (cmd[i] == '>')
        {
			R->op[j] = ">";
			// printf("%s\n", R->op[j]);
            if (i > 1)
            {
                if (cmd[i-1] == '1' && cmd[i-2] == ' ')
                {
                    R->left[j][0] = '1';
					R->left[j][1] = '\0';
					cmd[i-1] = ' ';
                }
                else if (cmd[i-1] == '2' && cmd[i-2] == ' ')
                {
                    R->left[j][0] = '2';
					R->left[j][1] = '\0';
					cmd[i-1] = ' ';
                }
                else
                {
					R->left[j][0] = 'C';
					R->left[j][1] = '\0';
                }
            }

            if (i < strlen(cmd)-3)
            {
                if (cmd[i+1] == '&')
                {
                    if (cmd[i+2] == '1')
                    {
                        R->right[j][0] = '1';
						R->right[j][1] = '\0';
						cmd[i+1]=' ';
                    }
                    else if (cmd[i+2] == '2')
                    {
                        R->right[j][0] = '2';
						R->right[j][1] = '\0';
						cmd[i+1]=' ';
                    }
					cmd[i+2]=' ';
                }
                else
                {
					int flag = 0, l=0;
					for (int k = i+1; k < strlen(cmd) ; k++) {
						// printf("%s\n", "inside");
						if (cmd[k] != ' ') {
							flag=1;
							// printf("%s\n","flag" );
							R->right[j][l] = cmd[k];
							cmd[k] = ' ';
							l++;
						}
						else if (flag == 1 && cmd[k] == ' ') {
							R->right[j][l] = '\0';
							break;
						}
					}
					if (flag == 1) {
						R->right[j][l] = '\0';
					}
			
                }

            }
			j++;
			cmd[i] = ' ';
        }

    }
	char * retc=malloc(MAX_INPUT_SIZE);
	strcat(retc,cmd);
	R->len = j;
	// printf("R Len %d\n",R->len );
	// printf("%s\n",retc );
	return retc;
}


// Function to extraxt the arguement for each process separated by pipes in Array arr
int extractArguments( char string[], char *arr[]) {
    int flag=0,j=0;
    arr[0]=malloc(20);
    char * s = string;
    int count = 0;

    while (*string != 0 ) {
        *s = *string++;

        if (*s != ' '){
            s++;
            count++;
            flag = 1;
        }
        else if(*s == ' ' && flag == 1){
            *s = '\0';
            s-=count;
            strcpy(arr[j++], s);
            arr[j]=malloc(20);
            flag = 0;
            count=0;
        }
    }
    if (flag == 1) {
        *s = '\0';
        s-=count;
        strcpy(arr[j++], s);
    }



    arr[j] = NULL;
    // for (int i = 0; i < j; i++) {
    //     printf("%s\n", arr[i] );
    // }
    return j;
}

// Signal handler for SIGINT (Ctrl+C)
void Handler(int sig){

    printf("\n");

    char cwd[200];
    getcwd(cwd, 200);
    printf(ANSI_COLOR_GREEN "%s$ " ANSI_COLOR_RESET, cwd);

    printf("\n");

    return;

}



// Main Execution function that calls exec sys call and handles all the piping and redirections
void executePipes(int pipes , struct Process P[], struct Redirect R) {
    int prev = 0, i = 0;
    int infil = -1;
	int outfil = -1;
	int erfil = -1;
	int isDupout = -1;
	int isDuper =-1;
    int fd[2];
    int pid;

	// printf("%d\n", R.len);
	for (int i = 0; i < R.len; i++) {
		if (R.left[i][0] == 'C' && strcmp(R.op[i], ">")==0) {
			R.left[i][0] = '1';
		}
		else if(R.left[i][0] == 'C' && strcmp(R.op[i], "<")==0) {
			R.left[i][0] = '0';
		}
		// printf("%s %s %s\n", R.left[i], R.op[i], R.right[i] );
	}

	for (int i = 0; i < R.len; i++){
		if (R.left[i][0] == '1' &&  strcmp(R.op[i], ">")==0 && R.right[i][0]!='2') {
			if (outfil != -1) {
				close(outfil);
			}
			isDupout=-1;
			char * retc=malloc(MAX_INPUT_SIZE);
			strcat(retc,R.right[i]);
			// printf(" lnk%s\n",retc );
			outfil = open (retc,O_WRONLY | O_CREAT, 0777);
		}
		else if (R.left[i][0] == '1' &&  strcmp(R.op[i], ">")==0){
			if (outfil != -1) {
				close(outfil);
			}
			// printf("%s\n", "outpuy" );
			isDupout = 1;
			outfil = -1;
		}
		else if (R.left[i][0] == '2' &&  strcmp(R.op[i], ">")==0 && R.right[i][0]!='1' ) {
			if (erfil != -1) {
				close(erfil);
			}
			isDuper = -1;
			char * retc=malloc(MAX_INPUT_SIZE);
			strcat(retc,R.right[i]);
			erfil = open (retc,O_WRONLY | O_CREAT, 0777);
		}
		else if (R.left[i][0] == '2' &&  strcmp(R.op[i], ">")==0){
			if (erfil != -1) {
				close(erfil);
			}
			isDuper = 1;
			erfil = -1;
		}
		else if ( strcmp(R.op[i], "<")==0 ) {
			if (infil != -1) {
				close(infil);
			}
			char * retc=malloc(MAX_INPUT_SIZE);
			strcat(retc,R.right[i]);
			infil = open (retc,O_RDONLY | O_CREAT, 0777);
		}
		else if (strcmp(R.op[i], ">>")==0) {
			if (outfil != -1) {
				close(outfil);
			}
			isDupout = -1;
			char * retc=malloc(MAX_INPUT_SIZE);
			strcat(retc,R.right[i]);
			//printf("%s\n",retc);
			outfil = open (retc, O_APPEND | O_CREAT | O_RDWR, 0777);
		}
	}


    while (i < pipes) {

        int ret = pipe(fd);
        int pipe_sz = fcntl(fd[1], F_SETPIPE_SZ, 1000000);
        if (ret == -1) {
            printf("%s\n", "Cannot create pipe");
        }
		if (strcmp(P[i].name,"cd")==0) {
			chdir(P[i].argu[1]);
			break;
		}
        if ((pid = fork())==0) {
            if (i > 0) {
                close(0);
                dup(prev);
                close(prev);
            }
			else {
				if (infil !=-1) {
					close (0);
					dup(infil);
					close(infil);
				}
			}
            if (i != pipes-1) {
                close(1);
                dup(fd[1]);
                close(fd[1]);
            }
			else{
				if (outfil != -1) {
					// printf("%d\n",outfil );
					close (1);
					dup(outfil);
					close(outfil);
				}
				else if(isDupout != -1){
					close (1);
					dup(2);
					dup(2);
				}
				if (erfil != -1) {
					close(2);
					dup(erfil);
					close(erfil);
				}
				else if(isDuper != -1){
					close (2);
					dup(1);
					dup(1);
				}
			}
            close(fd[0]);
            // printf("Inside %s\n", P[i].name);
            // printf("At 0 : %s\n", P[0].name);
            execvp(P[i].name, P[i].argu);
            printf("%s\n","Could not exec" );
            exit(1);


        }
        else if(pid > 0){
			if(BGRND == 0){
			   waitpid(pid, NULL, 0);
		   }
		  // wait(NULL);
            close(fd[1]);
            prev = fd[0];
            i++;
        }
        else{
            printf("%s\n", "Could not fork");
            exit(1);
        }
    }

}


// Need I say anything here?
int main (int argc, char const *argv[]) {

    char * cmd;
    char cwd[200];

    while(1) {
		signal(SIGINT, Handler);

		BGRND = 0;
        getcwd(cwd, 200);
		printf(ANSI_COLOR_GREEN "%s$ " ANSI_COLOR_RESET, cwd);
		cmd = input ();
        // printf("%s\n", cmd);
		
        if (strlen(cmd) == 0) {
			continue;
		}
        
        if (strcmp(cmd,"exit") == 0) {
            exit(0);
        } 

        else {

            char arr[MAX_INPUT_SIZE][MAX_INPUT_SIZE];
            char param[MAX_INPUT_SIZE];
            strcpy(param,cmd);

            int splitLen = splitPipe2(param, arr)-1;
            // printf("%d\n",splitLen );


            struct Process P[splitLen+1];
            for(int k=0; k<splitLen-1; k+=1){

    	    	char current[MAX_INPUT_SIZE];
        		strcpy(current, arr[k]);

    	    	// P[k].name = extractCommand(current);

    	    	// strcpy(current, Commands[k]);


    	    	//P[k].argu = args;
    	    	char * args[20];

    	    	int argLen = extractArguments(current, args);

    	    	P[k].name = args[0];

    	    	for (int l = 0; l < argLen+1; ++l) {
    	    		P[k].argu[l] = args[l];
    	    	}

    	    	P[k].len = argLen;

    	    }

			char current[MAX_INPUT_SIZE];
			struct Redirect R;
			char * arraycaucb[10];
			char * comd = redirection(arr[splitLen-1], &R, arraycaucb );
			strcpy(current, comd );
			// printf("%s\n",  current);
			char * args[20];
			int argLen = extractArguments(current, args);

			P[splitLen-1].name = args[0];
			// Check if the name ends with &
		    int length = strlen(P[splitLen-1].name);

		    // printf("Lenght %d\n", length);

		    if(P[splitLen-1].name[length-1] == '&'){
			   BGRND = 1;
			   P[splitLen-1].name[length-1] = '\0';
		    }


			for (int l = 0; l < argLen+1; ++l) {
				P[splitLen -1].argu[l] = args[l];
			}

			P[splitLen -1].len = argLen;

			// printf(" print %d\n", R.len);
			// for (int i = 0; i <R.len ; i++) {
			// 	printf("%s %s %s \n", R.left[i], R.op[i], R.right[i] );
			// }
            // printf("%s\n", "Printing names in struct");
			//
            // for(int i=0; i<splitLen; i+=1){
            //     printf("%s\n", P[i].name);
            // }

            // printf("%s\n", cmd);
			// printf("Just %d\n", R.len);
            // printf("Pipe %d\n", splitLen);
            executePipes(splitLen,P, R);

        }

        free(cmd);
	}
}
