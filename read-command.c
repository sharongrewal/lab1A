// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


char read_char (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
	return (char) (*get_next_byte)(get_next_byte_argument);
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

command_t make_complete_command (char* curr, command_stream current_command)
{	// for complete commands including subshell commands
	// type, 	
	
	command_t new_command = 
}


//stack for operators 

int  compare_operator ()
{
	int current_opr;
	int stack_opr;
	
	return (current_opr - stack_opr);
}


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  //error (1, 0, "command reading not yet implemented");
  //return 0;
  
  char curr; //current character
  char prev = ' '; //previous character
  command_stream_t root = NULL;
  command_stream_t current = NULL; //this is used to point to the current command_t
  //buffer
  
  curr = read_char (get_next_byte, get_next_byte_argument);
  
  while(curr != EOF)
  {
  	if(is_valid(curr))
  	{
  		
  	}
  	else if( curr== ';')
  	{
  		
  	}
  	else if( curr== '|')
  	{
  		if( )	
  	}
  	else if( curr== '&')
  	{
  		
  	}
  	else if( curr== '(' || curr ==')')
  	{
  		
  	}
  	else if( curr == '>' || curr == '<')
  	{
  		
  	}
  	else if( curr== '\n')
  	{
  	//one newline is treated as a ';'
  	//newline can not appear after '<' or '>'
  	//newline can ONLY appear before '(' or ')'
  	}
  	else if ( curr== '#')
  	{
  		//up to but not including the newline
  	}
  	else if (curr != ' ')
  	{
  		//anything invalid.
  	}
  	
  	prev = curr;
  	
  	curr = read_char (get_next_byte, get_next_byte_argument);
  	// handle about comments after #, need to skip everything
  }
  
  
}

command_t
read_command_stream (command_stream_t s)
{
  	command_t output = s -> current_root_command;
	if (s -> next_command_stream == NULL)
		memset(s,0,sizeof(struct command_stream));
	else
		memcpy(s,s->next_command_stream,sizeof(struct command_stream));
		//memset(s->next_command_stream,0,sizeof(struct command_stream));
	return output;
}
