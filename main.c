/*
Name: Lab2
Authour: William Van Leeuwen - 0697505
Purpose: finding the largest, smallest, youngest and oldest file in a directory

How to use:
    ./main [Target Directory]
    or: 
    ./main

    the 2nd option will run the program in the cwd

Params: standard params taken from the cmd line. 

Required: See include statements below. 
    The functions required are already embedded in this file

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*for opendir and readdir*/
#include <dirent.h>     
/*file stats*/
#include <sys/stat.h>   
/*for getcwd*/
#include <unistd.h>     
/*for errno*/
#include <errno.h>      
/*for time*/
#include <time.h>       
/*for conversion of uid to strings*/
#include <pwd.h>        
/*for conversion of gid to strings*/
#include <grp.h>        


int tryReadDir(DIR **dir, struct dirent **dirEntry);
void printLsl(char *filename, struct stat *pfileStat, int printFlag, char *printString);
char *filePermissionString(struct stat *fileStats);


#define MAX_DIR_LENGTH 256
#define MAX_PARAMS 2
#define MIN_PARAMS 1
#define MAX_FILES 100


int main( int argc, char *argv[] )
{


    /*************************
     * GET DIRECTORY SECTION *
     *************************/

    char directoryName[MAX_DIR_LENGTH];


    /***********************
     *check parameter count*
     ***********************/
    if( argc > MAX_PARAMS ){
        printf("Too many parameters. Exit. \n");
        exit(1);
    }

    /*get current directory*/
    else if (argc <= MIN_PARAMS){
        strcpy(directoryName, "./");
    }

    /*get dir from cmd line*/
    else{

        if(strlen(argv[1]) > MAX_DIR_LENGTH ){
            printf("Directory length longer than %d. Quitting.\n", MAX_DIR_LENGTH);
            exit(1);
        }

        else{
            strncpy(directoryName, argv[1], MAX_DIR_LENGTH );
        }  

    } 
    

    /**************************
     * GET FILE STATS SECTION *
     **************************/

    /*clear errno*/
    errno = 0; 
    
    DIR *dir = opendir(directoryName);

    /*error check dir*/
    if(dir == NULL){

        /*reference. GNU C Library - Section 2.3 */
        fprintf (stderr, "%s: Couldn't open directory %s;\n", directoryName, strerror(errno));

        exit(1);
    }


    char files[MAX_FILES][MAX_DIR_LENGTH];
    int filesCounter =0;
    char lsOutput[MAX_DIR_LENGTH];


    /*vars for the file loop*/
    struct dirent *dirEntry;
    char filePath[MAX_DIR_LENGTH];
    char filename[MAX_DIR_LENGTH];
    struct stat fileStat;
    struct stat *pfileStat = &fileStat;


    int readDirFlag = 0;

    /*loop til error or null entry*/
    while ((readDirFlag = tryReadDir(&dir, &dirEntry)) == 0){
 
        /*get full filepath*/
        strncpy(filePath, directoryName, MAX_DIR_LENGTH );
        strncat(filePath,"/",2);
        strncat(filePath, dirEntry->d_name,MAX_DIR_LENGTH);

        /*get just the filename*/
        strncpy(filename, dirEntry->d_name, MAX_DIR_LENGTH);

        /*Get the file stats, and error check*/
        /*clear errno*/
        errno = 0;
        stat(filePath, pfileStat);
        if(errno != 0){
            fprintf (stderr, "%s: Couldn't open stats on file %s;\n",filePath, strerror(errno));
            exit(1);
        }    

        printLsl(filename, pfileStat, 0, lsOutput);
        strcpy(files[filesCounter],lsOutput);
        filesCounter++;

    }
    if(readDirFlag == -1){
        printf("Error reading directory. Exiting.\n");
        exit(1);
    }

    /*print dirname and num files*/
    printf("\n%s/\n", directoryName);
    printf("total: %d\n", filesCounter);
    

    /*print files*/
    int i;
    for(i = 0; i < filesCounter; i++){
        printf("%s", files[i]);
    }
    printf("\n");


    /*should return 0 on close dir*/
    return closedir(dir);
}





/*
This function will read the next entry in the directory.
it will check if the directory entry is NULL and return -1 if error, or return 1 if just end of directory

Authour: William Van Leeuwen
Date: Feb 2022
*/
int tryReadDir(DIR **dir, struct dirent **dirEntry){

    errno = 0;

    int returnVal = 0;

    *dirEntry = readdir(*dir);

    if(*dirEntry == NULL){   
        if (errno != 0){    /*if there is an error*/

            returnVal = -1;        
            fprintf (stderr, "%s: Couldn't open next entry in directory; %s\n", "tryReadDir", strerror(errno));

        }
        else{               /*if we just reached the end of the directory*/
            returnVal = 1;
        }
    }                       /* else should still just be zero */
    
    return returnVal;
}


/*
Use: Pass this function the filename,
    the file stats struct,
    a print flag (will printf the string if ! 0),
    and the string which the ls -l output will be stored in

Purpose: Produce ls -l style output

Authour: William Van Leeuwen
Date: Feb 2022
*/
void printLsl(char *filename, struct stat *pfileStat, int printFlag, char *printString){

    /* convert the gid/uid to group/passwd structs inorder to print string */
    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(pfileStat->st_gid);
    pwd = getpwuid(pfileStat->st_uid);


    /*get and format time*/
    time_t modTime =  pfileStat->st_mtime;
    struct tm  ts;
    char timeString[80];

    ts = *localtime(&modTime);

    strftime(timeString, sizeof(timeString), "%b %d %Y [%H:%M] ", &ts);
    

    /*print in ls -l format. filePermissionString to format the permissions*/
    snprintf(printString, 500, "%-4s %-6s %-6s %5lu %-18s %-7s\n", filePermissionString(pfileStat), pwd->pw_name, grp->gr_name, pfileStat->st_size, timeString, filename);

    if (printFlag != 0){
        printf("%s\n", printString);
    }
    
}

/*this function will take the mode_t from the stat type and return a formatted string of permissions


Authour: William Van Leeuwen
Date: Feb 2022

references: 

The Linux Programming Interface
    Chapter 15
    pg 296

and 

https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
*/
char *filePermissionString(struct stat *fileStats){

    static char permissionsString[9];

    snprintf(permissionsString, 10, "%c%c%c%c%c%c%c%c%c",

    (fileStats->st_mode & S_IRUSR) ? 'r' : '-',
    (fileStats->st_mode & S_IWUSR) ? 'w' : '-', 
    (fileStats->st_mode & S_IXUSR) ? 'x' : '-',

    (fileStats->st_mode & S_IRGRP) ? 'r' : '-',
    (fileStats->st_mode & S_IWGRP) ? 'w' : '-', 
    (fileStats->st_mode & S_IXGRP) ? 'x' : '-',

    (fileStats->st_mode & S_IROTH) ? 'r' : '-',
    (fileStats->st_mode & S_IWOTH) ? 'w' : '-', 
    (fileStats->st_mode & S_IXOTH) ? 'x' : '-'
    );


    return permissionsString;
}