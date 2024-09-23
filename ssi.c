// check im allowed to use all of these 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h> 
#include <string.h>

char* prompt_input(){
    //! SHOULD MOVE THE CWD STUFF OUT SO I CAN USE IT IN change_dir() AS WELL
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

char* change_dir(char *path){
    // get the current working directory
    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        printf("cwd: %s\n", cwd);
    } else {
        printf("getcwd() error");
    }

    // split the path with / as a delimeter, loop through to determine new path
    char *token = strtok(path, "/");
    while(token != NULL) {
        // determine if we are going in or out of a directory 
        if (strcmp(token, "..") == 0){
            char *last_slash;
            last_slash = strrchr(cwd, '/');
            if (last_slash != NULL) {
                *(last_slash) = '\0';
            }
        } else {
            //! need to add something for if the path doesnt exist
            strcat(cwd, "/");
            strcat(cwd, token);  
        }
        token = strtok(NULL, "/");        
    }
    printf("new updated path: %s\n", cwd);

    // copy cwd into *final_cwd to be returned if not NULL
    char *final_cwd = malloc(strlen(cwd)+1);
    if(final_cwd != NULL){
        strcpy(final_cwd, cwd);
        return final_cwd;
    } 
    printf("cwd is null\n");
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
    char *first_token = strtok(user_input, " ");
    printf("first token: %s\n", first_token);
    char *token = strtok(NULL, " ");    
    if (strcmp(first_token, "cd") == 0){
        //* CHANGING DIRECTORIES
        printf("first_token is cd\n");
        // how do I execute this command to change directories
        printf("next token: %s\n", token);
        char *cwd = change_dir(token);        
        //? should I actually be changing directories or just keeping track of what directory I should be in and then using this to execute files and such?
    } else if (strcmp(first_token, "bg") == 0){
        //* BACKGROUND EXECUTION 
        printf("first_token is bg\n");        
        while (token!= NULL){
            printf("next token: %s\n", token);
            token = strtok(NULL, " ");
        }
    } else if (strcmp(first_token, "bglist") == 0){
        //* BACKGROUND EXECUTION LIST
        printf("first_token is bglist\n");
        while (token!= NULL){
            printf("next token: %s\n", token);
            token = strtok(NULL, " ");
        }
    } else {
        //* FOREGROUND EXECUTION
        printf("first_token is a function to run\n");
        while (token!= NULL){
            printf("next token: %s\n", token);
            token = strtok(NULL, " ");
        }
    }        
    return 0;
}

