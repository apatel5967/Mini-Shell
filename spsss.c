
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <sysexits.h>
#include "spsss.h"
#include "split.h"
#include "safe-fork.h"

Spsss_commands read_spsss_commands(const char compile_cmds[],
                                   const char test_cmds[]) {
    int i = 0 , count1 = 0, count2 = 0;
    char buffer[LINE_MAX];
    Spsss_commands commands;
    FILE* file1 = fopen(compile_cmds, "r");
    FILE* file2 = fopen(test_cmds, "r");

    commands.compile = NULL;
    commands.test = NULL;
    commands.compile_size = 0;
    commands.test_size = 0;
    if(file1 == NULL || file2 == NULL) {
        perror("Could not open file");
        exit(1);
    }

    while((fgets(buffer, sizeof(buffer), file1)) != NULL && !feof(file1)) {
        count1++;
    }

    while((fgets(buffer, sizeof(buffer), file2)) != NULL && !feof(file2)) {
        count2++;
    }

    commands.compile = malloc(count1 * sizeof(char*));
    if(commands.compile == NULL) {
        perror("Allocation failure");
        exit(1);
    }

    commands.test = malloc(count2 * sizeof(char*));
    if(commands.test == NULL) {
        perror("Allocation failure");
        exit(1);
    }

    fseek(file1, 0, SEEK_SET);

    while((fgets(buffer, sizeof(buffer), file1)) != NULL) {
        commands.compile[i] = malloc(strlen(buffer) * sizeof(char) +1);
        strcpy(commands.compile[i] , buffer);
        commands.compile_size++;
        i++;
    }
    fclose(file1);

    fseek(file2, 0, SEEK_SET);
    i = 0;
    while((fgets(buffer, sizeof(buffer), file2)) != NULL) {
        commands.test[i] = malloc(strlen(buffer) * sizeof(char) +1);
        strcpy(commands.test[i] , buffer);
        commands.test_size++;
        i++;
    }

    fclose(file2);
    return commands;
}

void clear_spsss_commands(Spsss_commands *const commands) {
    int i, j;
    if(commands == NULL) {
        return ;
    }
    
    for(i = 0; i < commands->compile_size; i++) {
        free(commands->compile[i]);
    }
    free(commands->compile);

    for(j = 0; j < commands->test_size; j++) {
        free(commands->test[j]);
    }
    free(commands->test);

    commands->compile = NULL;
    commands->compile_size = 0;
    commands->test = NULL;
    commands->test_size = 0;
}

int compile_program(Spsss_commands commands){
    pid_t pid;
    int count = 0; 
    int i = 0;
    char ** arr = NULL;
    while(count < commands.compile_size) {
        pid = safe_fork();

        if(pid == 0) {
            arr = split(commands.compile[count]);
            if(execvp(arr[0], arr) == -1) {
                while(arr[i] != NULL) {
                    free(arr[i]);
                    i++;
                }
                free(arr);
                clear_spsss_commands(&commands);
                perror("execvp");
                exit(FAILED_COMPILATION);
            }
            
            exit(0);
        } else if (pid > 0) {
            int status;
            if(waitpid(pid, &status, 0) == -1) {
                perror("child failure");
                exit(FAILED_COMPILATION);
            }

            if(WIFEXITED(status)) {
                if(WEXITSTATUS(status)) {
                    return FAILED_COMPILATION;
                }
            } else {
                return FAILED_COMPILATION;
            }
        

        } else {
            err(EX_OSERR, "fork error");
        }
        count++;
    }
    return SUCCESSFUL_COMPILATION;
}

int test_program(Spsss_commands commands) {
    pid_t pid;
    int count = 0;
    int success = 0;
    int fd;
    char **arr = NULL; 
    int i = 0;

    while(count < commands.test_size) {
        pid = safe_fork();

        if(pid == 0) {
            arr = split(commands.test[count]);
            while(arr[i] != NULL) {
                if(strcmp(arr[i], "<") == 0) {
                    fd = open(arr[i+1], O_RDONLY);

                    if(fd == -1) {
                        perror("input redirection");
                        exit(1);
                    }

                    dup2(fd, STDIN_FILENO);
                    close(fd);

                    arr[i] = NULL;
                    arr[i+1] = NULL;
                    i++;
                } else if(strcmp(arr[i], ">") == 0) {
                    fd = open(arr[i+1], FILE_FLAGS, FILE_MODE);

                    if(fd == -1) {
                        perror("output redirection");
                        exit(1);
                    }

                    dup2(fd, STDOUT_FILENO);
                    close(fd);

                    arr[i] = NULL;
                    arr[i+1] = NULL;
                    i++;
                }
                i++;
            }
            
            if(execvp(arr[0], arr) == -1) {
                perror("execvp");
                exit(1);
            }
            exit(0);
        } else if (pid > 0) {
            int status;
            if(waitpid(pid, &status, 0) == -1) {
                perror("child failure");
                exit(FAILED_COMPILATION);
            }

            if(WIFEXITED(status) == 1 && WEXITSTATUS(status) == 0) {
                success++;
            }

        } else {
            err(EX_OSERR, "fork error");
        }
        count++;
    }
    return success;
}
