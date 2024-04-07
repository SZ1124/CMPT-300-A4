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

#include "list.h"

int temp = 0;
int flag = 0; // 0 = no flag, 1 = i, 2 = j, 3 = both
int stage = 0;
List* directoryList; // could use a list here.
char* current_directory;
char* parent_directory;
char* root_directory;

void init()
{
    directoryList = List_create();
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        current_directory = malloc(strlen(cwd) + 1);
        strcpy(current_directory, cwd);
    } else {
        perror("getcwd");
    }
}

void readDirectory()
{
    char* directory_path = List_trim(directoryList);
    //printf("%s\n", directory_path);
    DIR* d = opendir(directory_path);
    if(d == NULL){
        perror("Error opening directory.\n");
        return;
    }
    struct dirent* dp; 
    struct stat buf; 
    while ((dp = readdir(d)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".git") == 0) {
            continue;
        }

        if (stat(dp->d_name, &buf) == -1) {
            //perror("Error retrieving file information");
            continue;
        }
        switch(flag){
            case 1:
                printf("%ld %s ", buf.st_ino, dp->d_name);
                break;

            default:
                break;
        }
        // You can access other information from 'buf' as needed
    }
    printf("\n");

    closedir(d);
}

void printDirectories()
{
    char* temp;
    List_first(directoryList);
    while(List_curr(directoryList) != NULL){
        temp = List_curr(directoryList);
        printf("%s\n", temp);
        List_next(directoryList);
    }
}

void readInput()
{
    char *input = NULL;
    const char delim[] = " \n";
    size_t len = 0;
    ssize_t read;

    while(1){
        flag = 0;
        stage = 0;
        temp = 0;
        read = getline(&input, &len, stdin);
        if (read == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        if (strcmp(input, "\n") == 0) {
            break;
        } else {
            char* token = strtok(input, delim);
            if (token == NULL || strcmp(token, "ls") != 0) {
                printf("Invalid input format. Please try again.\n");
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
                            } else {
                                stage = 1;
                                temp = 0;
                                List_prepend(directoryList, token);
                                break;
                            }
                        }
                    } else {
                        if(strcmp(token, ".") == 0){
                            List_prepend(directoryList, current_directory);
                            break;
                        }
                        if(strcmp(token, "..") == 0){
                        }
                        //printf("%s\n", token);
                        
                        List_prepend(directoryList, token);
                    } 
                    flag |= temp;
                }
                
                if(List_count(directoryList) == 0){
                    List_prepend(directoryList, current_directory);
                }
            }
        }
        while(List_count(directoryList) != 0){
            printf("directory: %s\n", (char*)List_curr(directoryList));
            readDirectory();
        } 
    }
}

int main() {
    init();
    readInput();
    return 0;

}
