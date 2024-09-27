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
    char cwd[PATH_MAX];
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
        // if input is empty
        // if (strlen(input) == 0) {
        //     return strdup("");
        // }
        
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

struct bg_pro* new_bg_pro(pid_t pid, char **command) {
    struct bg_pro* new_pro = (struct bg_pro*)malloc(sizeof(struct bg_pro));
    new_pro->pid = pid;
    new_pro->command = command; 
    char *cwd_result = getcwd(new_pro->cwd, sizeof(new_pro->cwd));
    if (cwd_result == NULL) {
        perror("getcwd() is null");
        exit(EXIT_FAILURE);
    }
    new_pro->next = NULL;
    return new_pro;
}

void add_bgpro_to_list(struct bg_pro** root, pid_t pid, char **command) {
    struct bg_pro* new_bgpro = new_bg_pro(pid, command);
    if (*root == NULL) {
        // list is empty, make new bg pro the root
        *root = new_bgpro;
    } else {
        // add new bg pro to end of linked list 
        struct bg_pro* node = *root;
        while(node->next != NULL) {
            node = node->next;
        }
        node->next = new_bgpro;
    }
}

void remove_bgpro_from_list(struct bg_pro** root, pid_t pid) {
    struct bg_pro* curr = *root;
    struct bg_pro* prev = NULL;

    // removing the root
    if (curr != NULL && curr->pid == pid) {
        *root = curr->next;
        // printf("%d: path %s has terminated\n", curr->pid, curr->command[0]); //! come back to this to print in right format so the whole command and path is displayed
        printf("%d: %s ", curr->pid, curr->cwd);
        for (char **user_cmd = curr->command; *user_cmd != NULL; user_cmd++) {
            printf("%s ", *user_cmd);
        }
        printf("has terminated.\n");
        free(curr);            
        return;
    }
    // remove any other, traverse until matching pid is found 
    while (curr != NULL && curr->pid != pid) {
        prev = curr;
        curr = curr->next;
    }
    // if curr is NULL then pid not found 
    if (curr == NULL) {
        printf("bg process not found so it cannot be removed\n");
        return;
    }
    // otherwise remove the matching pid process        
    prev->next = curr->next;
    printf("%d: %s ", curr->pid, curr->cwd);
    for (char **user_cmd = curr->command; *user_cmd != NULL; user_cmd++) {
        printf("%s ", *user_cmd);
    }
    printf("has terminated.\n");
    free(curr);
}

void print_bglist(struct bg_pro* root){
    struct bg_pro* temp = root;
    int counter = 0;
    while(temp != NULL){
        counter++;
        printf("%d: %s ", temp->pid, temp->cwd);
        for (char **user_cmd = temp->command; *user_cmd != NULL; user_cmd++) {
            printf("%s ", *user_cmd);
        }
        printf("\n");
        temp = temp->next;
    }
    printf("Total Background jobs: %d\n", counter);
}

int main(int argc, char *argv[]) {
    //! do we need to worry about piped commands? or anything involving changing directories and running things?
    // struct bg_pro* root = (struct bg_pro*)malloc(sizeof(struct bg_pro));    
    struct bg_pro* root = NULL;
    while (1) {        
        //* check for completed child processes and remove them from the linked list
        if (root != NULL) {
            pid_t ter = waitpid(0, NULL, WNOHANG);
            if (ter > 0) {
                remove_bgpro_from_list(&root, ter);
            }
        }
        // printf("testing testing testing\n");
        char* user_input = prompt_input();
        // printf("testing testing testing\n");

        // if user input is empty, reprompt
        if(user_input[0] == '\0'){            
            continue;
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
            
            // create new bg_pro and add it to linked list 
            // add_bgpro_to_list(&root, pid, args);

            if (pid == -1) {
                perror("Failed to fork");
            } else if (pid == 0) { 
                // child process
                // make it so execvp doesnt output the processes output
                

                if (execvp(args[0], args) == -1) {
                    perror("execvp failed");
                }
                exit(EXIT_FAILURE);
            } else {
                // parent process         
                //! what am i  doing in here then? 
                // create new bg_pro and add it to linked list 
                add_bgpro_to_list(&root, pid, args);
                printf("starting bg process: %s with pid: %d\n", args[0], pid);

                //* check for completed child processes and remove them from the linked list
                // if (root != NULL) {
                //     pid_t ter = waitpid(0, NULL, WNOHANG);
                //     if (ter > 0) {
                //         remove_bgpro_from_list(&root, ter);
                //     }
                // }
            }
            // now once a child process finishes executing, remove it from the list and print that it is finished in the formatted way specified
        } else if (strcmp(first_token, "bglist") == 0){
            //* BACKGROUND EXECUTION LIST            
            //! move this out of the function and just into here once everything done but for now use it for testing bg
            print_bglist(root);
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
        
        //* check for completed child processes and remove them from the linked list
        // if (root.next != NULL) {
        //     printf("bglist is not empty\n");
        //     pid_t ter = waitpid(0, NULL, WNOHANG);
        //     if (ter > 0) {
        //         if (root->next->pid == ter) {
        //             printf("%d:  %s has terminated\n", root->next->pid, root->next->command[0]); //! come back to this to figure out how to print the whole command and the path
        //             root->next = root->next->next;
        //         } else {
        //             struct bg_pro* node = &root;
        //             while (node->next->pid != ter) {
        //                 node = node->next;
        //             }
        //             printf("%d:  %s has terminated\n", root->pid, root->command[0]); //! come back to this to figure out how to print the whole command and the path
        //             node->next = node->next->next;
        //         }
        //     }
        // }
    }
    return 0;
}

