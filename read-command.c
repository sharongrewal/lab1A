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

command_t make_complete_command (char* curr, command_t stack)
{	// for complete commands
	// type, 
	// only LHS
	//curr is the token
	
	command_t new_command = (command_t)malloc(sizeof(struct command));
	switch(curr[0])
		{
			case ';':
				new_command -> type = SEQUENCE_COMMAND;
				break;
			case '&':
				new_command -> type = AND_COMMAND;
				break;
			case '|':
				new_command -> type = PIPE_COMMAND;
				break;
		}
	new_command -> u.command[0] = stack;
	return new_command;
}

command_t combine_complete_command (commant_t stack, command_t curr_command)
{ 
	stack -> u.command[1] = curr_command;	
	return stack;
}

//stack for commands
//stack of commands, stack_size starting from 0 to array size-1
//pushing stack 
// when calling push, stack_size should ++
void push (command_t curr_command, commnad_t* stack, int stack_size)
{
	stack[stack_size] = curr_command;
}

//popping stack
//after calling pop, stack_size should --
void pop (commnad_t* stack, int stack_size)
{
	stack[stack_size-1] = NULL; //should it be stack_num-1?
}

//need detailed implementation
//need one for END? and NONE?
int  compare_operator (command_type current_type, command_type stack )
{
	int current_opr;
	int stack_opr;
	
	if (current_type == SEQUENCE_COMMAND)
		current_opr = 1;
	else if (current_type == AND_COMMAND || current_type == OR_COMMAND)
		current_opr = 2;
	else if (current_type == PIPE_COMMAND)
		current_opr = 3;
	else if (current_type== SUBSHELL_COMMAND)
		current_opr = 4;

	if (stack == SEQUENCE_COMMAND)
		stack_opr = 1;
	else if (stack == AND_COMMAND || stack == OR_COMMAND)
		stack_opr = 2;
	else if (stack == PIPE_COMMAND)
		stack_opr = 3;
	else if (stack == SUBSHELL_COMMAND)
		stack_opr = 4;

	return (current_opr - stack_opr);
}

command_t make_command (char* curr, in_subshell)
{
	
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
  
  bool comments = false;
  int lineNumber =0; // keeping track of line number for error message
  char curr; //current character
  char prev = ' '; //previous character
  command_stream_t root = (command_stream_t)malloc(sizeof(struct command_stream));
  command_stream_t current_stream = (command_stream_t)malloc(sizeof(struct command_stream));
  
  current_stream  =root;	
  //command_stream_t current = NULL; //this is used to point to the current command_t
  //buffer
  
  command_type current_type = NULL;
  command_t current_command = (command_t) malloc (sizeof(struct command));
  //stack 
  int stack_size =0;
  int max_stack_size = 50l
  command_t* command_stack = (command_t*) malloc (sizeof(command_t)*max_stack_size);
  if( stack_size +1 == max_stack_size)
  {
  	max_stack_size = 2*max_stack_size;
  	command_stack = (command_t*) realloc (command_stack, max_stack_size);
  }
  curr = read_char (get_next_byte, get_next_byte_argument);
  
  
  while(curr != EOF)
  {
  	if(comments)
  	{
  		//just print? just 
  	}
  	else
  	{
	  	if(is_valid(curr))
	  	{
	  		// make_simple_command ()	
	  	}
	  	else if( curr== ';' ||  (curr == '&' && prev =='&') ||curr == '|')
	  	{
	  		/*if(buffer is empty )
	  		{
	  			it's something like &&& or |||
	  			fprintf(stderr, "ERROR");
	  			exit(1);
	  		}
	  		*/
	  		if(curr== ';' )
	  		{
	  			current_type = SEQUENCE_COMMAND;
	  		}
	  		else if (curr == '&' && prev =='&') 
	  		{
	  			current_type = AND_COMMAND;
	  		}
	  		else if(curr == '|' &&prev != '|')
	  		{
	  			current_type = PIPE_COMMAND;
	  		}
	  		else if(curr == '|' && prev =='|')
	  		{
	  			current_type = OR_COMMAND;
	  		}
	  		
	  		current_command = make_simple_command(buffer?);
	  		if(stack_size>0)
	  		{	
	  			 
	  			while (compare_operator(current_type, command_stack[stack_size-1]->type)>=0)
	  			{
	  				current_command =combine_complete_command(command_stack[stack_size-1],current_command);
	  				pop(command_stack,stack_size);
	  				stack_size --;
	  				
	  			}
	  			if(curr != EOF)
	  			{
	  				//if it's end of line, you won't need to start a new command
	  				make_complete_command (curr, command_stack[stack_size-1]);
	  				push(current_command, command_stack, stack_size-1);
	  			}
	  		}
	  		else
	  		{
	  			push(current_command, command_stack, stack_size);
	  			stack_size ++;
	  		}
	  		
	  	}
	  	else if( curr== '(' || curr ==')')
	  	{
	  		/*
	  		call make_command () until ')'
	  		current_command = output from that 
	  		*/
	  	}
	  	else if( curr == '>' || curr == '<')
	  	{
	  		
	  	}
	  	else if( curr== '\n')
	  	{
	  	//one newline is treated as a ';'
	  	//newline can not appear after '<' or '>'
	  	//newline can ONLY appear before '(' or ')'
	  		if(prev == '\n' && stack_size ==0)
	  		{
	  			//new command
	  			numLine ++;
	  			command_stream_t new_stream = (command_stream_t)malloc(sizeof(struct command_stream));
	  			current_stream ->command = current_command;
	  			current_stream ->next_stream = new_stream;
			
	  		}
	  		if(comments)
	  		{
	  			comments = false;
	  		}
	  	}
	  	else if ( curr== '#' && prev == ' ')
	  	{
	  		comments = true;
	  		//up to but not including the newline
	  	}
	  	else if (curr != ' ')
	  	{
	  		//anything invalid.
	  	}
  	}	
  	prev = curr;
  	
  	curr = read_char (get_next_byte, get_next_byte_argument);
  	// handle about comments after #, need to skip everything
  }
  
  return root;
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
