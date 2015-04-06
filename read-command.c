// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

/*

char read_char ()
{
	
}

bool is_valid(char c) 
{	// check if it is a valid character for simple commands
	// it does not include the eight special token for complete commands
	if (isalnum(c) || c == '!' || c == '%' || c == '+' || c == ',' || c == '-' || c == '.' 
			|| c =='/' || c == ':' || c == '@' || c == '^' || c == '_' || c == ' ')
		return true;
	else 
		return false;
}

command_t make_simple_command ()
{	// words, input, output
	
}

command_t make_complete_command ()
{	// for complete commands including subshell commands
	// type, 		
}


//stack for operators 

int  compare_operator ()
{
	int current_opr;
	int stack_opr;
	
	return (current_opr - stack_opr);
}
*/

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
