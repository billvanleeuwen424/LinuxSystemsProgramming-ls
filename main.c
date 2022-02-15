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
int tryStat(struct stat *fileStats, char *fullPath);
void printLsl(char *filename, struct stat *pfileStat, int printFlag, char *printString);
char *filePermissionString(struct stat *fileStats);



#define MAX_DIR_LENGTH 256
#define MAX_PARAMS 2
#define MIN_PARAMS 1



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
        getcwd(directoryName, MAX_DIR_LENGTH); 

        if(directoryName == NULL){
            printf("Get Current Directory Failed. Exiting \n");
            exit(1);
        }
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



    struct dirent *dirEntry;
    char filePath[MAX_DIR_LENGTH];
    char filename[MAX_DIR_LENGTH];
    struct stat fileStat;
    struct stat *pfileStat = &fileStat;
    
    /*vars to hold the important files*/
    int largest, smallest, newest, oldest;
    char largestString[MAX_DIR_LENGTH], smallestString[MAX_DIR_LENGTH], newestString[MAX_DIR_LENGTH], oldestString[MAX_DIR_LENGTH];
    char largestFileName[MAX_DIR_LENGTH], smallestFileName[MAX_DIR_LENGTH], newestFileName[MAX_DIR_LENGTH], oldestFileName[MAX_DIR_LENGTH];
    /*char *plargestString = &largestString, *psmallestString = &smallestString, *pnewestString = &newestString, *poldestString = &oldestString;*/
    int largestSet = 0, smallestSet = 0, newestSet = 0, oldestSet = 0;


    /*loop til error or null entry*/
    while (tryReadDir(&dir, &dirEntry) == 0){
 
        /*get full filepath*/
        strncpy(filePath, directoryName, MAX_DIR_LENGTH );
        strncat(filePath,"//",1);
        strncat(filePath, dirEntry->d_name,MAX_DIR_LENGTH);

        /*get just the filename*/
        strncpy(filename, dirEntry->d_name, MAX_DIR_LENGTH);



        /*Get the file stats, and error check*/
        /*clear errno*/
        errno = 0;
        stat(fullPath, fileStats);
        if(errno != 0){
            fprintf (stderr, "%s: Couldn't open stats on file %s;\n",fullPath, strerror(errno));
            exit(1);
        }



        /*check if it is a directory before adding to list*/
        if(S_ISDIR(pfileStat->st_mode) == 0){

            /*find the largest, smallest, newest, oldest*/
            if(largestSet != 1 || pfileStat->st_size > largest){
                largest = pfileStat->st_size;
                strncpy(largestString, filePath, MAX_DIR_LENGTH);
                strncpy(largestFileName, dirEntry->d_name, MAX_DIR_LENGTH);
                largestSet = 1;
            }
            if(smallestSet != 1 || pfileStat->st_size < smallest){
                smallest = pfileStat->st_size;
                strncpy(smallestString, filePath, MAX_DIR_LENGTH);
                strncpy(smallestFileName, dirEntry->d_name, MAX_DIR_LENGTH);
                smallestSet = 1;
            }
            if(oldestSet != 1 || pfileStat->st_mtime < oldest){
                oldest = pfileStat->st_mtime;
                strncpy(oldestString, filePath, MAX_DIR_LENGTH);
                strncpy(oldestFileName, dirEntry->d_name, MAX_DIR_LENGTH);
                oldestSet = 1;
            }
            if(newestSet != 1 || pfileStat->st_mtime > newest){
                newest = pfileStat->st_mtime;
                strncpy(newestString, filePath, MAX_DIR_LENGTH);
                strncpy(newestFileName, dirEntry->d_name, MAX_DIR_LENGTH);
                newestSet = 1;
            }
        }
        
    }



    /*check if any of the objective strings are empty, which would infer an empty directory. or maybe a broken program*/
    if(largestString[0] == '\0' || smallestString[0] == '\0' || newestString[0] == '\0' || oldestString[0] == '\0'){
        printf("Empty Directory.\n");
    }
    else{


        char lsOutput[MAX_DIR_LENGTH];

        if(tryStat(pfileStat, largestString) == -1){
            exit(1);
        }
        printf("\nLargest:\n");
        printLsl(largestFileName, pfileStat, 1, lsOutput);
        
        if(tryStat(pfileStat, smallestString) == -1){
            exit(1);
        }
        printf("\nSmallest:\n");
        printLsl(smallestFileName, pfileStat, 1, lsOutput);
    
        if(tryStat(pfileStat, newestString) == -1){
            exit(1);
        }
        printf("\nNewest:\n");
        printLsl(newestFileName, pfileStat, 1, lsOutput);
        
        if(tryStat(pfileStat, oldestString) == -1){
            exit(1);
        }
        printf("\nOldest:\n");
        printLsl(oldestFileName, pfileStat, 1, lsOutput);
    }


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
Use: pass the stat struct, and the full filepath

Purpose:  this function will try stat, upon any errors will return -1

Authour: William Van Leeuwen
Date: Feb 2022
*/
int tryStat(struct stat *fileStats, char *fullPath){

    int returnVal = 0;

    errno = 0;

    stat(fullPath, fileStats);

    if(errno != 0){
        fprintf (stderr, "%s: Couldn't open stats on file %s; %s\n", "tryStat",fullPath, strerror(errno));
        returnVal = -1;
    }

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