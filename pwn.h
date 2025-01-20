#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    int std_in;
    int std_out;
    //int std_err;
    pid_t pid;
    char* name;
    int ret;
} process;

process* new_process(char* path, char* argv[]) {
    process* proc = (process*) malloc(sizeof(process));
    memset(proc, 0, sizeof(process));
    proc->name = path;
    int read_pipe[2];
    int write_pipe[2];
    //int error_pipe[2];
    if (pipe(read_pipe) == -1) { exit(EXIT_FAILURE); }
    if (pipe(write_pipe) == -1) { exit(EXIT_FAILURE); }
    //if (pipe(error_pipe) == -1) { exit(EXIT_FAILURE); }
    if ((proc->pid = fork()) == 0) {
        close(read_pipe[1]);
        close(write_pipe[0]);
        //close(error_pipe[0]);
        if (dup2(read_pipe[0], STDIN_FILENO) == -1) {
            perror("Failed to dup stdin");
            exit(EXIT_FAILURE);
        }
        if (dup2(write_pipe[1], STDOUT_FILENO) == -1) {
            perror("Failed to dup stdout");
            exit(EXIT_FAILURE);
        }
        /*
        if (dup2(error_pipe[1], STDERR_FILENO) == -1) {
            perror("Failed to dup stderr");
            exit(EXIT_FAILURE);
        }
        */
        int ret = execve(path, argv, NULL);
        if (ret < 0) {
            perror("Failed to start process");
            exit(EXIT_FAILURE);
        }
        proc->ret = ret;
    } else {
        close(read_pipe[0]);
        close(write_pipe[1]);
        //close(error_pipe[1]);
        proc->std_in = read_pipe[1];
        proc->std_out = write_pipe[0];
        //proc->std_err = error_pipe[0];
    }
    return proc;
}

int proc_write(process* proc, char* input) {
    int bytes_written = 0;
    if ((bytes_written = write(proc->std_in, input, strlen(input))) == -1) {
        perror("Failed to write to process stdout"); 
        exit(EXIT_FAILURE);
    }
    return bytes_written;
}

int proc_read(process* proc, char* buf, int len) {
    int bytes_read = 0;
    if ((bytes_read = read(proc->std_out, buf, len)) == -1) {
        perror("Failed to read to process stdin");
        exit(EXIT_FAILURE);
    }
    return bytes_read;
}
