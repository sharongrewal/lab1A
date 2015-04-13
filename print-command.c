// UCLA CS 111 Lab 1 command printing, for debugging

#include "command.h"
#include "command-internals.h"

#include <stdio.h>
#include <stdlib.h>

static void
command_indented_print (int indent, command_t c)
{
printf("%d: prblem in print_command, c->type ",__LINE__);
  switch (c->type)
    { printf("%d: c->type read",__LINE__);
    case AND_COMMAND:
    case SEQUENCE_COMMAND:
    case OR_COMMAND:
    case PIPE_COMMAND:
      { 
      	printf("%d: prblem in print_command ",__LINE__);
	command_indented_print (indent + 2 * (c->u.command[0]->type != c->type),
				c->u.command[0]);
	printf("%d: prblem in print_command ",__LINE__);
	static char const command_label[][3] = { "&&", ";", "||", "|" };
	printf("%d: prblem in print_command ",__LINE__);
	printf (" \\\n%*s%s\n", indent, "", command_label[c->type]);
	printf("%d: prblem in print_command ",__LINE__);
	command_indented_print (indent + 2 * (c->u.command[1]->type != c->type),
				c->u.command[1]);
	break;
      }

    case SIMPLE_COMMAND:
      {
      	printf("%d: prblem in print_command ",__LINE__);
	char **w = c->u.word;
	
	printf ("%*s%s", indent, "", *w);
	
	while (*++w)
	{
	  printf (" %s", *w);
	}
	break;
      }

    case SUBSHELL_COMMAND:
      printf ("%*s(\n", indent, "");
      command_indented_print (indent + 1, c->u.subshell_command);
      printf ("\n%*s)", indent, "");
      break;

    default:
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
  putchar ('\n');
}
