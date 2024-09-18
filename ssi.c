// check im allowed to use all of these 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h> 
#include <string.h>

char* prompt_input(){
    // get users login, hostame, and cwd then construct and print prompt
    char cwd[PATH_MAX];
    char *login = getlogin();
    char hostname[HOST_NAME_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL && login != NULL && gethostname(hostname, sizeof(hostname)) == 0){
        printf("%s@%s: %s > ", login, hostname, cwd);
    }

    // read user input 
    char input[100]; //! find out what number to put here
    char *token;
    if(fgets(input, sizeof(input), stdin) != NULL){
        input[strcspn(input, "\n")] = '\0'; //! check I can use this, this removes newline char if present
        // free up memory and copy input so it can be returned
        char *input_copy = malloc(strlen(input)+1);
        if(input_copy != NULL){
            strcpy(input_copy, input);
            return input_copy;
        } else {
            printf("input is null\n");
            return NULL;
        }
    }
    printf("Something went wrong with the input\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    //TODO still need to put this all in a loop of some sort (probably while)
    char* user_input = prompt_input();
    if(user_input == NULL){
        printf("something went wrong, user input is null\n");
        return 0;
    }
    printf("You input: %s\n", user_input);
    //TODO determine if they typed cd/bglist or bg .../any other program 
        // create functions to handle each of these seperately
    return 0;
}

