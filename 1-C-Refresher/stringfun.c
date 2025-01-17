#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

// prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);

// prototypes for functions to handle required functionality
int count_words(char *, int, int);
// add additional prototypes here
int reverse_string(char *, int);
void print_word(char *, int, int, int, int);
int print_words(char *, int);

int setup_buff(char *buff, char *user_str, int len)
{
    // TODO: #4:  Implement the setup buff as per the directions
    int count = 0;
    int null_terminated = 0;
    int in_whitespace = 0;
    int index = 0;

    while (count < len + 1)
    {
        if (*(user_str + index) == '\0')
        {
            null_terminated = 1;
            break;
        }

        if (*(user_str + index) == ' ')
        {
            if (!in_whitespace)
            {
                in_whitespace = 1;
                *(buff + count) = ' ';
                count++;
            }
        }
        else
        {
            in_whitespace = 0;
            *(buff + count) = *(user_str + index);
            count++;
        }

        index++;
    }

    if (!null_terminated)
    {
        return -1;
    }

    for (int i = count; i < len; i++)
    {
        *(buff + i) = '.';
    }

    return count;
}

void print_buff(char *buff, int len)
{
    printf("Buffer:  ");
    for (int i = 0; i < len; i++)
    {
        putchar(*(buff + i));
    }
    putchar('\n');
}

void usage(char *exename)
{
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int len, int str_len)
{
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++)
    {
        if (*(buff + i) != ' ' && !in_word)
        {
            in_word = 1;
            word_count++;
        }
        else if (*(buff + i) == ' ')
        {
            in_word = 0;
        }
    }

    return word_count;
}

int reverse_string(char *buff, int str_len)
{
    // Reverse and print out the string
    printf("Reversed String:  ");
    for (int i = str_len - 1; i >= 0; i--)
    {
        putchar(*(buff + i));
    }
    putchar('\n');

    return 0;
}

void print_word(char *buff, int start, int end, int word_count, int word_length)
{
    if (word_length > 0)
    {
        printf("%d. ", word_count);
        for (int j = start; j < end; j++)
        {
            putchar(*(buff + j));
        }
        printf(" (%d)\n", word_length);
    }
}

int print_words(char *buff, int str_len)
{
    printf("Word Print\n");
    printf("----------\n");
    int word_count = 1;
    int word_length = 0;

    // The following loop traverses the string and prints out each word
    // and its length. It checks for spaces to determine when a word has
    // ended and prints the word and its length when a space is found.
    for (int i = 0; i < str_len; i++)
    {
        if (*(buff + i) == ' ')
        {
            print_word(buff, i - word_length, i, word_count, word_length);
            if (word_length > 0)
            {
                word_count++;
            }
            word_length = 0;
        }
        else
        {
            word_length++;
        }

        // If we are at the end of the string and the last character is not a space,
        // we print the last word and its length
        if (i == str_len - 1 && *(buff + i) != ' ')
        {
            print_word(buff, i - word_length + 1, i + 1, word_count, word_length);
        }
    }

    putchar('\n');
    return 0;
}

// ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[])
{

    char *buff;         // placehoder for the internal buffer
    char *input_string; // holds the string provided by the user on cmd line
    char opt;           // used to capture user option from cmd line
    int rc;             // used for return codes
    int user_str_len;   // length of user supplied string

    // TODO:  #1. WHY IS THIS SAFE, aka what if argv[1] does not exist?
    // We first perform a check to see if argc is less than 2. If argc is less than 2,
    // then that means we have passed in only the program name and no other arguments.
    // Since C performs short circuit evaluation with "||," we won't need to evaluate
    // *argv[1] if we already determined no other arguments are getting passed into
    // the program.
    if ((argc < 2) || (*argv[1] != '-'))
    {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1] + 1); // get the option flag

    // handle the help flag and then exit normally
    if (opt == 'h')
    {
        usage(argv[0]);
        exit(0);
    }

    // WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    // TODO:  #2 Document the purpose of the if statement below
    // Our Text Line Processor utility takes the program name, command line operation,
    // and input string from the user. If we pass less than 3 values into the shell,
    // we are missing something from the user and will return exit with value 1 to
    // indicate a command line problem.
    if (argc < 3)
    {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; // capture the user input string

    // TODO:  #3 Allocate space for the buffer using malloc and
    //           handle error if malloc fails by exiting with a
    //           return code of 99
    //  CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ);
    if (buff == NULL)
    {
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ); // see todos
    if (user_str_len < 0)
    {
        printf("Error setting up buffer, error = %d\n", user_str_len);
        exit(2);
    }

    switch (opt)
    {
    case 'c':
        rc = count_words(buff, BUFFER_SZ, user_str_len); // you need to implement
        if (rc < 0)
        {
            printf("Error counting words, rc = %d\n", rc);
            exit(2);
        }
        printf("Word Count: %d\n", rc);
        break;
    case 'r':
        rc = reverse_string(buff, user_str_len);
        if (rc < 0)
        {
            printf("Error reversing string, rc = %d\n", rc);
            exit(2);
        }
        break;
    case 'w':
        rc = print_words(buff, user_str_len);
        if (rc < 0)
        {
            printf("Error printing words, rc = %d\n", rc);
            exit(2);
        }
        break;
    case 'x':
        if (argc == 5)
        {
            printf("Not Implemented!\n");
            exit(2);
        }
        else
        {
            printf("You need to provide the 3 required arguments for this option!\n");
            exit(2);
        }
        break;

    // TODO:  #5 Implement the other cases for 'r' and 'w' by extending
    //        the case statement options
    default:
        usage(argv[0]);
        exit(1);
    }

    // TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}

// TODO:  #7  Notice all of the helper functions provided in the
//           starter take both the buffer as well as the length.  Why
//           do you think providing both the pointer and the length
//           is a good practice, after all we know from main() that
//           the buff variable will have exactly 50 bytes?
//
//           PLACE YOUR ANSWER HERE
