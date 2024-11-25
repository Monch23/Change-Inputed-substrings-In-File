#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <signal.h>

int main() {
    const size_t shMemSize = getpagesize();
    int fd = open("shared.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    char *shMem = (char*)mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, 
    MAP_SHARED, fd, 0);
    if (!shMem) {
        perror("mmap()");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (!pid) { // child process
        raise(SIGSTOP);
        puts("Child Process \n");
        char src[100];
        memset(src, 0, sizeof(src));
        char dest[100];
        memset(dest, 0, sizeof(dest));

        puts("Input string what you want to change");
        fgets(src, 100, stdin);
        src[strlen(src) - 1] = '\0';
        puts("Input string what you want to replace");
        fgets(dest, 100, stdin);
        dest[strlen(dest) - 1] = '\0';
        if (strlen(src) != strlen(dest)) {
            printf("Conflicting sizes \n");
            exit(EXIT_FAILURE);
        }
        char *shPtr = shMem;

        while ((shPtr = strstr(shMem, src))) {
            strncpy(shPtr, dest, strlen(dest));
            shPtr += strlen(dest);   
        }

        exit(EXIT_SUCCESS);
    } else if (pid > 0) { // parent process
        puts("Parent Process \n");
        char src[100];
        char dest[100];

        puts("Input string what you want to change");
        fgets(src, 100, stdin);
        src[strlen(src) - 1] = '\0';
        puts("Input string what you want to replace");
        fgets(dest, 100, stdin);
        dest[strlen(dest) - 1] = '\0';
        if (strlen(src) != strlen(dest)) {
            printf("Conflicting sizes \n");
            exit(EXIT_FAILURE);
        }
        system("clear");
        kill(pid, SIGCONT);
        char *shPtr = shMem;

        while ((shPtr = strstr(shMem, src))) {
            strncpy(shPtr, dest, strlen(dest));
            shPtr += strlen(dest);   
        }

        wait(NULL);
    }

    munmap(shMem, shMemSize);
    close(fd);
    return 0;
}