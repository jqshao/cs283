#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 *
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 *
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 *
 *   Also, use the constants in the dshlib.h in this code.
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 *
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 *
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 *
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    int rc = 0;
    command_list_t *cmd_list = malloc(sizeof(command_list_t));

    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        rc = build_cmd_list(cmd_buff, cmd_list);
        if (rc == WARN_NO_CMDS)
        {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
        if (rc == ERR_TOO_MANY_COMMANDS)
        {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }

        // Check for built-in commands before forking
        Built_In_Cmds bi_rc = exec_built_in_cmd(&cmd_list->commands[0]);
        if (bi_rc == BI_CMD_EXIT)
        {
            break;
        }
        else if (bi_rc == BI_EXECUTED)
        {
            continue;
        }

        execute_pipeline(cmd_list);
    }

    free_cmd_list(cmd_list);
    free(cmd_buff);
    return OK;
}

int tokenizer(char *cmd_line, char *commands[], char *delim)
{
    int command_count = 0;
    commands[command_count] = strtok(cmd_line, delim);

    while (commands[command_count] != NULL)
    {
        trim_spaces(commands[command_count]);
        command_count++;
        commands[command_count] = strtok(NULL, delim);
    }

    return command_count;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    clear_cmd_buff(cmd_buff);
    int cmd_index = 0;

    trim_spaces(cmd_line);
    strcpy(cmd_buff->_cmd_buffer, cmd_line);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';

    char *exe = strtok(cmd_line, SPACE_STRING);
    if (strlen(exe) > EXE_MAX)
    {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }
    cmd_buff->argv[cmd_index] = exe;
    cmd_buff->argc++;

    char *args = strtok(NULL, "");
    if (args == NULL)
    {
        return OK;
    }
    store_args(args, cmd_buff);

    return OK;
}

void store_args(char *args, cmd_buff_t *cmd_buff)
{
    char *start = args;
    int in_quote = 0;
    char temp_arg[ARG_MAX];
    int temp_index = 0;

    while (*start != '\0')
    {
        if (!in_quote && *start == SPACE_CHAR && temp_index > 0)
        {
            temp_arg[temp_index] = '\0';
            cmd_buff->argv[cmd_buff->argc] = strdup(temp_arg);
            cmd_buff->argc++;
            temp_index = 0;
        }

        // Toggle in_quote state if we encounter a quote character
        if (*start == QUOTE_CHAR)
        {
            in_quote = !in_quote;
            start++;
            continue;
        }

        // Treat the space as part of the argument if inside quotes
        if (in_quote)
        {
            temp_arg[temp_index++] = *start;
        }
        else if (*start != SPACE_CHAR)
        {
            temp_arg[temp_index++] = *start;
        }

        start++;
    }

    // Handles the last argument
    if (temp_index > 0)
    {
        temp_arg[temp_index] = '\0';
        cmd_buff->argv[cmd_buff->argc++] = strdup(temp_arg);
    }
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    // Put each command into the commands array split by PIPE_STRING
    int cmd_count = 0;
    char *commands[CMD_MAX];
    cmd_count = tokenizer(cmd_line, commands, PIPE_STRING);
    if (cmd_count == 0)
    {
        return WARN_NO_CMDS;
    }
    if (cmd_count > CMD_MAX)
    {
        return ERR_TOO_MANY_COMMANDS;
    }

    for (int i = 0; i < cmd_count; i++)
    {
        alloc_cmd_buff(&clist->commands[i]);
        build_cmd_buff(commands[i], &clist->commands[i]);
    }

    clist->num = cmd_count;

    return OK;
}

void execute_pipeline(command_list_t *clist)
{
    int num_commands = clist->num;
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];

    for (int i = 0; i < num_commands - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0)
        {
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < num_commands - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < num_commands - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_commands; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff->_cmd_buffer == NULL)
    {
        return ERR_MEMORY;
    }
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff->_cmd_buffer != NULL)
    {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->argc = 0;
    if (cmd_buff->_cmd_buffer != NULL)
    {
        memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    }
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst)
{
    for (int i = 0; i < cmd_lst->num; i++)
    {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    return OK;
}

Built_In_Cmds match_command(const char *input)
{
    if (strcmp(input, EXIT_CMD) == 0)
    {
        return BI_CMD_EXIT;
    }
    if (strcmp(input, DRAGON_CMD) == 0)
    {
        return BI_CMD_DRAGON;
    }
    if (strcmp(input, "cd") == 0)
    {
        return BI_CMD_CD;
    }

    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    Built_In_Cmds match = match_command(cmd->argv[0]);
    switch (match)
    {
    case BI_CMD_EXIT:
        return BI_CMD_EXIT;
    case BI_CMD_DRAGON:
        print_dragon();
        return BI_EXECUTED;
    case BI_CMD_CD:
        if (cmd->argc == 2)
        {
            if (chdir(cmd->argv[1]) != 0)
            {
                perror("chdir failed");
            }
        }
        return BI_EXECUTED;
    default:
        break;
    }
    return match;
}

void trim_spaces(char *str)
{
    // Trim leading spaces
    char *start = str;
    while (*start == SPACE_CHAR)
    {
        start++;
    }

    // Trim trailing spaces
    char *end = start + strlen(start) - 1;
    while (end > start && *end == SPACE_CHAR)
    {
        end--;
    }

    memmove(str, start, end - start + 1);
    str[end - start + 1] = '\0';
}
