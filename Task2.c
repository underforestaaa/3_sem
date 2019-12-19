#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    int txd[2]; 
    int rxd[2]; 
} dpipe_t;

int main() {
    while(1) {
        char buf[4096];
        int size;
        dpipe_t dpipe;
        if ((pipe(dpipe.rxd) < 0) | (pipe(dpipe.txd) < 0)) {
            printf("Pipe creation is failed!\n");
            return -1;
        }
        int y = getpid();
        const pid_t pid = fork();
        if (pid < 0) {
            printf("fork failed!\n");
            return -1;
        }
        if (pid) {
            close(dpipe.txd[0]);
            close(dpipe.rxd[1]);
            while((size = read(0, buf, sizeof(buf)-1)) > 0) {
                if (waitpid(pid, NULL, WNOHANG) != 0) {
                    printf("Child terminated\nParent exiting\n");
                    exit(0);
                }
                buf[size] = 0;
                write(dpipe.txd[1], buf, size);
                printf("Parent sent: %s\n", buf);
                sleep(1);
                size = read(dpipe.rxd[0], buf, sizeof(buf)-1);

                buf[size] = '\0';
                printf("Received from child: %s\n", buf);
            }
        }
        else {
            close(dpipe.txd[1]);
            close(dpipe.rxd[0]);
            while((size = read(dpipe.txd[0], buf, sizeof(buf)-1)) > 0) {
                if (getppid() != y)
                    break;
                buf[size] = '\0';
                printf("Received from parent: %s\n", buf);
                write(dpipe.rxd[1], buf, size);
                printf("Child sent: %s\n", buf);
            }
            printf("Parent terminated\nChild exiting\n");
            exit(0);
        }
    }
}
