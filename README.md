# CSc 360: Operating Systems (Fall 2024)

# Programming Assignment 1 (P1)
# A Simple Shell Interpreter (SSI)

> **Spec Out**: Sept 13, 2024 <br>
> **Code Due**: Oct 4, 2024

## Table of Contents

1. [Introduction](#1-introduction)
2. [Schedule](#2-schedule)
3. [Requirements](#3-requirements)
    1. [Foreground Execution](#31-foreground-execution-5-marks)
    2. [Changing Directories](#32-changing-directories-5-marks)
    3. [Background Execution](#33-background-execution-5-marks)
4. [Bonus Features](#4-bonus-features)
5. [Submission](#5-submission)
    1. [Version Control and Submission](#51-version-control-and-submission)
    2. [Submission Requirements](#52-submission-requirements)
6. [Rubric](#6-rubric)
    1. [Basic Functions](#basic-functions-total-1-mark-025-mark-each)
    2. [Foreground Execution](#foreground-execution-total-4-marks-1-mark-each)
    3. [Changing Directories](#changing-directories-total-5-marks-1-mark-each)
    4. [Background Execution](#background-execution-total-5-marks-1-mark-each)
7. [Odds and Ends](#7-odds-and-ends)
    1. [Compilation](#71-compilation)
    2. [Helper Programs](#72-helper-programs)
    3. [Code Quality](#73-code-quality)
    4. [Plagiarism](#74-plagiarism)

## 1. Introduction

In this assignment, you will implement a simple [shell interpreter](https://en.wikipedia.org/wiki/Unix_shell) (`SSI`), using system calls and functions to interact with the Linux operating system.

The `SSI` will be very similar to the common `bash` shell found on Linux, while you are going to implement a set of fundamental features: it should support the foreground and background execution of programs and the ability to change directories.

You shall implement your solution in `C` programming language only. Your work will be tested on `linux.csc.uvic.ca`, which you can remotely log in by ssh. You can also access Linux computers in ECS labs in person or remotely by following the guide at https://itsupport.cs.uvic.ca/services/e-learning/login_servers/.

**Be sure to test your code on `linux.csc.uvic.ca` before submission.**

Many students have developed their programs for their macOS or Windows laptops only to find that their code works differently on `linux.csc.uvic.ca` resulting in a substantial loss of marks.

Be sure to study the manual pages for the various system calls and functions suggested in this assignment. These functions are in Section 2 of the manual pages. For example, if you want to check the manual page for the `waitpid()` system call, you should type

```
$ man 2 waitpid
```

## 2. Schedule

In order to help you finish this programming assignment on time successfully, the schedule of this assignment has been synchronized with both the lectures and the tutorials.

There are three tutorials arranged during the course of this assignment.

| Date          | Tutorial                                             | Milestones by the Friday of that week |
|---------------|------------------------------------------------------|---------------------------------------|
| Sept 16/17/18 | T2: P1 spec go-thru, design hints, system calls      | design and code skeleton              |
| Sept 23/24/25 | T3: code design & implementation                     | code design/implementation done       |
| (online only) | T4: code implementation & testing; submission issues | code implementation/testing done      |

## 3. Requirements

### 3.1 Foreground Execution

Once launched, your `SSI` should show the following prompt waiting for user input:

```
username@hostname: /home/user >
```

For example,

```
clarkzjw@linux204:~/CSC360$ ./ssi
clarkzjw@linux204: /home/clarkzjw/CSC360 >
```

The prompt includes the current directory name in absolute path, e.g., `/home/user`.

You can use `getcwd()` to obtain the current directory, `getlogin()` to get the username, and `gethostname()` to get the name of the host running the program.

Using `fork()` and `execvp()`, implement the ability for the user to execute arbitrary commands using your `SSI` shell.

For example, if the user types `ls -l /usr/bin`, followed by `Enter`,

```
username@hostname: /home/user > ls -l /usr/bin
```

your `SSI` should run the `ls` program with the parameters `-l` and `/usr/bin`, which should list the contents of the `/usr/bin` directory on the screen. The expected output should be exactly the same as if you had typed `ls -l /usr/bin` in a `bash` shell.

You can use the `readline` library and/or `strtok()` to process the input line/token.

**Note**: The above example uses 2 arguments. We will, however, test your `SSI` by invoking programs that take arbitrary number of arguments (no arguments or arbitrary many). A well-written `SSI` should support as many arguments as given on the command line.

### 3.2 Changing Directories

You `SSI` should support changing the current working directory using the `cd` command.

You can use `getcwd()` and `chdir()` to implement this feature.

For example, suppose the following directory structure exists in the home directory of the user:

```
.
├── A
│   └── C
└── B
```

A well-written `SSI` should support the following sequence of commands:

```
clarkzjw@linux204: /home/clarkzjw > cd A
clarkzjw@linux204: /home/clarkzjw/A > cd ../B
clarkzjw@linux204: /home/clarkzjw/B > cd ../A/C
clarkzjw@linux204: /home/clarkzjw/A/C > cd ../..
clarkzjw@linux204: /home/clarkzjw >
```

**Note**: `SSI` should always show the current working directory in the prompt.

The `cd` command should take no more than one argument, i.e., the name of the directory to change into, and ignore all others.

Note that no leading `/` indicates that the directory changed into is relative to the current directory. Please note the special argument `.` indicates the current directory, and `..` indicates the parent directory of the current directory.

That is, if the current directory is `/home/user/subdir` and the user types:

```
username@hostname: /home/user/subdir > cd ..
```

the current working directory will become `/home/user`.

The special argument `~` indicates the home directory of the current user. If `cd` is used without any argument, it is equivalent to `cd ~`, i.e., returning to the home directory, e.g., `/home/user`.

**Q**: How do you know the user's home directory location? <br>
**Hint**: From the `$HOME` environment variable with the `getenv()` system call.

**Note**: There is no such a program called `cd` in the system that you can run directly (as you did with `ls`) to change the current working directory of the calling program, even if you created one (why?), i.e., you have to use the system call `chdir()`. In other words, `cd` is a [shell built-in](https://en.wikipedia.org/wiki/Shell_builtin) command implemented by the shell itself, instead of an external program.

### 3.3 Background Execution

Many shell implementations allow background execution of programs, that is, an external program is running in the background, while the shell continues to accept input from the user at prompt.

You will implement a simplified version of background execution that supports executing processes in the background. The maximum number of background processes is not limited. (**Hint**: What data structure can you use?)

If the user types: `bg cat foo.txt`, your `SSI` shell should start the command `cat` with the argument `foo.txt` in the background. That is, the program `cat foo.txt` will be executed in the background and the `SSI` shell will continue show the prompt to accept more commands in the foreground.

The command `bglist` will have the `SSI` shell display a list of all the programs, including their execution arguments, **currently** executing in the background, e.g.,:

```
123:  /home/user/a1/foo 1
456:  /home/user/a1/foo 2
Total Background jobs:  2
```

In this case, there are 2 background jobs, both running the program `foo`, the first one with process ID (PID) 123 and execution argument 1 and the second one with PID 456 and argument 2.

Your `SSI` shell must indicate to the user after background jobs have terminated. Read the man page for the `waitpid()` system call. You are suggested to use the `WNOHANG` option. E.g.,

```
username@hostname: /home/user/subdir > cd
456: /home/user/a1/foo 2 has terminated.
username@hostname: /home/user >
```

**Q**: How do you make sure your `SSI` has this behavior? <br>
**Hint**: Check the list of background processes every time processing a user input.

## 4. Bonus Features

Only a simplified shell with limited functionality is required in this assignment. However, students have the option to extend their design and implementation to include more features in a regular shell or a remote shell.

Example bonus features include:

+ Capture and redirect program output to a file, e.g., `ls -l > output.txt`, `ping -c 4 1.1.1.1 > ping.txt`.
+ Command history, e.g., use the up and down arrow keys to navigate through the command history.
+ Killing/pausing/resuming background processes, e.g., `bgkill 123`, `bgstop 123`, `bgstart 123`.

If you want to design and implement a bonus feature, you should contact the course instructor by Teams for permission one week before the due date, and clearly indicate the feature in the submission of your code with a README.txt file. The credit for approved and correctly implemented bonus features will not exceed 20% of the full marks for this assignment.

## 5. Submission

### 5.1 Version Control and Submission

You shall use the department GitLab server `https://gitlab.csc.uvic.ca/` for version control during this assignment.

You shall already have a personal assignment group named with your Netlink ID, e.g., `https://gitlab.csc.uvic.ca/courses/2024091/CSC360_COSI/assignments/<netlink_id>`, which already contains an `assignments_notes` repository.

You shall create a new repository named `p1` under your personal assignment group, e.g., `https://gitlab.csc.uvic.ca/courses/2024091/CSC360_COSI/assignments/<netlink_id>/p1`, and commit your code to this repository.

You are encouraged to commit and push your work-in-progress code regularly to your `p1` GitLab repository.

The submission of your `p1` assignment is the last commit before the due date. If you commit and push your code after the due date, the last commit before the due date will be considered as your final submission.

The TA will clone your `p1` repository, test and grade your code on `linux.csc.uvic.ca`.

**Note**: Use [`.gitignore`](https://git-scm.com/docs/gitignore/en) to ignore the files that should not be committed to the repository, e.g., the executable file, `.o` object files, etc.

### 5.2 Submission Requirements

+ Your submission **must** include a `README.txt` file that describes which features you have implemented and whether it works correctly or not (not only the bonus parts). Including a `README.txt` file is mandatory, and you will lose marks if you do not submit it in your GitLab repository.

+ Your submission **must** compile successfully with a single `make` command. You can update the provided [Makefile](./sample/Makefile) to compile your code. The produced executable file **must** be named `ssi`. Otherwise, you will lose marks.

## 6. Rubric

### Basic Functions (Total: 1 Mark, 0.25 Mark each)

+ Your submission contains a `README.txt` that clearly describes which features you have implemented and whether they work correctly or not.
+ Your submission can be successfully compiled by `make` without errors. The produced executable is named `ssi`.
+ Your `ssi` can be executed without additional parameters, showing a prompt waiting for user input.
+ The prompt is in the correct `username@hostname: /home/user >` format.

### Foreground Execution (Total: 4 Marks, 1 Mark each)

+ Your `ssi` can execute `ls` without additional parameters and output the content correctly within the current directory.
+ Your `ssi` can execute `ls` with parameters, such as `ls -alh`, `ls -alh ./`, `ls -l /usr/bin` and output the content within the corresponding directory correctly.
+ Your `ssi` can execute long-running programs in the foreground, and it can respond to `Ctrl-C` to stop the program. For example, `ping 1.1.1.1`.
+ Your `ssi` can execute arbitrary external programs, and prints `<name>: No such file or directory` when the requested external program doesn't exist.

### Changing Directories (Total: 5 Marks, 1 Mark each)

+ Your `ssi` can print the current working directory by `pwd`.
+ Your `ssi` can change to directories by specifying absolute directory paths, such as `cd /usr/bin`, cd `/tmp`.
+ Your `ssi` can handle relative directory paths, such as `cd .`, `cd ..`, cd `../..`.
+ Your `ssi` can change to the user's home directory if no parameters are given to the `cd` command, and it can handle the special `~`.
+ The command prompt always prints the current working directory correctly.

### Background Execution (Total: 5 Marks, 1 Mark each)

+ Your `ssi` can use `bg` to execute programs in the background, such as `bg cat foo.txt`, `bg ping 1.1.1.1`.
+ When putting a job into the background using `bg`, your `ssi` can still accept user input in the foreground.
+ Your `ssi` can use `bglist` to list the currently running background jobs.
+ When a background job is finished, your `ssi` outputs `<pid>: <program> <parameters> has terminated.` when processing the next user input.
+ Your `ssi` can update background jobs correctly. For example, terminated background jobs should not appear in the `bglist` output.

## 7. Odds and Ends

### 7.1 Compilation

A sample [Makefile](./sample/Makefile) is provided to help you build the sample code. It takes care of linking-in the GNU readline library for you. The sample code shows you how to use `readline()` to get input from the user, only if you choose to use the readline library.

### 7.2 Helper Programs

#### 7.2.1 [inf.c](./sample/inf.c)

This program takes two parameters:

+ tag: a single word which is printed repeatedly
+ interval: the interval, in seconds, between two printings of the tag

The purpose of this program is to help you with debugging background processes. It acts as a trivial background process, whose presence can be "observed" since it prints a tag (specified by you) every few seconds (specified by you). This program takes a tag so that even when multiple instances of it are executing, you can tell the difference between each instance.

This program considerably simplifies the programming of the part of your SSI shell. You can find the running process by `ps -ef` and `kill -9 pid` by its process ID `pid`.

#### 7.2.2 [args.c](./sample/args.c)

This is a very trivial program which prints out a list of all arguments passed to it.

This program is provided so that you can verify that your shell passes all arguments supplied on the command line — Often, people have off-by-1 errors in their code and pass one argument less.

### 7.3 Code Quality

We cannot specify completely the coding style that we would like to see but it includes the following:

+ Proper decomposition of a program into subroutines — A 500 line program as a single routine won't suffice.

+ Comment - judiciously, but not profusely. Comments serve to help a marker. To further elaborate:

    + Your favorite quote from Star Wars does not count as comments. In fact, they simply count as anti-comments, and will result in a loss of marks.
    + Comment your code in English. It is the official language of this university.

+ Proper variable names, `int a;` is not a good variable name, it never was and never will be.

+ Small number of global variables, if any. Most programs need a very small number of global variables, if any. (If you have a global variable named temp, think again.)

+ The return values from all system calls listed in the assignment specification should be checked and all values should be dealt with appropriately.

+ If you are in doubt about how to write good C code, you can easily find many C style guides on the Net. The Indian Hill Style Guide http://www.cs.arizona.edu/~mccann/cstyle.html is an excellent short style guide.

### 7.4 Plagiarism

This assignment is to be done individually. You are encouraged to discuss the design of your solution with your classmates, but each person must implement their own assignment.

Your markers will submit the code to an automated plagiarism detection program. We add archived solutions from previous semesters (a few years worth) to the plagiarism detector, in order to catch "recycled" solutions, including those on GitHub.

The End
