#include <stdio.h>

#include <stdlib.h>
#include <string.h>
//for opendir and readdir
#include <dirent.h>
//file stats
#include <sys/stat.h>
//for getcwd
#include <unistd.h>
//for errno
#include <errno.h>
//for time
#include <time.h>
//for conversion of gid and uid to strings
#include <pwd.h>
#include <grp.h>


int checkMaxParams(int argc, char *argv[]);
void cpyDirectory(char * director, int position, char *argv[]);
int getLastFileModification(struct stat *pfileStats, char *fullPath);
int getFileSize(struct stat *pfileStats, char *fullPath);
int tryOpenDir(DIR **dir, char * dirpath);
int tryReadDir(DIR **dir, struct dirent **dirEntry);
void getFullPath(struct dirent *pdirectoryEntry, char *dirpath, char *fullPath);
int tryStat(struct stat *fileStats, char *fullPath);
void printLs(char *filename);
void printLsl(char *filename, struct stat *pfileStat);

char *filePermStr(mode_t perm, int flags);

#define MAX_DIR_LENGTH 256
#define MAX_PARAMS 2
#define MIN_PARAMS 1


#define MAX_BUFFER 4096


int main( int argc, char *argv[] )
{

    /*************************
     * GET DIRECTORY SECTION *
     *************************/

    char directoryName[MAX_DIR_LENGTH];

    int maxParamReturnVal = checkMaxParams(argc, argv);
    
    /*error check and get either cwd or cmdline arg */
    if(maxParamReturnVal == 1){ /*too many params*/
        exit(1);
    }
    /*empty cmdline. getcwd*/
    else if(maxParamReturnVal == -1){   
        getcwd(directoryName, MAX_BUFFER);
    }
    /*copy the dirname from argv*/
    else{
        cpyDirectory(directoryName, 1, argv);   
    }  
    

    /**************************
     * GET FILE STATS SECTION *
     **************************/

    DIR *dir;
    /*try to open the dir*/
    if(tryOpenDir(&dir, directoryName) == -1){
        exit(1);
    }

    struct dirent *dirEntry;
    char filePath[MAX_BUFFER];
    char filename[MAX_DIR_LENGTH];
    struct stat fileStat;
    struct stat *pfileStat = &fileStat;
    
    /*vars to hold the important files*/
    int largest, smallest, newest, oldest;
    char largestString[MAX_DIR_LENGTH], smallestString[MAX_DIR_LENGTH], newestString[MAX_DIR_LENGTH], oldestString[MAX_DIR_LENGTH];
    char largestFileName[MAX_DIR_LENGTH], smallestFileName[MAX_DIR_LENGTH], newestFileName[MAX_DIR_LENGTH], oldestFileName[MAX_DIR_LENGTH];
    //char *plargestString = &largestString, *psmallestString = &smallestString, *pnewestString = &newestString, *poldestString = &oldestString;
    int largestSet = 0, smallestSet = 0, newestSet = 0, oldestSet = 0;

    /*loop here til error or null entry*/
    while (tryReadDir(&dir, &dirEntry) == 0){

        getFullPath(dirEntry,directoryName,filePath);
        strncpy(filename, dirEntry->d_name, MAX_DIR_LENGTH);

        if(tryStat(pfileStat, filePath) != 0){
            exit(1);
        }
        else {
            /*check if directory before adding to list*/
            if(S_ISDIR(pfileStat->st_mode) == 0){
                if(largestSet != 1 || pfileStat->st_size > largest){
                    largest = pfileStat->st_size;
                    strncpy(largestString, filePath, MAX_DIR_LENGTH);
                    strncpy(largestFileName, filename, MAX_DIR_LENGTH);
                    largestSet = 1;
                }
                if(smallestSet != 1 || pfileStat->st_size < smallest){
                    smallest = pfileStat->st_size;
                    strncpy(smallestString, filePath, MAX_DIR_LENGTH);
                    strncpy(smallestFileName, filename, MAX_DIR_LENGTH);
                    smallestSet = 1;
                }
                if(oldestSet != 1 || pfileStat->st_mtime < oldest){
                    oldest = pfileStat->st_mtime;
                    strncpy(oldestString, filePath, MAX_DIR_LENGTH);
                    strncpy(oldestFileName, filename, MAX_DIR_LENGTH);
                    oldestSet = 1;
                }
                if(newestSet != 1 || pfileStat->st_mtime > newest){
                    newest = pfileStat->st_mtime;
                    strncpy(newestString, filePath, MAX_DIR_LENGTH);
                    strncpy(newestFileName, filename, MAX_DIR_LENGTH);
                    newestSet = 1;
                }
            }
        }
    }


    /*check if directory empty*/
    if(largestString[0] == '\0' || smallestString[0] == '\0' || newestString[0] == '\0' || oldestString[0] == '\0'){
        printf("Empty Directory.\n");
    }
    else{

        if(tryStat(pfileStat, largestString) == -1){
            exit(1);
        }
        printf("\nLargest:\n");
        printLsl(largestFileName, pfileStat);
        
        if(tryStat(pfileStat, smallestString) == -1){
            exit(1);
        }
        printf("\nSmallest:\n");
        printLsl(smallestFileName, pfileStat);
    
        if(tryStat(pfileStat, newestString) == -1){
            exit(1);
        }
        printf("\nNewest:\n");
        printLsl(newestFileName, pfileStat);
        
        if(tryStat(pfileStat, oldestString) == -1){
            exit(1);
        }
        printf("\nOldest:\n");
        printLsl(oldestFileName, pfileStat);
    }


    /*should return 0 on close dir*/
    return closedir(dir);
}


/*  pass this function a string, the position of the directory in argv, and of course argv
 *  this will check if the string is longer than the MAX_DIR_LENGTH
 */
void cpyDirectory(char * directory, int position, char *argv[]){

    if(strlen(argv[position]) > MAX_DIR_LENGTH ){
        printf("Directory length longer than %d. Quitting.\n", MAX_DIR_LENGTH);
        exit(1);
    }

    else{
        strncpy(directory, argv[position], MAX_DIR_LENGTH );
    }
}


/* Pass this function the input from the cmd line
 * Checks if more params than max
 * will exit if too many, returns 0 on successful
 *
 * return values:
 *      0: just right amount of params
 *      1: too many params
 *      -1: too few params
 * Reference: I took this from the /home/COIS/3380/sample_scripts directory.
 * I also refactored this to make it my own
 */
int checkMaxParams(int argc, char *argv[]){

    int parameter_count = 0;

    int returnval = 0;

    if( argc > MAX_PARAMS ){
        printf("Too many parameters. Exit. \n");
        returnval = 1;
    }
    else if (argc <= MIN_PARAMS)
    {
        returnval = -1;
    }

    /*DEBUGGING
    while( parameter_count < argc )
    {
        printf("\tparameter[%d]: %s\n",parameter_count,argv[parameter_count]);
        parameter_count++;
    }
    printf("\n");
    */

    return returnval;
}

/*
This function will return the last modification time in epoch time.
it requires a pointer to a stat type, and the filepath of the deisred file.
*/
int getLastFileModification(struct stat *pfileStats, char *fullPath){

    stat(fullPath, pfileStats);

    return pfileStats->st_atime;
}

/*
This function will return the size of the file (bytes).
it requires a pointer to a stat type, and the filepath of the deisred file.
*/
int getFileSize(struct stat *pfileStats, char *fullPath){

    stat(fullPath, pfileStats);

    return pfileStats->st_size;
}

/*
This function just opens the directory specificed in the string passed.
it will check if the directory stream is NULL and return -1 if that is the case.
*/
int tryOpenDir(DIR **dir, char * dirpath){
    
    /*clear errno*/
    errno = 0; 

    int returnVal = 0;
    
    *dir = opendir(dirpath);

    if(*dir == NULL){

        /*reference. GNU C Library - Section 2.3 */
        fprintf (stderr, "%s: Couldn't open directory %s; %s\n", "tryOpenDir", dirpath, strerror(errno));

        returnVal = -1;
    }

    return returnVal;
}

/*
This function will read the next entry in the directory.
it will check if the directory entry is NULL and return -1 if error, or return 1 if just end of directory
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
This function will return a filename with its path as a single string.
It requires the files directory entry, and the path of the file.
*/
void getFullPath(struct dirent *pdirectoryEntry, char *dirpath, char *fullPath){

    strncpy(fullPath, dirpath, MAX_BUFFER );

    strncat(fullPath,"//",1);

    strncat(fullPath,pdirectoryEntry->d_name,MAX_BUFFER);

}

/*this function will try stat, upon any errors will return -1*/
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

/*this function will print the stats of a file in an ls -l format*/
void printLsl(char *filename, struct stat *pfileStat){

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
    

    /*print in ls -l format. filePermStr to format the permissions*/
    printf("%-4s %-6s %-6s %5lu %-18s %-7s\n", filePermStr(pfileStat->st_mode,1), pwd->pw_name, grp->gr_name, pfileStat->st_size, timeString, filename);

    /*
    incase we need to go back from the borrowed function
    printf("%-4o %-6s %-6s %5lu %-18s %-7s\n", pfileStat->st_mode, pwd->pw_name, grp->gr_name, pfileStat->st_size, timeString, dirEntry->d_name);
    */
}

/*this function will print the stats of a file in a regulat ls format*/
void printLs(char *filename){

    printf(" %-7s", filename);

}

/*this function will take the mode_t from the stat type and return a formatted string of permissions
this function was unashamedly taken from:
    The Linux Programming Interface
    Chapter 15
    pg 296

full disclosure. I'm not sure what the flags input does
*/
#define FILE_PERMS_H
#include <sys/types.h>
#define FP_SPECIAL 1


#define STR_SIZE sizeof("rwxrwxrwx")

char *filePermStr(mode_t perm, int flags){
    static char str[STR_SIZE];

    snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
        (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
        (perm & S_IXUSR) ?
            (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
            (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
        (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
        (perm & S_IXGRP) ?
            (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
            (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
        (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
        (perm & S_IXOTH) ?
            (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
            (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));

    return str;
}