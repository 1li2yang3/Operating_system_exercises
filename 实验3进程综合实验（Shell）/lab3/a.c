#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 100

int running = 1; // 用于控制shell的运行状态

void sigint_handler(int sig) {
    printf("\nExiting shell...\n");
    running = 0; // 设置运行状态为0，退出shell
}

void execute_command(char *cmd) {
    char *args[MAX_ARGS];
    char *input_file = NULL, *output_file = NULL;
    int background = 0;

    // 处理管道
    char *pipe_pos = strchr(cmd, '|');
    if (pipe_pos) {
        *pipe_pos = '\0'; // 分割命令

        // 创建管道
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("Pipe creation failed");
            return;
        }

        // 执行左侧命令
        if (fork() == 0) {
            dup2(pipefd[1], STDOUT_FILENO); // 输出到管道
            close(pipefd[0]);
            close(pipefd[1]);
            execute_command(cmd); // 递归执行左侧命令
            exit(0);
        }

        // 执行右侧命令
        if (fork() == 0) {
            dup2(pipefd[0], STDIN_FILENO); // 输入来自管道
            close(pipefd[1]);
            close(pipefd[0]);
            execute_command(pipe_pos + 1); // 递归执行右侧命令
            exit(0);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        wait(NULL); // 等待左侧进程结束
        wait(NULL); // 等待右侧进程结束
        return;
    }

    // 处理重定向和背景运行
    char *token = strtok(cmd, " \n");
    int i = 0;
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1; // 设置后台标志
            break;
        } else if (strcmp(token, ">") == 0) {
            output_file = strtok(NULL, " \n");
        } else if (strcmp(token, "<") == 0) {
            input_file = strtok(NULL, " \n");
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    pid_t pid;
    if ((pid = fork()) == 0) { // 子进程
        if (input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in < 0) {
                perror("Input redirection failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if (output_file) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("Output redirection failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        execvp(args[0], args);
        perror("Command execution failed");
        exit(EXIT_FAILURE);
    }

    // 在后台运行
    if (background) {
        printf("Running command in background with PID: %d\n", pid);
    } else {
        wait(NULL); // 等待前台进程结束
    }
}

int main() {
    char *cmd;

    signal(SIGINT, sigint_handler); // 捕获Ctrl+C信号

    while (running) {
        cmd = readline("lab3>"); // 使用readline获取输入
        if (cmd == NULL) break; // EOF，退出

        if (strcmp(cmd, "exit") == 0) { // 退出
            free(cmd);
            break;
        }

        add_history(cmd); // 将命令添加到历史中,直接调用readline中的函数
        execute_command(cmd);
        free(cmd); // 释放命令字符串
    }

    return 0;
}

