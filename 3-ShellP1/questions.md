1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: We can use `fgets()` to safely read input into our char buffer, and `fgets()` allows us to handle EOF gracefully by returning a NULL.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  By allocating memory on the heap with `malloc()`, we can prevent potential stack overflows that may occur when we declare a large fixed-size array on the stack.


3. In `dshlib.c`, the function `build_cmd_list()` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  If we did not trim spaces, then we might compare a given command to the incorrect string. For example, if we want to change the directory using `cd`, we want to compare our command to exactly match `cd` instead of ` cd` or `cd `.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  We should implement the output redirection `>`, input redirection `<`, and append output redirection `>>` in our custom shell. Some of the challenges we might have implementing them include dealing with missing files, having the ability to open, read, and write files, and ensuring that our commands have the correct syntax. We don't want to accidentally read a `>>` redirection as a `>` redirection due to our incorrectly processing the redirection operators.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Piping allow us to pass output from one command to another command, while redirection allows us to pass input or output between files and commands.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: The separation of STDERR and STDOUT is important because it allows us to accurately process the output of one command to another, so that only the intended output from STDOUT is passed on. It also keeps our output clean and helps us debug our scripts and commands.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  To handle errors from commands that fail, our custom shell should print the message using STDERR to the terminal so that the user is aware of this error. Our custom shell should allow the user to check for the exit status for commands that fail. It is better to keep STDERR and STDOUT separate so that only the intended output from STDOUT is passed on and mixed with STDERR.