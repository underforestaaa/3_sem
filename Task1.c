#include <unistd.h> //fork
#include <stdio.h> //printf fgets stdin
#include <string.h> //strcmp strtok strlen
#include <stdlib.h> //malloc
#include <sys/wait.h> //waitpid WEXITSTATUS

int main() {
    int arg_max = sysconf(_SC_ARG_MAX); //get the maximum possible length of a command
    char* cmd = malloc(arg_max); //allocate memory for the command
    while(1) {
        printf("Enter command: ");
        int cmdlen = strlen(fgets(cmd, arg_max, stdin)); 
        
        if(!strcmp(cmd, "exit")) 
            return 0;

        if(strspn(cmd, " ") == strlen(cmd)) { //ignore commands consisting only of spaces
            printf("Incorrect command\n");
            continue;
        }

        int c_pid = fork(); //fork into two

        int status = c_pid;
        if(c_pid == 0) { //act as child

            int argc = (cmd[0] != ' '); 

            for(int i = 0; cmd[i]; i++) 
                if(cmd[i] == ' ' && cmd[i+1] != ' ' && cmd[i+1] != 0)
                    argc++;

            char** argv = malloc(argc * sizeof(char*)); //allocate memory for arguments

            char* name = strtok(cmd, " ");
            argv[0] = name;

            for(int i = 1; i < argc; i++) 
                argv[i] = strtok(NULL," ");
            execvp( 
                name, 
                argv 
                );
            return 42; 
        }
        else { //act as parent
            waitpid(c_pid, &status, 0); 
            status = WEXITSTATUS(status); 
        }
        if(status == 42) 
            printf("Incorrect command\n");
        else
            printf("Exit status: %d\n", status);
    }
    return 0;
}
