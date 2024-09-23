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

int main(int argc, char *argv[]) {
    //! need to do something for if user just presses enter and doesnt type anything 
    while (1) {        
        char* user_input = prompt_input();
        if(user_input == NULL){
            printf("something went wrong, user input is null\n");
            return 0;
        }
        printf("You input: %s\n", user_input);        
        char *first_token = strtok(user_input, " ");
        printf("first token: %s\n", first_token);
        char *token = strtok(NULL, " ");    
        if (strcmp(first_token, "cd") == 0){
            //* CHANGING DIRECTORIES
            printf("next token: %s\n", token);
            //! I CAN ACTUALLY JUST PUT THE TOKEN DIRECTLY INTO chdir() instead of using this change_dir() function I made 
            if (chdir(token) == 0){
                printf("successfully changed directory\n");
            } else {
                perror("Failed to change directory");
            }             
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
    }
    return 0;
}

