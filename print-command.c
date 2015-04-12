// UCLA CS 111 Lab 1 command printing, for debugging

#include "command.h"
#include "command-internals.h"

#include <stdio.h>
#include <stdlib.h>

static void
command_indented_print (int indent, command_t c)
{
	 printf("it's print command1\n");
  switch (c->type)
    {
    case AND_COMMAND:
    case SEQUENCE_COMMAND:
    case OR_COMMAND:
    case PIPE_COMMAND:
      {
	command_indented_print (indent + 2 * (c->u.command[0]->type != c->type),
				c->u.command[0]);
	static char const command_label[][3] = { "&&", ";", "||", "|" };
	printf (" \\\n%*s%s\n", indent, "", command_label[c->type]);
	command_indented_print (indent + 2 * (c->u.command[1]->type != c->type),
				c->u.command[1]);
	break;
      }

    case SIMPLE_COMMAND:
      {
      	printf("it's print command2\n");
	char **w = c->u.word;
	
	printf("it's print command3\n");
	printf ("%*s%s", indent, "", *w);
	
	printf("it's print command4\n");
	while (*++w)
	{
	  printf (" %s", *w);
	  printf("it's print command5\n");
	}
	break;
      }

    case SUBSHELL_COMMAND:
      printf ("%*s(\n", indent, "");
      command_indented_print (indent + 1, c->u.subshell_command);
      printf ("\n%*s)", indent, "");
      break;

    default:
	 printf("going to abort in print_command\n");
      abort ();
    }

  if (c->input)
    printf ("<%s", c->input);
  if (c->output)
    printf (">%s", c->output);
}

void
print_command (command_t c)
{
	
  command_indented_print (2, c);
  printf("it's print command\n");
  putchar ('\n');
}
