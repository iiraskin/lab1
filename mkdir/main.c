#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <sys/stat.h>


void Create_directory (char* path, char option) {
    int i = 0;

    /*Check if the path exist*/
    for (i; path[i] != 0; i++) {
        /*Check each directory in the path*/
        if (path[i] == '/') {
            path[i] = 0;
            DIR* dir = opendir (path);
            if (!dir && strcmp (path, "\0")) {
                if (option) {
                    /*If a directory in the path doesn't exist -
                    it should be created*/
                    mkdir (path, 0744);
                } else {
                    /*If the path doesn't exist - it's an error*/
                    printf ("Incorrect path: %s \n", path);
                    error (1, errno, " ");
                }
            }
            closedir (dir);
            path[i] = '/';
        }
    }

    /*Create the directory*/
    int r = mkdir (path, 0744);
    if (r) {
        printf ("This directory already exist. \n");
    }
}

int main (int argc, char* argv[])
{
    char date;
    char option = 0;

    /*Determine which option should be used*/
    while ((date = getopt(argc, argv, "-p")) != -1) {
        if (date == 'p') {
            option = 'p';
        }
    }

    /*For each element of argv call chosen function*/
    int i = 1;
    for (i; i < argc; ++i) {
        if (strcmp (argv[i], "-p")) {
            Create_directory (argv[i], option);
        }
    }

    return 0;
}
