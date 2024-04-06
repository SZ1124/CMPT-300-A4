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

int flag = 0; // 0 = no flag, 1 = i, 2 = j, 3 = both
int stage = 0;
List* directory; // could use a list here.

void init()
{
    directory = List_create();
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
        read = getline(&input, &len, stdin);
        if (read == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        if (strcmp(input, "!\n") == 0) {
            break;
        } else {
            char* token = strtok(input, delim);
            if (token == NULL || strcmp(token, "ls") != 0) {
                printf("Invalid input format. Please try again.\n");
                continue;
            } else {
                while ((token = strtok(NULL, delim)) != NULL) {
                    // search token for invalid flag, if invalid flag found, treat it as a directory
                    // the first time a invalid flag is found, stop trying to find flags, all the rest of the arguements become directory
                    if (token[0] == '-' && stage == 0) {
                        for(int i = 1; i < strlen(token); i++){
                            if(token[i] == 'i'){
                                //printf("i found at index %d\n", i);
                                flag |= 1;
                            } else if (token[i] == 'l'){
                                //printf("l found at index %d\n", i);
                                flag |= 2;
                            } else {
                                stage = 1;
                                flag = 0;
                                //printf("directory: %s\n", token);
                                List_append(directory, token);
                            }
                        }
                    } else{
                        //printf("directory: %s\n", token);
                        List_append(directory, token);
                    }
                }
            }
        }
        //printf("%d\n",flag);
    }
}

int main() {

    readInput();
    return 0;

}
