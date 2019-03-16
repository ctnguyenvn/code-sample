#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

#define BUFF_SIZE 1024
#define True    1

int copy_file(int, int);

int check_is_dir(char *);

int main(int argc, char *argv[]) {
    int fd_from, fd_to;
    int check_file_exits;
    char status[3];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file from> <file to>\n", argv[0]);
        return 1;
    }

    if (check_is_dir(argv[1])) {
        fprintf(stderr, "%s: is directory\n", argv[1]);
        fprintf(stderr, "Usage: %s <file from> <file to>\n", argv[0]);
        return 1;
    }

    if (check_is_dir(argv[2])) {
        strcat(argv[2], "/");
        strcat(argv[2], basename(argv[1]));
    }

    check_file_exits = access(argv[2], F_OK);
    if (check_file_exits != -1) {
        printf("%s: File exits\nDo you want replace ([Y]/n): ", argv[1]);
        fgets(status, 3, stdin);
        strtok(status, "\n");
        if ((strcmp(status, "n")) == 0) {
            exit(0);
        }
    }
    
    fd_from = open(argv[1], O_RDONLY);
    if (fd_from < 0) {
        perror("Error open");
        return 1;
    }

    fd_to = open(argv[2], O_CREAT | O_WRONLY, 0644);
    copy_file(fd_from, fd_to);
    printf("Copy to new file complete !!!");

    return 0;
}

int copy_file(int fd_from, int fd_to) {
    off_t data_len;
    char data[BUFF_SIZE];

    // get file len
    data_len = lseek(fd_from, 0, SEEK_END);
    if (data_len < 0) {
        perror("Error read file");
        return -1;
    }

    // read and write to new file
    lseek(fd_from, 0, SEEK_SET);
    while (True) {
        if (data_len >= BUFF_SIZE) {
            read(fd_from, data, BUFF_SIZE);
            write(fd_to, data, BUFF_SIZE);
            data_len -= BUFF_SIZE;
        } else {
            read(fd_from, data, data_len);
            write(fd_to, data, data_len);
            break;
        }
    }
    return 0;
}

int check_is_dir(char *dir_name) {
    struct stat stat_dir;
    int check_exits_dir;

    check_exits_dir = access(dir_name, F_OK);
    if (check_exits_dir != -1) {
        stat(dir_name, &stat_dir);
        if (S_ISDIR(stat_dir.st_mode)) {
            return 1;
        }
    }
    return 0;
}
