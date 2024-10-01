#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h> 
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

//* HELPER FUNCTIONS

// bg_pro struct to hold info for outputting in bglist and for termination 
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
    char input[512]; 
    char *token;
    if(fgets(input, sizeof(input), stdin) != NULL){
        input[strcspn(input, "\n")] = '\0'; 
        
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

char **args_copy(char **args) {
    // get size of args to store 
    int count = 0;
    while (args[count] != NULL) {
        count++;
    }

    // allocate memory and copy args over
    char **copy = malloc((count+1) * sizeof(char *));
    if (copy==NULL) {
        perror("error allocating memory for args");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < count; i++) {
        copy[i] = strdup(args[i]);
        if (copy[i] == NULL) {
            perror("error copying args");
            free(copy);
            exit(EXIT_FAILURE);
        }
    }
    copy[count] = NULL;
    return copy;
}

void free_args(char **args) {
    // free dynamic memory allocated for args 
    if (args == NULL) {
        return;
    }
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

struct bg_pro* new_bg_pro(pid_t pid, char **command) {
    // allocate memory for bg_pro
    struct bg_pro* new_pro = (struct bg_pro*)malloc(sizeof(struct bg_pro));
    if (new_pro == NULL) {
        perror("error allocating memory for background process");
        free(new_pro);
        exit(EXIT_FAILURE);
    }
    
    // assign values to bg_pro struct and return it
    new_pro->pid = pid;
    new_pro->command = args_copy(command); 
    char *cwd_result = getcwd(new_pro->cwd, sizeof(new_pro->cwd));
    if (cwd_result == NULL) {
        perror("getcwd() is null");
        free(new_pro);
        exit(EXIT_FAILURE);
    }
    new_pro->next = NULL;
    return new_pro;
}

void add_bgpro_to_list(struct bg_pro** root, pid_t pid, char **command) {
    // create new bg_pro
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

    // removing the root from linked list
    if (curr != NULL && curr->pid == pid) {
        *root = curr->next;
        printf("%d: %s ", curr->pid, curr->cwd);
        for (char **user_cmd = curr->command; *user_cmd != NULL; user_cmd++) {
            printf("%s ", *user_cmd);
        }
        printf("has terminated.\n");
        free_args(curr->command);
        free(curr);            
        return;
    }
    // removing any other bg_pro from linked list, traverse until matching pid is found 
    while (curr != NULL && curr->pid != pid) {
        prev = curr;
        curr = curr->next;
    }
    // if curr is NULL then pid not found 
    if (curr == NULL) {
        printf("bg process not found so it cannot be removed\n");
        return;
    }
    // otherwise remove the matching pid process and print that the process has terminated        
    prev->next = curr->next;
    printf("%d: %s ", curr->pid, curr->cwd);
    for (char **user_cmd = curr->command; *user_cmd != NULL; user_cmd++) {
        printf("%s ", *user_cmd);
    }
    printf("has terminated.\n");
    free_args(curr->command);
    free(curr);
}

void print_bglist(struct bg_pro* root){
    // print a list of all the bg_pro's and their pid, cwd, command, arguments and options and total number of bg_pro's
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

void redirect_output() {
    // redirect the output of the program to /dev/null so that stdout and stderr dont print to console    
    int devnull = open("/dev/null", O_WRONLY);
    if (devnull == -1) {
        perror("error opening /dev/null");
        exit(EXIT_FAILURE);
    }
    if(dup2(devnull, STDOUT_FILENO) == -1) {
        perror("error redirecting stdout to /dev/null");
        exit(EXIT_FAILURE);
    }
    if(dup2(devnull, STDERR_FILENO) == -1) {
        perror("error redirecting stderr to /dev/null");
        exit(EXIT_FAILURE);
    }
    close(devnull);
}

void free_linkedlist(struct bg_pro* root) {
    // free the memory allocated for the linked list
    struct bg_pro* curr = root;
    while (curr != NULL) {
        struct bg_pro* next = curr->next;
        free_args(curr->command);
        free(curr);
        curr = next;
    }
}


//* MAIN FUNCTION
int main(int argc, char *argv[]) {
    // handle Ctrl C in parent process 
    signal(SIGINT, SIG_IGN);

    struct bg_pro* root = NULL;
    while (1) {        
        //* check for completed child processes and remove them from the linked list
        if (root != NULL) {
            pid_t ter;
            while((ter = waitpid(0, NULL, WNOHANG)) > 0) {
                remove_bgpro_from_list(&root, ter);
            }
        }
        char* user_input = prompt_input();

        // if user input is empty, reprompt
        if(user_input[0] == '\0'){            
            free(user_input);
            continue;
        }
        // otherwise determine if user is changing directories, executing process in background, listing background processes, or executing process in foreground
        char *first_token = strtok(user_input, " ");
        char *token = strtok(NULL, " ");    
        if (strcmp(first_token, "cd") == 0){
            //* CHANGING DIRECTORIES
            // determine whether to go to home directory or somewhere else
            char *home_dir = getenv("HOME");            
            if (token == NULL || strcmp(token, "~") == 0) {                
                if (chdir(home_dir) != 0) {
                    perror("cd");                    
                }
            } else {
                char first_char = token[0];
                if (first_char == '~') {
                    if (chdir(home_dir) != 0 || chdir(token+2) != 0) {
                        fprintf(stderr, "cd: No such file or directory: %s/%s\n", home_dir, token+2);
                    }
                } else {
                    if (chdir(token) != 0) {
                        fprintf(stderr, "cd: No such file or directory: %s\n", token);
                    }
                }   
            }      
        } else if (strcmp(first_token, "bg") == 0){
            //* BACKGROUND EXECUTION             
            pid_t pid = fork();
            // get the command, arguments and options 
            char *args[100]; 
            int i = 0;
            while (token != NULL) {
                args[i++] = token;      
                token = strtok(NULL, " ");
            }
            args[i] = NULL;

            if (pid == -1) {
                perror("Failed to fork");
            } else if (pid == 0) { 
                //* child process
                // redirect execvp stdout and stderr to /dev/null so it doesnt output to terminal
                redirect_output();
                
                // run execvp with the user input
                if (execvp(args[0], args) == -1) {
                    perror(args[0]);
                }
                free(user_input);
                exit(EXIT_FAILURE);
            } else {
                //* parent process         
                // create new bg_pro and add it to linked list 
                add_bgpro_to_list(&root, pid, args);
            }
        } else if (strcmp(first_token, "bglist") == 0){
            //* BACKGROUND PROCESSES LIST            
            print_bglist(root);
        } else if (strcmp(first_token, "exit") == 0){
            // exit program if user inputs "exit"
            free(user_input);
            free_linkedlist(root);
            break;
        } else {
            //* FOREGROUND EXECUTION
            pid_t pid = fork();
            if (pid == -1) {
                perror("Failed to fork");
            } else if (pid == 0) {
                //* child process                         
                // handle Ctrl C in the child process
                signal(SIGINT, SIG_DFL);

                // get the command, arguments and options  
                char *args[100];
                int i = 0;       
                args[i++] = first_token;
                while (token != NULL) {
                    args[i++] = token;
                    token = strtok(NULL, " ");
                }
                args[i] = NULL;

                // run execvp with the user input 
                if (execvp(args[0], args) == -1) {
                    perror(args[0]);
                }
                free(user_input);
                exit(EXIT_FAILURE);
            } else {
                //* parent process
                int status;
                waitpid(pid, &status, 0);
            }
        }
        // free user_input after done processing
        free(user_input);       
    }
    return 0;
}