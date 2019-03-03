#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 150

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


// Return filepath from filename
// Search all of path env also
// 1. Bin 2. User Bin 3. PWD - Now only in BIN
char * returnFile ( char * fileName ) {
    DIR * x;
    struct dirent * files;
    char * prefixDir = "./";
    char * filePath = malloc (100) ;
    int flag = 0;

    x = opendir(prefixDir);
    // printf("Opened dir\n" );
    while ((files = readdir(x))!= NULL) {
        if (strcmp(fileName, files->d_name)==0) {
            // printf("Found file\n" );
            flag=1;
            break;
        }
    }

    closedir(x);

    if (flag==1) {
        // printf("%s\n", "Found in cur");
        strcpy (filePath, prefixDir);
        strcat (filePath, fileName);
    }
    else {
        prefixDir = "/bin/";
        x = opendir(prefixDir);
        // printf("Opened dir\n" );
        while ((files = readdir(x))!= NULL) {
            if (strcmp(fileName, files->d_name)==0) {
                // printf("Found file\n" );
                flag=1;
                break;
            }
        }

        closedir(x);

        if (flag==1) {
            strcpy (filePath, prefixDir);
            strcat (filePath, fileName);
        }
        else{

            prefixDir = "/usr/bin/";
            x = opendir(prefixDir);
            // printf("Opened dir\n" );
            while ((files = readdir(x))!= NULL) {
                if (strcmp(fileName, files->d_name)==0) {
                    // printf("Found file\n" );
                    flag=1;
                    break;
                }
            }

            closedir(x);

            if (flag==1) {
                strcpy (filePath, prefixDir);
                strcat (filePath, fileName);
            }
            else {
                filePath = "Not Found";
            }
        }
    }
    // printf("%s\n",filePath );
    return filePath;
}



/*
	Passing file address and dong exec system call

	Also pass attributes, and arguments

*/


//void execute (char * addr, char * attributes) {

void execute (char * addr,int argscount, char * arguments[]) {

    int pid, status;
    char *args[argscount+1];

    int i=0;
    args[i] = addr;
    for (int j = 1; j < argscount; j++) {
        args[++i] = arguments [j];
    }
    args[++i]=NULL;


    if ((pid = fork())==0) {
        execv(args[0],args);
    }
    else if(pid>0){
        wait(&status);
    }
    else{
        printf("Cannot execute\n");
    }

}

// To split the command by pipe and store it into an array

void splitPipe( char * command, char * array[]) {

    array[0] = strsep(&command, "|");
    array[1] = strsep(&command, "|");

    //printf("%s %s\n", array[0], array[1] );
}

// Just to remove spaces from a char array

char * removespaces(char * string) {
    char * s= string;
    int count = 0;
    while (*string != 0 ) {
        *s = *string++;
        if ( *s != ' ' && *s != '\t') {
            s++;
            count+=1;
        }
    }
    *s = 0;
    s-=count;
    // printf("%s\n",s );
    return s;
}

char * extractAttributes( char string[]) {
    int flag=0,flaggy=0;
    string[strlen(string)]=' ';
    string[strlen(string)]='\0';
    // printf("%s%s\n",string,"Added space" );

    char * s= string;
    int count = 0;
    while (*string != 0 ) {
        *s = *string++;
        if ( *s == '-' && flag == 0 && flaggy == 0) {
            s++;
            count++;
            flag = 1;
            flaggy = 1;
        }
        else if (*s == '-' && flag == 0 && flaggy == 1){
            flag = 1;
        }
        else if(*s != ' ' && flag == 1){
            s++;
            count++;
        }
        else {
            flag = 0;
        }
    }
    *s = '\0';
    s-=count;

    if (count == 0) {
        s = "No attributes";
    }
    // printf("%s\n", s);
    return s;
}

char * extractCommand( char string[]) {
    int flag=0;
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
            break;
        }
    }
    *s = '\0';
    s-=count;
    // printf("%s\n", s);
    return s;
}

int extractArguments( char string[], char *arr[]) {
    int flag=0,j=0;
    arr[0]=malloc(10);
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
            arr[j]=malloc(10);
            flag = 0;
            count=0;
        }
    }
    if (flag == 1) {
        *s = '\0';
        s-=count;
        strcpy(arr[j++], s);
    }

    // for (int i = 0; i < j; i++) {
    //     printf("%s\n", arr[i] );
    // }
    return j;
}

int main (int argc, char const *argv[]) {
    char * cmd;
    while(1) {
		printf("$ ");
		cmd = input ();
        // printf("%s\n", cmd);
        if (strcmp(cmd,"exit")==0) {
            exit(0);
        }
        else {
            char param[MAX_INPUT_SIZE], params[MAX_INPUT_SIZE];
            strcpy(param,cmd);
            char * command = extractCommand(param);
            // printf("%s\n", command );
            strcpy(params,cmd);
            char * arr[20];
            int num = extractArguments(params,arr);
            // printf("%d\n", num );
            // char * attributes = extractAttributes(params);
            // printf("%s\n", attributes );
            //char * array[2];
            //splitPipe (cmd,array);
            //printf("%s\n", array[0]);
            //char * r = removespaces(array[0]);
            char * filePath = returnFile(command);
            // printf("%s\n", filePath);
            if (strcmp(filePath,"Not Found")!=0) {
                if (num == 1) {
                    execute(filePath, num, NULL);
                }
                else{
                    execute(filePath, num, arr);
                }
            }
            else{
                printf("%s\n", filePath );
            }
            for (int i = 0; i < num; i++) {
                free(arr[i]);
            }
            free(filePath);
        }
        free(cmd);
	}
}
