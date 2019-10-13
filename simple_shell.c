#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_LINE 80
#define HISTORY_SIZE 10

struct HistoryCommands {
    int count;
    char *commands[HISTORY_SIZE];
};

int separate_tokens(char *command, char *args[]) {
    int count = 0;
    char *p;
    int i;
    for (i = 0, p = strtok(command, " "); p && (i < MAX_LINE); p = strtok(NULL, " ")) {
        args[i] = (char *) (malloc(MAX_LINE * sizeof(char)));
        strcpy(args[i], p);
        count++;
        i++;
    }
    for (; i < MAX_LINE / 2 + 1; ++i) args[i] = NULL;
    return count;
}

void copy_command(char *command, struct HistoryCommands *history) {
    strcpy(command, history->commands[history -> count - 1]);
    printf("%s\n", command);
    return;
}

void cd_command(char *args[], int num_args) {
    char *path = args[1];
    char *home = (char *) (malloc(MAX_LINE * sizeof(char)));
    strcpy(home, getenv("HOME"));
    if (num_args == 1) {
        chdir(home);
        return;
    }
    if (args[1][0] == '~') {
        if ((strlen(args[1]) == 1) || (strlen(args[1]) == 2 && args[1][1] == '/')) {
            chdir(home);
            return;
        }
        if (strlen(args[1]) > 2 && args[1][1] == '/') {
            path = strcat(strcat(home, "/"), args[1] + 2);
        }
    }
    if (chdir(path) == -1) {
        printf("%s : No such file or directory.\n", args[1]);
    }
    return;
}

int check_exclamation_mark(char *command, struct HistoryCommands *history) {
    char *args_temp[MAX_LINE / 2 + 1];
    char *command_temp = (char *) (malloc(MAX_LINE * sizeof(char)));
    int num_args = 0;
    strcpy(command_temp, command);
    num_args = separate_tokens(command_temp, args_temp);
    if (num_args == 0) return 1;
    if (args_temp[0][0] == '!') {
        if (num_args > 1) {
            printf("Please use only one parameter.\n");
            return 1;
        }
        if (strcmp(args_temp[0], "!!") == 0) {
            if (history->count == 0) {
                printf("No commands in history.\n");
                return 1;
            }
            else
                copy_command(command, history);
        }
        else {
            printf("Event not found.\n");
            return 1;
        }
    }
    return 0;
}

void store_command(char *command, struct HistoryCommands *history) {
    if (history->count != HISTORY_SIZE) {
        history->commands[history->count] = (char *) malloc(MAX_LINE * sizeof(MAX_LINE));
        strcpy(history->commands[history->count++], command);
    }
    else {
        for (int i = 0; i < HISTORY_SIZE - 1; ++i)
            strcpy(history->commands[i], history->commands[i + 1]);
        strcpy(history->commands[HISTORY_SIZE - 1], command);
    }
    return;
}

int check_pipe(int num_args, char *args[]){
    int i;
    for (i = 0; i < num_args; i++)
        if (strcmp(args[i], "|") == 0)
            return 1;
    return 0;
}

int main(void)
{
    struct HistoryCommands history;
    char *args[MAX_LINE / 2 + 1];
    int should_run = 1;
    history.count = 0;
    int fd;
    while (should_run)
    {
        char *command = (char *) (malloc(MAX_LINE * sizeof(char)));
        int num_args = 0;
        int check_ampersand = 0;
        int my_pipe[2];

        printf("osh> ");
        fflush(stdout);
        fgets(command, MAX_LINE, stdin);
        command[strlen(command) - 1] = '\0';

        if (strlen(command) == 0) continue;

        if (check_exclamation_mark(command, &history) == 1) continue;

        store_command(command, &history);
        num_args = separate_tokens(command, args);

        if (check_pipe(num_args, args) == 1)
        {
            char *args_1[MAX_LINE / 2 + 1];
            int k = 0;
            int num_args_1 = 0;
            for (int i = 0; i < num_args; i++)
            {
                if (strcmp(args[i], "|") == 0){
                    k = i;
                    continue;
                }
                if (k != 0)
                {
                    args_1[num_args_1] = (char *) (malloc(MAX_LINE * sizeof(char)));
                    strcpy(args_1[num_args_1], args[i]);
                    num_args_1 ++;
                }
            }
            args_1[num_args_1] = NULL; 
            while (num_args - 1 > k)
            {
                args[num_args - 1] = NULL;
                num_args--;
            }
            args[num_args - 1] = NULL;
            num_args--;

            pipe(my_pipe);
            pid_t new_pid;
            new_pid = fork();
            if(new_pid == 0)
            {
                dup2(my_pipe[1], 1);
                close(my_pipe[0]);
                close(my_pipe[1]);
                execvp(args[0], args);
                fprintf(stderr, "Failed to execute '%s'\n", args[0]);
                exit(0);
            }
            else
            { 
                new_pid=fork();

                if(new_pid==0)
                {
                    dup2(my_pipe[0], 0);
                    close(my_pipe[1]);
                    close(my_pipe[0]);
                    execvp(args_1[0], args_1);
                    fprintf(stderr, "Failed to execute '%s'\n", args_1[0]);
                    exit(0);
                }
                else
                {
                    int status;
                    close(my_pipe[0]);
                    close(my_pipe[1]);
                    waitpid(new_pid, &status, 0);
                }
            }
            continue;
        }
        if (num_args >= 3)
        {
            if (strcmp(args[num_args - 2], ">") == 0)
            {                
                char *file_name = (char *) (malloc(MAX_LINE * sizeof(char)));
                file_name = strcpy(file_name, args[num_args - 1]);
                args[num_args - 1] = NULL;
                num_args--;
                args[num_args - 1] = NULL;
                num_args--;
                pid_t new_child = fork();
                if (new_child == 0)
                {
                    fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC , 400);
                    if (strcmp(args[0], "history") == 0)
                    {
                        FILE *fptr;
                        fptr = fopen(file_name, "w");
                        for (int i = history.count; i > 0; i--)
                            fprintf(fptr, "%d %s\n", i, history.commands[i - 1]);
                        exit(0);
                    }
                    dup2(fd, 1);
                    execvp(args[0], args);
                    exit(0);
                }   
                else
                    continue;
            }
            else
            {
                if (strcmp(args[num_args - 2], "<") == 0)
                {
                    char *file_name = (char *) (malloc(MAX_LINE * sizeof(char)));
                    file_name = strcpy(file_name, args[num_args - 1]);
                    args[num_args - 1] = NULL;
                    num_args--;
                    args[num_args - 1] = file_name;
                    pid_t new_child = fork();
                    if (new_child == 0)
                    {
                        //fd = open(file_name, O_RDONLY, 644);
                        //dup2(fd, 0);
                        execvp(args[0], args);
                        exit(0);
                    }
                    else{
                        waitpid(new_child, NULL, 0);
                        continue;
                    }
                }
            }
                
        }                  
        
        if (strcmp(args[num_args - 1], "&") == 0)
        {
            check_ampersand = 1; 
            args[num_args - 1] = NULL;
            num_args--;
        }

        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            continue;
        }

        if (strcmp(args[0], "cd") == 0)
        {
            cd_command(args, num_args);
            continue;
        }

        if (strcmp(args[0], "history") == 0)
        {
            for (int i = history.count; i > 0; i--)
                printf("%d %s\n", i, history.commands[i - 1]);
            continue;
        }
        
        pid_t new_pid = fork();
        if (new_pid == 0)
        {
            execvp(args[0], args);
            printf("%s : Command not found.\n", args[0]);
            exit(0);
        }

        if (check_ampersand == 0)
            waitpid(new_pid, NULL, 0);

        
    }
    return 0;
}