#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

char * input (void) {
    char * line = (char *) malloc( 100 );
    memset(line,0,strlen(line));
    char a ='\0', c='\0';

    while (1) {
        scanf("%c", &a );
        if (a != '\n') {
            strncat(line, &a,1);
        }
        else{
            break;
        }
    }
    strncat(line, &c, 1);
    return line;
}


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

void execute (char * addr, char * attributes) {

    int pid;
    char *args[2];

    strcpy(args[0],addr);
    args[1]=NULL;

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

void splitPipe( char * command, char * array[]) {

    array[0] = strsep(&command, "|");
    array[1] = strsep(&command, "|");

    //printf("%s %s\n", array[0], array[1] );
}

char * removespaces(char * string) {
    char * s= string;
    int count = 0;
    while (*string != 0 ) {
        *s = *string++;
        if ( *s != ' ') {
            s++;
            count+=1;
        }
    }
    *s = 0;
    s-=count;
    // printf("%s\n",s );
    return s;
}

int main (int argc, char const *argv[], char **envp) {
    char * cmd;
    while(1) {
		printf("$ ");
		cmd = input ();
        if (strcmp(cmd,"exit")==0) {
            exit(0);
        }
        else {
            char * array[2];
            splitPipe (cmd,array);
            //printf("%s\n", array[0]);
            char * r = removespaces(array[0]);
            char * filePath = returnFile(r);
            //printf("%s\n", filePath);
            if (strcmp(filePath,"Not Found")!=0) {
                execute(filePath);
            }
            else{
                printf("%s\n", filePath );
            }

        }
	}
}
