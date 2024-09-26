// check im allowed to use all of these 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h> 
#include <string.h>
#include <sys/wait.h>

struct bg_pro {
    pid_t pid;
    char **command; 
    struct bg_pro* next;
};

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
    //! need to do something for if user just presses enter and doesnt type anything 
    //! do we need to worry about piped commands? or anything involving changing directories and running things?
    // struct bg_pro* root = (struct bg_pro*)malloc(sizeof(struct bg_pro));
    struct bg_pro root;
    root.next = NULL;
    while (1) {        
        char* user_input = prompt_input();
        if(user_input == NULL){
            printf("something went wrong, user input is null\n");
            return 0;
        }
        char *first_token = strtok(user_input, " ");
        char *token = strtok(NULL, " ");    
        if (strcmp(first_token, "cd") == 0){
            //* CHANGING DIRECTORIES
            // determine whether to go to home directory or somewhere else
            if (token == NULL || strcmp(token, "~") == 0) {
                char *home_dir = getenv("HOME");
                // if (chdir(home_dir) == 0) {
                //     printf("successfully changed directory\n");
                // } else {
                //     perror("Failed to change directory");
                // }
                chdir(home_dir);
            } else {
                // if (chdir(token) == 0){
                //     printf("successfully changed directory\n");
                // } else {
                //     perror("Failed to change directory");
                // }
                chdir(token);
                //! need to do something for if the directory doesnt exist 
            }            
        } else if (strcmp(first_token, "bg") == 0){
            //* BACKGROUND EXECUTION 
            // printf("first_token is bg\n");        
            // first breakup into parent and child 
                // would I only be running 1 bg process at a time? (1 child at a time?)
            
            // fork to create parent and child processes
            pid_t pid = fork();
            // get the command and options and put in a list
            char *args[10]; //! what should this number be?
            int i = 0;
            // args[i++] = first_token;
            while (token != NULL) {
                args[i++] = token;      
                token = strtok(NULL, " ");
            }
            args[i] = NULL;
            // create the instance of the process
            struct bg_pro* next_pro = (struct bg_pro*)malloc(sizeof(struct bg_pro));
                next_pro->pid = pid;
                next_pro->command = args;
                next_pro->next = NULL;

            if (pid == -1) {
                perror("Failed to fork");
            } else if (pid == 0) { 
                // child process                                
                    // take the process off top of list and execute then shift root down one 
                if (execvp(args[0], args) == -1) {
                    perror("execvp failed");
                }
                exit(EXIT_FAILURE);
            } else {
                // parent process          
                    // check if this is the first process - if so set it to the root - if not add it to the end of the list       
                if (root.next == NULL) { //? or should this be if pid == 0
                    root.next = next_pro;
                } else {
                    struct bg_pro* node = &root;
                    while (node->next != NULL) {
                        node = node->next;
                    }
                    node->next = next_pro;                 
                }
                printf("starting bg process: %s with pid: %d\n", args[0], pid);

            }
            // now once a child process finishes executing, remove it from the list and print that it is finished in the formatted way specified
        } else if (strcmp(first_token, "bglist") == 0){
            //* BACKGROUND EXECUTION LIST
            printf("first_token is bglist\n");
            while (token!= NULL){
                printf("next token: %s\n", token);
                token = strtok(NULL, " ");
            }
        } else if (strcmp(first_token, "exit") == 0){
            break;
        } else {
            //* FOREGROUND EXECUTION
            pid_t pid = fork();
            if (pid == -1) {
                perror("Failed to fork");
            } else if (pid == 0) {
                // now in child process so execute the command using execvp
                // get the args into something that execvp can take as a param COME BACK TO THIS 
                char *args[10]; //! what should this number be?
                int i = 0;       
                args[i++] = first_token;         
                while (token!=NULL) {
                    args[i++] = token;
                    token = strtok(NULL, " ");
                }
                args[i] = NULL;
                if (execvp(args[0], args) == -1) {
                    perror("execvp failed");
                }
                exit(EXIT_FAILURE);
            } else {
                // in the parent process so wait for the child to finish process 
                //TODO what do I do in here?
                int status;
                waitpid(pid, &status, 0);
                // printf("Child process finished with status: %d\n", WEXITSTATUS(status));
            }
        }       
    }
    return 0;
}

