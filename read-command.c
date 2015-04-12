// UCLA CS 111 LAB1a read-command.c
#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

char read_char (int (*get_next_byte) (void *),
         void *get_next_byte_argument)
{
	return (char) (*get_next_byte)(get_next_byte_argument);
}

bool is_valid(char c) 
{ // check if it is a valid character for simple commands
	// it does not include the eight special token for complete commands
	if (isalnum(c) || c == '!' || c == '%' || c == '+' || c == ',' || c == '-' || c == '.' 
					|| c =='/' || c == ':' || c == '@' || c == '^' || c == '_')
		return true;
	else 
		return false;
}
void copy_word()
{
	
}
command_t make_simple_command (command_t new_command, char* word_buffer[], bool has_input, bool has_output, char* i, char* o, int nWords)
{ 
	// words, input, output
	//read from word_buffer
	//clear buffer
	//command_t* pointer = (command_t*)malloc(sizeof(command_t));
	
	//command_t new_command = (command_t)malloc(sizeof(command_t));
	
	new_command ->type = SIMPLE_COMMAND;
	
	if(has_input)
	{
		new_command -> input = i; 
	}

	if(has_output)
	{
		new_command -> output = o; 
	}

	//current_command -> u.word = word_buffer;
	// for loop?
	//char * words[10]; 
	
	//char **words = (char**) malloc(20 * sizeof(char*));
	//copy_word(words,)
	new_command -> u.word = word_buffer;
	
	return new_command;
}


command_t make_complete_command (char curr, command_t stack)
{ 
	// for complete commands
  	// type
  	// only LHS
  	//curr is the operator
  	command_t new_command = (command_t)malloc(sizeof(command_t));
  	switch(curr)
    {
        case ';':
        	new_command -> type = SEQUENCE_COMMAND;
        	new_command -> u.command[0] = stack;
        		break;
      	case '&':
        	new_command -> type = AND_COMMAND;
        	new_command -> u.command[0] = stack;
        		break;
      	case '|':
        	new_command -> type = PIPE_COMMAND;
        	new_command -> u.command[0] = stack;
        		break;
      	case '(':
        	new_command-> type = SUBSHELL_COMMAND;
        		break;
       	default:
		        break;
    }
  
  return new_command;
}

command_t combine_complete_command (command_t stack, command_t curr_command)
{ 
  	switch (stack->type)
    {
		case SEQUENCE_COMMAND:
		case AND_COMMAND:
		case OR_COMMAND:
		case PIPE_COMMAND:
			stack -> u.command[1] = curr_command; 
				break;
		case SUBSHELL_COMMAND:
			stack -> u.subshell_command = curr_command;
				break;
		default:
				break;
    }
    
 	return stack;
}

//stack for commands
//stack of commands, stack_size starting from 0 to array size-1
//pushing stack 
// when calling push, stack_size should ++
void push (command_t curr_command, command_t* stack, int stack_size)
{
	stack[stack_size] = curr_command;
}

//popping stack
//after calling pop, stack_size should --
void pop (command_t* stack, int stack_size)
{
	if(stack_size != 0) //don't pop an empty stack
      stack[stack_size-1] = NULL; 
}

//need detailed implementation
//need one for END? and NONE?
int  compare_operator (enum command_type current_type, enum command_type stack )
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
	  current_opr = 0;


	if (stack == SEQUENCE_COMMAND)
	  stack_opr = 1;
	else if (stack == AND_COMMAND || stack == OR_COMMAND)
	  stack_opr = 2;
	else if (stack == PIPE_COMMAND)
	  stack_opr = 3;
	else if (stack == SUBSHELL_COMMAND)
	  stack_opr = 0;

	return (current_opr - stack_opr);
}


command_stream_t make_command_stream (int (*get_next_byte) (void *), void *get_next_byte_argument)
{

	//Variables
	bool was_subshell = false; //check if immediately before was an end of subshell 
	bool has_input =false; // current simple commadn has input
	bool has_output = false; //current simple command has output


	int lineNumber = 1; //keeping track of line number for error message
	int subshell_level =0; //level of subshell

	char curr; //current character
	char prev = ' '; //previous character
	char prev_prev =' '; //two characters before curr

	//word holding data structures
	char * word_buffer[10]; //contains the list of words in a line?
	char  word [] = "0123456789"; //contains a word
	int wordsize = 50; //max number of chars in a word
	int nChars = 0; //number of chars
	int nWords = 0; //number of words
	
	word_buffer[9] =NULL;
	word[9] = NULL;
	
	//allocate memory for word_buffer and word
	//word_buffer = (char**)malloc(sizeof(char*)*wordsize);
	//word = (char*) malloc(sizeof(char)*wordsize);
	
	curr = read_char(get_next_byte, get_next_byte_argument);
	if(curr == EOF)
	{
	  	fprintf(stderr, "%d: Nothing in the file \n", lineNumber);
        exit(1);

	}    //FILE IS EMPTY


	char* input = (char*) malloc(wordsize*sizeof(char)); //space for input storing 
														 //needed for make_simple_command
	char* output = (char*) malloc(wordsize*sizeof(char)); //space for output storing

	//command_stream_t root = (command_stream_t)malloc(sizeof(command_stream_t));
//	command_stream_t current_stream = (command_stream_t)malloc(sizeof(command_stream_t));
	
	//root ->next_command_stream = NULL;
	//root -> current_root_command -> type = NULL;
	//current_stream-> next_command_stream = root; 
	
	command_stream_t root =NULL;
	command_stream_t current_stream = (command_stream_t)malloc (sizeof(command_stream_t));
	current_stream -> next_command_stream = root;
	
	enum command_type current_type = SIMPLE_COMMAND; //what is command_type? has it been declared?

	command_t current_command = NULL;

	//stack
	int stack_size = 0;
	int max_stack_size = 501;
	command_t* command_stack = (command_t*)malloc(sizeof(command_t)*max_stack_size);

	if((stack_size +1) == max_stack_size)
	{
		max_stack_size = 2 * max_stack_size;
		command_stack = (command_t*)realloc(command_stack, max_stack_size);
	}

	while (curr != EOF)
	{
		if(is_valid(curr) || curr =='(')
		{
			if(prev_prev =='\n' && prev =='\n')
			{	/*
				//new command stream (new line)
				command_stream_t new_stream = (command_stream_t)malloc(sizeof(command_stream_t));
				current_stream->next_command_stream ->current_root_command = *current_command;
				current_stream->next_command_stream ->next_command_stream = new_stream;
				current_stream->next_command_stream = new_stream;
				*/
				command_stream_t new_stream = (command_stream_t)malloc(sizeof(command_stream_t));
				new_stream ->current_root_command = current_command;
				new_stream -> next_command_stream = NULL;
				if(root == NULL)
				{
					root = new_stream;
				}
				else
				{
					current_stream ->next_command_stream = new_stream;
				}
				current_command = NULL;
				
			}
		}

		if(is_valid (curr))
		{
			word[nChars]=curr;
			printf("8888\n");
			nChars++;  
			printf("nChars = %d",nChars);
			printf("\n");
			
			


		}
		else if( curr ==' ' && is_valid (prev))
		{
			//if white space after valid word
			{
				if(nChars >0)
				{
					//copy word to word_buffer
					char newword[10];
					//strcpy(newword, word);
					word_buffer[nWords] = newword;
					//strcpy (word_buffer[nWords],newword);
					nWords ++;

					while(nChars > 0) //delete word
					{
						newword[nChars-1] = word[nChars-1];
						word[nChars-1] = NULL;
						nChars--;
					}

				}

			}


		}
		
		else if (curr == '\n')
		{
			//newline can not appear after '<' or '>'
			//newline can ONLY appear before '(' or ')'

			lineNumber ++;

			//end of line 
			if( !was_subshell)
			{

				
				//copy word to word_buffer
				if(nChars >0)
				{
					//copy word to word_buffer
					char newword[10];
					//strcpy(newword, word);
					word_buffer[nWords] = newword;
					//strcpy (word_buffer[nWords],newword);
					nWords ++;

					while(nChars > 0) //delete word
					{
						newword[nChars-1] = word[nChars-1];
						word[nChars-1] = NULL;
						nChars--;
					}

				}
				if(nWords >0)
				{
					command_t new_command = (command_t)malloc(sizeof(command_t));
					char **temp = (char**) malloc(20 * sizeof(char*));
					temp = word_buffer;
				current_command = make_simple_command(new_command,temp, has_input,has_output, input, output, nWords);
				has_input = false;
				has_output = false;
				nWords = 0;
				
				}

			}

			if(prev == '\n')
			{
				if(stack_size !=0)
				{
					fprintf(stderr, "%d: No RHS", lineNumber);
					exit(1);
				}
			}

		}
		else if (curr != ' ')
		{
			//anything invalid 
			//it's here just in case.. something is missing
			fprintf(stderr, "%d: Invalid Syntax", lineNumber);
			exit(1);
		}
		

		if( curr != ' ')
		{
			// only store the char that is not a ' ' 
			// white space.. ' ' '\n' and tab? how are we dealing with tab?
			prev_prev = prev;
			prev = curr;
		}
		curr = read_char(get_next_byte, get_next_byte_argument);
	
	}


				if(nChars >0)
				{
					//copy word to word_buffer
					char newword[10];
					//strcpy(newword, word);
					word_buffer[nWords] = newword;
					//strcpy (word_buffer[nWords],newword);
					nWords ++;

					while(nChars > 0) //delete word
					{
						newword[nChars-1] = word[nChars-1];
						word[nChars-1] = NULL;
						nChars--;
					}
				
				}
				
				if(nWords >0)
				{
						printf("8888\n");
						command_t new_command = (command_t)malloc(sizeof(command_t));
						char **temp2 = (char**) malloc(20 * sizeof(char*));
						temp2 = word_buffer;
				current_command = make_simple_command(new_command,temp2, has_input,has_output, input, output, nWords);
				has_input = false;
				has_output = false;
				nWords = 0;
				}



	if(subshell_level != 0)
	{
		// if number of '(' and ')' are different.. at the end
		fprintf(stderr, "%d: Number of '(' and ')' are different", lineNumber);
		exit(1);
	}

	if(stack_size >0)
	{
		fprintf(stderr, "%d: stack is not empty at the end", lineNumber);
		exit(1);  
	}

	if(current_command == NULL)
	{
		fprintf(stderr, "%d:Nothing in the file", lineNumber);
		exit(1);	
	}
	//when curr == 'EOF'
	//when curr == 'EOF'
	//
	//current_stream->next_command_stream -> current_root_command = *current_command;
	if(current_command != NULL)
	{
		/*
		printf("right before end of file\n");
		command_stream_t new_stream = (command_stream_t)malloc(sizeof(command_stream_t));
		new_stream ->current_root_command = *current_command;
		new_stream -> next_command_stream = NULL;
		if(root == NULL)
		{
			root = new_stream;
		}
		else
		{
			current_stream ->next_command_stream = new_stream;
		}
		*/
		root = (command_stream_t)malloc(sizeof(command_stream_t));
		root ->current_root_command = current_command;
		root-> next_command_stream = NULL;
		
	}

	free (command_stack);
	if( root == NULL)
	{
		fprintf (stderr, "root NULL\n");
		exit(1);
	}
	if( current_stream == NULL)
	{
		fprintf (stderr, "current stream NULL \n");
		exit(1);
	}

	free(current_stream);
	return root;

}

command_t read_command_stream (command_stream_t s)
{	
	//command_t output = (command_t)malloc(sizeof(command_t));
	command_t output = s -> current_root_command;
	
	if (s -> next_command_stream == NULL)
	{
		memset(s,0,sizeof(command_stream_t));
	}
	else
	{
	
		memcpy(s,s->next_command_stream,sizeof(command_stream_t));
		//memset(s->next_command_stream,0,sizeof(struct command_stream));
	}
	printf (" read command stream function - \n");
	//free(s);
	return output;
}
