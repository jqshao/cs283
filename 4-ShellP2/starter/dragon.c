#include <stdio.h>

// EXTRA CREDIT - print the drexel dragon from the readme.md
extern void print_dragon()
{
  FILE *file = fopen("dragon.txt", "r");
  if (file == NULL)
  {
    printf("Error: Could not open dragon.txt\n");
    return;
  }

  char line[256];
  while (fgets(line, sizeof(line), file))
  {
    printf("%80s", line);
  }

  printf("\n");
  fclose(file);
}
