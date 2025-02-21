1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  When a process calls fork(), the operating system creates a new child process that is an exact copy of the parents, inheriting its memory, file descriptors, and execution state. Using execvp after fork replaces the calling process's address space with a new program and preserves file descriptors. By using `fork` before `execvp`, the parent process can remain intact and continue its work, while the child process executes the new program with `execvp`.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails, our f_result returns a value less than 0. In that case, we throw and print an error and exit with a value of 1. This allows the user to know that an error has occurred but allows them to continue to input commands inside the dragon shell and not crash the program.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  `execvp()` finds the command to execute by searching for the command in directories listed in the `PATH`  environment variable. It will recurse through the subdirectories until it finds the command the execute.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Calling wait() in the parent process blocks the parent process until the child process exits. If we didn't call wait(), the child process will become a "zombie" process, meaning that it still appears in the system as a running process even if it has exited. This consumes system resources unnecessarily.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  `WEXITSTATUS()` extracts the status code from the child process, which is returned by `wait()`. The `exit()` value from the child process indicates the success or failure of the program, so we can capture and use this information and handle it accordingly in the parent process.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of `build_cmd_buff()` handles quoted arguments by ensuring that spaces inside quotes are treated as part of an argument, instead of a delimiter. This is necessary because arguments in a command-line may contain spaces, especially when the user wants to pass a multi-word argument that should be treated as a single unit. The `build_cmd_buff()` uses a flag (in_quote) to track if the parser is inside a quoted string. It also handles spaces inside and outside quotes differently.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Instead of keeping track of a flag for `space_found`, I kept track of a flag for `in_quote`. For the previous assignment, I did not have to initialize a brand new pointer since I was only concerned with trimming any excess spaces between arguments and returning the same pointer. For this assignment, I introduced three new variables, `*start`, `temp_arg[ARG_MAX]` and `temp_index` so that each new argument can be stored in its respective cmd_buff argument. I had to keep track of more conditions in this new version, so it was slightly confusing to refactor my code initially.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are asychronous notifications used to communicate events and control processes such as interrupting, terminating, or pausing processes. Other forms of interprocess communication like pipes involve data communication and synchronization between processes. Signals are more simple, intended for notifications and control, while other forms of IPC are more complex that allow for data sharing.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL is a termination signal that cannot be caught, blocked, or ignored by the process. It tells the kernel to kill the process, so it usually used when a process becomes unresponsive and doesn't terminate with a normal exit signal. SIGTERM is another terminal signal, but unlike SIGKILL, the process can catch and handle this signal. When we release memory or close database connections, applications usually catch SIGTERM and perform their shutdown routines. SIGINT means that we've experienced an interruption from keyboard, usually with Ctrl+C. Like SIGTERM, SIGINT can be caught and handled by the process, allowing it to clean up before exiting. Users may use SIGINT to stop processes because they no longer need the process or the process is taking too long.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When it process receives SIGSTOP, the process stops and it cannot be caught, blocked, or ignored. This is because they are handled by the kernel and are used to kill unresponsive tasks.
