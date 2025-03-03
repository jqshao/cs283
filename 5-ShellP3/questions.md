1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_My implementation ensures that all child processes complete before the shell continues to accept user input by using the waitpid() function. The function waits for each child process to terminate, so that the parent process, our dshell, does not proceed and accept more input before all child processes finish executing. If we did not call waitpid() on all child processes, the child process might become a "zombie" process, meaning that it still appears in the system as a running process even if it has exited. This consumes system resources unnecessarily._

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_If we leave our pipes open after calling dup(), processes might continue waiting for input that will never arrive because the write end of the pipe is still open somewhere. This prevents the EOF signal from being sent. If we do not close unused pipe ends, the process might read from or write to the wrong end, which leads to incorrect data processing in our process._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_If cd were implemented as an external command, it would run in a separate child process. This means that any changes made to the working directory in the child process would not affect the parent shell process. To effectively change the shell's working directory directory, the cd command must be executed within the context of the shell process itself, which is why is it implemented as a built-in command._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_To allow an arbitrary number of piped commands, we can use dynamic memory allocation to build and manage the command list. We can start by allocating memory for a small number of commands, and resize the command list dynamically as more commands are added. Some trade-offs to consider include having to manage memory with allocation, resizing, and deallocation. Resizing the command list and commands also brings a performance cost, especially if it happens frequently._
