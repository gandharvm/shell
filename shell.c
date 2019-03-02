#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

// Take input character by character
char * input (void) {
    char * line = (char *) malloc( 100 );
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
    char * prefixDir = "/bin/";
    char * filePath;
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
        strcpy (filePath,prefixDir);
        strcat (filePath, fileName);
    }
    else{
        filePath = "Not Found";
    }

    // printf("%s\n",filePath );
    return filePath;
}



/*
	Passing file address and dong exec system call

	Also pass attributes, and arguments

*/


//void execute (char * addr, char * attributes) {

void execute (char * addr, char * attri) {

    int pid;
    char *args[3];

    int i=0;
    strcpy(args[0],addr);
    if (attri) {
        printf("%s\n"," NOt Null" );
        args[++i]=attri;
    }
    args[2]=NULL;


    if ((pid = fork())==0) {
        execv(args[0],args);
    }
    else if(pid>0){
        wait();
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
    printf("%s\n", s);
    return s;
}

int main (int argc, char const *argv[]) {
    char * cmd;
    while(1) {
		printf("$ ");
		cmd = input ();
        printf("%s\n", cmd);
        if (strcmp(cmd,"exit")==0) {
            exit(0);
        }
        else {
            char * array[2];
            splitPipe (cmd,array);
            //printf("%s\n", array[0]);
             char * attributes = extractAttributes(array[0]);
            printf("%s\n",attributes );
            //char * r = removespaces(array[0]);
            char * filePath = returnFile(array[0]);
            printf("%s\n", filePath);

            if (strcmp(filePath,"Not Found")!=0) {
                execute(filePath,attributes);
            }
            else{
                printf("%s\n", filePath );
            }

        }
	}
}
