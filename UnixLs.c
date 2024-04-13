#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>

#include "list.h"

int temp = 0;
int flag = 0; // 0 = no flag, 1 = i, 2 = j, 3 = both
int stage = 0;
List* directoryList; // List for printing
char* current_directory;
char* parent_directory;
char* home_directory;

void init()
{   
    printf("*****WELCOME TO UnixLs SIMULATION******\n\n");
    directoryList = List_create();
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {

        current_directory = malloc(strlen(cwd) + 1);
        strcpy(current_directory, cwd);
        printf("Current directory: %s\n", cwd);
        
        home_directory = getenv("HOME");
        printf("Home directory: %s\n", home_directory);

        char* last_slash = strrchr(cwd, '/');

        if(last_slash != NULL)
        {
            *last_slash = '\0';
            printf("Parent directory:  %s\n\n", cwd);

            parent_directory = malloc(strlen(cwd) + 1);
            strcpy(parent_directory, cwd);

            *last_slash = '/'; // Restore the last slash
        }
    } else {
        perror("getcwd\n");
    }
}

void readDirectory()
{   
    while(List_count(directoryList) > 0)
    {
        char* directory_path = List_trim(directoryList);
        printf("%s: \n", directory_path);
        DIR* d = opendir(directory_path);
        if(d == NULL){
            perror("Error opening directory.\n");
            return;
        }
        struct dirent* dp; 
        struct stat buf; 
        while ((dp = readdir(d)) != NULL) {

            char full_path[1024]; // Adjust the size as needed
            snprintf(full_path, sizeof(full_path), "%s/%s", directory_path, dp->d_name);
            

            if (stat(full_path, &buf) == -1) {
                perror("Error getting file status.\n");
                continue;
            }
            else {
                if (lstat(full_path, &buf) == -1) {
                    perror("Error getting file status.\n");
                    continue;
                }
            }

            switch(flag){
                case 0:
                    if(strncmp(dp->d_name, ".", 1) != 0)
                    {
                        printf("%s \n", dp->d_name);
                    }
                    break;

                case 1:
                    if(strncmp(dp->d_name, ".", 1) != 0)
                    {                   
                        printf("%10ld %s \n", buf.st_ino, dp->d_name); //Size being occupied by 10 characters (changeable) to align numbers to the right
                    }
                    break;

                case 2:
                    if(strncmp(dp->d_name, ".", 1) != 0)
                    {
                        //print permission (1st column)
                        if (S_ISLNK(buf.st_mode)) {
                            printf("l");
                        } else if (S_ISDIR(buf.st_mode)) {
                            printf("d");
                        } else {
                            printf("-");
                        }
                        printf((buf.st_mode & S_IRUSR) ? "r" : "-");
                        printf((buf.st_mode & S_IWUSR) ? "w" : "-");
                        printf((buf.st_mode & S_IXUSR) ? "x" : "-");
                        printf((buf.st_mode & S_IRGRP) ? "r" : "-");
                        printf((buf.st_mode & S_IWGRP) ? "w" : "-");
                        printf((buf.st_mode & S_IXGRP) ? "x" : "-");
                        printf((buf.st_mode & S_IROTH) ? "r" : "-");
                        printf((buf.st_mode & S_IWOTH) ? "w" : "-");
                        printf((buf.st_mode & S_IXOTH) ? "x " : "- ");

                        //print # of hard link (2nd column)
                        printf("%5lu ", buf.st_nlink); //Size being occupied by 5 characters (changeable) to align numbers to the right

                        //print the owner of the file (3rd column)
                        struct passwd *pwd1 = getpwuid(buf.st_uid);
                        if (pwd1 != NULL) {
                            printf("%s ", pwd1->pw_name);
                        } else {
                            printf("Unknown ");
                        }


                        //print the name of the group file belongs to (4th column)
                        struct group *grp1 = getgrgid(buf.st_gid);
                        if (grp1 != NULL) {
                            printf("%s", grp1->gr_name);
                        } else {
                            printf("Unknown");
                        }

                        //print the size of the file in bytes (5th column)
                        printf("%8ld ", buf.st_size); //Size being occupied by 8 characters (changeable) to align numbers to the right
                        
                        //print the date and time of the most recent change (6th column)
                        time_t lastModified1 = buf.st_mtime;
                        struct tm* time1;
                        time1 = localtime(&lastModified1);
                        char buffer1[80];
                        strftime(buffer1, 80, "%b %e %Y %H:%M", time1);
                        printf("%s ", buffer1);

                        //print the name of the file or directory (7th column)
                        printf("%s ", dp->d_name);

                        //print the actual directory of the symbolic link (symbolic link only)
                        if (S_ISLNK(buf.st_mode)) {
                            printf("-> %s \n", full_path);
                        }
                        else{
                            printf("\n");
                        }
                    }
                    break;

                case 3:
                    if(strncmp(dp->d_name, ".", 1) != 0)
                    {
                        //print serial number (1st column)
                        printf("%10ld ", buf.st_ino); //Size being occupied by 10 characters (changeable) to align numbers to the right

                        //print permission (2nd column)
                        if (S_ISLNK(buf.st_mode)) {
                            printf("l");
                        } else if (S_ISDIR(buf.st_mode)) {
                            printf("d");
                        } else {
                            printf("-");
                        }                    
                        printf((buf.st_mode & S_IRUSR) ? "r" : "-");
                        printf((buf.st_mode & S_IWUSR) ? "w" : "-");
                        printf((buf.st_mode & S_IXUSR) ? "x" : "-");
                        printf((buf.st_mode & S_IRGRP) ? "r" : "-");
                        printf((buf.st_mode & S_IWGRP) ? "w" : "-");
                        printf((buf.st_mode & S_IXGRP) ? "x" : "-");
                        printf((buf.st_mode & S_IROTH) ? "r" : "-");
                        printf((buf.st_mode & S_IWOTH) ? "w" : "-");
                        printf((buf.st_mode & S_IXOTH) ? "x " : "- ");

                        //print # of hard link (3rd column)
                        printf("%5lu ", buf.st_nlink); //Size being occupied by 8 characters (changeable) to align numbers to the right

                        //print the owner of the file (4th column)
                        struct passwd *pwd2 = getpwuid(buf.st_uid);
                        if (pwd2 != NULL) {
                            printf("%s ", pwd2->pw_name);
                        } else {
                            printf("Unknown ");
                        }

                        //print the name of the group file belongs to (5th column)
                        struct group *grp2 = getgrgid(buf.st_gid);
                        if (grp2 != NULL) {
                            printf("%s", grp2->gr_name);
                        } else {
                            printf("Unknown");
                        }

                        //print the size of the file in bytes (6th column)
                        printf("%8ld ", buf.st_size); //Size being occupied by 8 characters (changeable) to align numbers to the right
                        
                        //print the date and time of the most recent change (7th column)
                        time_t lastModified2 = buf.st_mtime;
                        struct tm* time2;
                        time2 = localtime(&lastModified2);
                        char buffer2[80];
                        strftime(buffer2, 80, "%b %e %Y %H:%M", time2);
                        printf("%s ", buffer2);

                        //print the name of the file or directory (8th column)
                        printf("%s \n", dp->d_name);
                    }
                    break;

                default:
                    break;
            }
        }
        printf("\n");

        closedir(d);
    }

    printf("\n\n");

}

void printDirectories()
{
    List_first(directoryList);
    while(List_curr(directoryList) != NULL){
        List_next(directoryList);
    }
}

void readInput()
{
    char *input = NULL;
    const char delim[] = " \n";
    size_t len = 0;
    ssize_t read;
    bool checkOptionBeforeDirectory = true;
    int count = 0;

    while(1){
        flag = 0;
        stage = 0;
        temp = 0;
        read = getline(&input, &len, stdin);
        if (read == -1) {
            perror("getline\n");
            exit(EXIT_FAILURE);
        }

        if (strcmp(input, "\n") == 0) {
            break;
        } else {
            char* token = strtok(input, delim);
            if (token == NULL || strcmp(token, "UnixLs") != 0) {
                printf("Invalid input format (UnixLs). Please try again.\n");
                continue;
            } else {
                while ((token = strtok(NULL, delim)) != NULL) {
                    // need to detect, the moment u see 
                    if (token[0] == '-' && stage == 0) {
                        for(int i = 1; i < strlen(token); i++){
                            if(token[i] == 'i'){
                                temp |= 1;
                            } else if (token[i] == 'l'){
                                temp |= 2;
                            }else {
                                stage = 1;
                                temp = 0;
                                List_prepend(directoryList, token);
                                break;
                            }
                        }

                        // if "-" options appear after directory(s)
                        if(!checkOptionBeforeDirectory){
                            //empty the prepended items
                            for(int z = 0; z < count; z++){
                                List_trim(directoryList);
                            }

                            printf("Invalid input format (Option needs to be before directory(s)). Please try again.\n\n");

                            break;
                        }

                    } 
                    else
                    {
                        //Different cases
                        if(strcmp(token, ".") == 0){
                            List_prepend(directoryList, current_directory);
                        }
                        else if(strcmp(token, "..") == 0){
                            List_prepend(directoryList, parent_directory);
                        }
                        else if(strcmp(token, "~") == 0){
                            List_prepend(directoryList, home_directory);
                        }
                        else if(strstr(token, "~/") != NULL && strstr(token, "/~") == NULL){
                            List_prepend(directoryList, home_directory);
                        }
                        else if(strcmp(token, "/") == 0){
                            List_prepend(directoryList, token);
                        }
                        else
                        {
                            //case for file
                            List_prepend(directoryList, token);
                        }

                        count++;
                        checkOptionBeforeDirectory = false;
                    }
                    
                    flag |= temp;
                }
                
                // if(List_count(directoryList) == 0 && ){
                //     List_prepend(directoryList, current_directory);
                // }
            }
        }

        checkOptionBeforeDirectory = true;
        count = 0;

        while(List_count(directoryList) != 0){
            readDirectory();
        } 
    }
}

int main() {
    init();
    readInput();
    return 0;

}
