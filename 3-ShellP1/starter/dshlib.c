#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char *cmd_token = strtok(cmd_line, PIPE_STRING);
    char *commands[CMD_MAX];
    int command_count = 0;

    while (cmd_token != NULL)
    {
        trim_spaces(cmd_token);
        commands[command_count] = cmd_token;
        command_count++;
        cmd_token = strtok(NULL, PIPE_STRING);
    }

    if (command_count > CMD_MAX)
    {
        return ERR_TOO_MANY_COMMANDS;
    }

    for (int i = 0; i < command_count; i++)
    {
        char *exe = strtok(commands[i], SPACE_STRING);
        char *args = strtok(NULL, "");
        if (args != NULL)
        {
            trim_spaces(args);
            trim_spaces_between_words(args);
            if (strlen(args) > ARG_MAX)
            {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
        }

        if (strlen(exe) > EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        strcpy(clist->commands[i].exe, exe);
        clist->commands[i].exe[strlen(exe)] = '\0';
        if (args != NULL)
        {
            strcpy(clist->commands[i].args, args);
            clist->commands[i].args[strlen(args)] = '\0';
        }
        else
        {
            clist->commands[i].args[0] = '\0';
        }
    }

    clist->num = command_count;
    return OK;
}

void trim_spaces(char *str)
{
    // Trim leading spaces
    while (*str == SPACE_CHAR)
    {
        str++;
    }

    // Trim trailing spaces
    char *end = str + strlen(str) - 1;
    while (end > str && *end == SPACE_CHAR)
    {
        end--;
    }

    *(end + 1) = '\0';
}

void trim_spaces_between_words(char *str)
{
    int i = 0;
    int j = 0;
    int space_found = 1;

    while (str[i])
    {
        if (str[i] == SPACE_CHAR)
        {
            if (!space_found)
            {
                str[j++] = ' ';
                space_found = 1;
            }
        }
        else
        {
            str[j++] = str[i];
            space_found = 0;
        }
        i++;
    }
    str[j] = '\0';
}
