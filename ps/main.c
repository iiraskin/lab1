#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


struct Process
{
    char* tty;
    short hours;
    short minutes;
    short seconds;
    char* name;
};

int SelectPids (const struct dirent * name)
{
    /*PID of process contains only numbers*/    
    if (name->d_name[0] < '0' || name->d_name[0] > '9') {
        return 0;
    }
    return 1;
}

int main()
{
    struct dirent **PidsOfProcess;
    int countOfProc;
    countOfProc = scandir ("/proc", &PidsOfProcess, SelectPids, alphasort);
    int i = 0;
    struct Process* processes = calloc (sizeof(struct Process), countOfProc);

    /*Find characteristic of processes*/
    for (; i < countOfProc; i++) {
        /*The name of process is in /proc as name of a directory
        so, it is already known */        
        char c = 0;
        char* procCharect = malloc (25);
        strcpy (procCharect, "/proc/");
        procCharect = strcat (procCharect, PidsOfProcess[i]->d_name);
        /*Find name*/
        /*The name of process is in file /proc/comm */
        procCharect = strcat (procCharect, "/comm");
        int fd = open (procCharect, O_RDONLY);
        processes[i].name = malloc (30);
        processes[i].name[0] = 0;
        while (read (fd, &c, 1)) {        
            processes[i].name = strncat (processes[i].name, &c, 1);
        }
        close (fd);
        /*Find TTY*/
        /*The seventh fild in file /proc/[PID]/stat contains necessery information*/
        processes[i].tty = malloc (10);
        processes[i].tty[0] = 0;
        procCharect[strlen (procCharect) - 5] = 0;
        procCharect = strcat (procCharect, "/stat");
        fd = open (procCharect, O_RDONLY);
        int counter = 0;
        while (counter != 6) {
            read(fd, &c, 1);
            if (c == ' ') {
                ++counter;
            }
        }
        char* ProcessTTY = malloc (10);
        ProcessTTY[0] = 0;
        do {
            read (fd, &c, 1);
            ProcessTTY = strncat (ProcessTTY, &c, 1);            
        } while (c != ' ');
        int ttyCode = atoi (ProcessTTY);
        if (ttyCode == 0) {
            processes[i].tty[0] = '?';
            processes[i].tty[1] = 0;
        }
        else {
            if (ttyCode / 256 == 4) {
                processes[i].tty = strcat (processes[i].tty, "tty/");
            } else {
                processes[i].tty = strcat (processes[i].tty, "pts/");
            }
            sprintf (ProcessTTY, "%d", ttyCode % 256);    
            processes[i].tty =    strcat (processes[i].tty, ProcessTTY);
        }
        free (ProcessTTY);    
        //Find time
        /*The fourteenth and the fiftinth fild in file /proc/[PID]/stat contains necessery information*/
        char* ProcessTime = malloc (10);
        ProcessTime[0] = 0;
        counter ++;
        while (counter != 13) {           
            read (fd, &c, 1);
            if (c == ' ') {
                ++counter;
            }
        }
        int utime = 0, stime = 0;
        do {
            read (fd, &c, 1);
            ProcessTime = strncat (ProcessTime, &c, 1);            
        } while (c != ' ');
        utime = atoi (ProcessTime);
        ProcessTime[0] = 0;
        do {
            read (fd, &c, 1);
            ProcessTime = strncat (ProcessTime, &c, 1);
        } while (c != ' ');
        close (fd);
        stime = atoi (ProcessTime);
        int time = (utime + stime) / sysconf(_SC_CLK_TCK);
        processes[i].hours = time / 3600;
        processes[i].minutes = (time - (processes[i].hours * 3600)) / 60;
        processes[i].seconds = time % 60;
        free (ProcessTime);
        free (procCharect);
    }

    //Output
    printf ("  PID TTY          TIME CMD\n");
    int j = 1;    
    for (; j <= 7; ++j) {    
        i = 0;        
        for (; i < countOfProc; i++) {
            if (strlen (PidsOfProcess[i]->d_name) == j) {                
                printf ("%5s %-9s%02d:%02d:%02d %s", PidsOfProcess[i]->d_name, 
                        processes[i].tty, processes[i].hours, processes[i].minutes,
                           processes[i].seconds, processes[i].name);
                free (processes[i].tty);
                free (processes[i].name);
            }
        }
    }

    free (PidsOfProcess);
    return 0;
}
