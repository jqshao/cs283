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
    cmd_buff_t cmd;

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

        alloc_cmd_buff(&cmd);

        // Build the command buffer and parse the command into exe and args
        rc = build_cmd_buff(cmd_buff, &cmd);

        Built_In_Cmds bi_rc = exec_built_in_cmd(&cmd);
        if (bi_rc == BI_CMD_EXIT)
        {
            free_cmd_buff(&cmd);
            break;
        }
        if (bi_rc == BI_NOT_BI)
        {
            exec_cmd(&cmd);
            continue;
        }

        free_cmd_buff(&cmd);
    }

    free(cmd_buff);
    return OK;
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

int exec_cmd(cmd_buff_t *cmd)
{
    int f_result, c_result;

    f_result = fork();
    if (f_result < 0)
    {
        perror("fork error");
        exit(1);
    }

    if (f_result == 0)
    {
        // Child process will exec
        int rc;
        rc = execvp(cmd->argv[0], cmd->argv);
        if (rc < 0)
        {
            perror("fork error");
            exit(1);
        }
    }
    else
    {
        // Parent process
        wait(&c_result);
    }
    return OK;
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
        return BI_RC;
    case BI_CMD_CD:
        if (cmd->argc == 2)
        {
            if (chdir(cmd->argv[1]) != 0)
            {
                perror("chdir failed");
            }
        }
        return BI_RC;
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
