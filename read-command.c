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

command_t make_simple_command (command_t new_command, char** words,  char* i, char* o)
{ 
	new_command ->type = SIMPLE_COMMAND;

	if(i != NULL)
	{
		new_command -> input = i; 
	}

	if(o != NULL)
	{
		new_command -> output = o; 
	}


	new_command->u.word = words;

	return new_command;
}


command_t make_complete_command (command_t new_command,char curr, command_t stack)
{ 
	// for complete commands
  	// type
  	// only LHS
  	//curr is the operator
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
        	new_command -> type = OR_COMMAND;
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
void push (command_t  curr_command, command_t* stack, int stack_size)
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
	char *word;//contains a word
	int wordsize = 50; //max number of chars in a word
	int maxwords = 50;
	int nChars = 0; //number of chars
	int nWords = 0; //number of words

	//allocate memory for word_buffer and word
	//word_buffer = (char**)malloc(sizeof(char*)*wordsize);
	//word = (char*) malloc(sizeof(char)*wordsize);
	word_buffer[9] =NULL;
	word = (char*) malloc (sizeof(char)*wordsize);
	
	curr = read_char(get_next_byte, get_next_byte_argument);
	printf("%d: read first curr: %c\n",__LINE__,curr);
	if(curr == EOF)
	{
	  	fprintf(stderr, "%d: Nothing in the file \n", lineNumber);
        	exit(1);
	}    //FILE IS EMPTY


	char* input;//space for input storing 
	char* output; //space for output storing
	input = NULL;
	output = NULL;
	
	command_stream_t head = NULL;
	command_stream_t current_stream = NULL; 

	
	enum command_type current_type = SIMPLE_COMMAND; //what is command_type? has it been declared?

	//struct command * current_command = (struct command *) malloc (sizeof(struct command));
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
		if (curr == '#' && (prev == ' ' || prev == '\n' || prev =='\t'))
		{
			while(curr != '\n' && curr  != EOF)
			{
				curr = read_char(get_next_byte, get_next_byte_argument);
			}
			printf("%d: just got out of the loop for comments, curr: %c, prev:%c, prev_prev:%c\n",__LINE__, curr, prev, prev_prev);;
		
		}
			
		if(is_valid (curr))
		{ 
			if(prev_prev =='\n' && prev =='\n')
			{	printf("%d: you met a valid curr after two consecutive new line, curr:%c \n",__LINE__, curr);
		
				//new command stream (new line)
				if(current_command != NULL)
				{printf("%d: current_command not null, need to put that in a stream\n",__LINE__);
					if(stack_size ==0)
					{printf("%d: stack size is 0, should make a new command_Stream\n",__LINE__);
			
						command_stream_t new_stream = (command_stream_t)malloc(sizeof(struct command_stream));
						new_stream->current_root_command = current_command;
						new_stream ->next_command_stream = NULL;
						
						if(head == NULL)
						{printf("%d: head was null, making first command stream\n",__LINE__);
			
							head = new_stream;
							current_stream = new_stream;
						
						}
						else
						{printf("%d:head was not null, making more than one commadn stream\n",__LINE__);
			
							current_stream ->next_command_stream = new_stream;
							current_stream = new_stream;
							
						}
						current_command = NULL;
					}
				}
			}

			
			if (prev =='&' && is_valid (prev_prev) )
			{
				//When you have only one '&'
				fprintf(stderr, "%d: invalid operator. Must have two '&'.\n", lineNumber);
				exit(1); 
			}

			if (is_valid(prev_prev) && prev =='\n')
			{printf("%d: you met the CASE A '\n' B, changing to sequence command\n",__LINE__);
			printf("%d: making a complete command\n",__LINE__);
		
				// A \n B == A ; B
				// A must be already in simple_command when the program reached '\n'
				current_type = SEQUENCE_COMMAND;
				command_t new_command = (command_t)malloc(sizeof(command_t));
				command_t temp =  make_complete_command(new_command,';', current_command);
				push(temp, command_stack, stack_size);
				stack_size ++;
				current_command = command_stack [stack_size-1]; //contain subshell command
				pop(command_stack, stack_size);
				stack_size --;

				if( was_subshell && stack_size >0)
				{
					was_subshell= false;
				}

				if(stack_size > 0)
				{
					//combining LHS!!!
						
					while(stack_size >0 && (compare_operator(current_type, command_stack[stack_size-1]->type) <= 0))
					{
						command_t temp = combine_complete_command(command_stack[stack_size-1], current_command);
						pop(command_stack, stack_size);
						stack_size--;
						push(temp, command_stack, stack_size);
						stack_size ++;
						current_command = command_stack [stack_size-1]; //contain subshell command
						pop(command_stack, stack_size);
						stack_size --;
					}

					if( curr ==')')
					{	command_t temp  = combine_complete_command(current_command, command_stack[stack_size-1]);
						push(temp, command_stack, stack_size-1);
						subshell_level --;
						was_subshell =true;
					}
					else 
					{printf("%d: making a complete command\n",__LINE__);
						//if it's end of line, you won't need to start a new command
						command_t new_command = (command_t)malloc(sizeof(command_t));
						command_t temp= make_complete_command(new_command,curr, command_stack[stack_size-1]);
						push(temp, command_stack, stack_size-1);
						//this push overwrites an entry; does not increase stack_size
					}

				}
				else
				{printf("%d: stack >0, making a complete command\n",__LINE__);
					//don't need to combine, just put the simple command into a complete command
					command_t new_command = (command_t)malloc(sizeof(command_t));
					command_t temp = make_complete_command(new_command,curr, current_command);
					push(temp, command_stack, stack_size);
					stack_size++; 
				}
			}
			else if(is_valid(prev_prev) && prev =='|')
			{printf("%d: it's pipe command, make a new pipe complete command and put it in stack\n",__LINE__);
			// did you make a simple command yet??
				printf("%d: curr :%c\n",__LINE__,curr);
				// A \n B == A ; B
				// A must be already in simple_command when the program reached '\n'
				current_type = PIPE_COMMAND;
				command_t new_command = (command_t)malloc(sizeof(command_t));
				command_t temp = make_complete_command(new_command,'|', current_command);
				push(temp, command_stack, stack_size);
				stack_size ++;

				current_command = command_stack [stack_size-1]; //contain subshell command
				pop(command_stack, stack_size);
				stack_size --;

				if( was_subshell && stack_size >0)
				{
					was_subshell= false;
				}

				if(stack_size > 0)
				{
					//combining LHS!!!
						
					while(stack_size >0 && (compare_operator(current_type, command_stack[stack_size-1]->type) <= 0))
					{	
						command_t temp  = combine_complete_command(command_stack[stack_size-1], current_command);
						pop(command_stack, stack_size);
						stack_size--;
						push(temp, command_stack, stack_size);
						stack_size ++;
						current_command = command_stack [stack_size-1]; //contain subshell command
						pop(command_stack, stack_size);
						stack_size --;
					}

					if( curr ==')')
					{
						command_t temp = combine_complete_command(current_command, command_stack[stack_size-1]);
						push(temp, command_stack, stack_size-1);
						subshell_level --;
						was_subshell =true;
					}
					else 
					{printf("%d: making a complete command\n",__LINE__);
						//if it's end of line, you won't need to start a new command
						command_t new_command = (command_t)malloc(sizeof(command_t));
						command_t temp= make_complete_command(new_command,curr, command_stack[stack_size-1]);
						push(temp, command_stack, stack_size-1);
						//this push overwrites an entry; does not increase stack_size
					}

				}
				else
				{printf("%d:stacksize>0, making a complete command\n",__LINE__);
					//don't need to combine, just put the simple command into a complete command
					command_t new_command = (command_t)malloc(sizeof(command_t));
					command_t temp= make_complete_command(new_command,curr, current_command);
					push(temp, command_stack, stack_size);
					stack_size++; 
				}
			}

			word[nChars] = curr;

			nChars++;
			printf("%d: curr :%c, char: %d\n",__LINE__,curr,nChars);

		}
		else if( curr ==' ' && (is_valid (prev) || prev ==')'))
		{printf("%d: curr :%c, met a space and it was valid before, so put owrd in word_buffer \n",__LINE__,curr);
			//if white space after valid word

			if( has_input )
			{printf("%d: has input\n",__LINE__);
				if(nChars > wordsize)
				{
					wordsize = nChars;
					input = (char*) realloc(input,wordsize);
				}

				input = (char*) malloc(wordsize*sizeof(char));
				 strcpy(input, word);

				while(nChars > 0) //delete word or set everything to ''
				{
					word[nChars-1] = '\0';
					nChars--;
				}
				has_input = false;
			}
			else if (has_output)
			{printf("%d: has output\n",__LINE__);
				if(nChars > wordsize)
				{
					wordsize = nChars;
					output = (char*) realloc(output,wordsize);

				}
				
				 output = (char*) malloc(wordsize*sizeof(char));
				 strcpy(output, word);
				
				
				while(nChars > 0) //delete word
				{
					word[nChars-1] = '\0';
					nChars--;
				}
				has_output = false;
			}
			else
			{printf("%d: curr :%c, copying to word buffer\n",__LINE__,curr);
				if(nChars >0)
				{
				
					char* newword = (char*)malloc(20*sizeof(char));

					while(nChars > 0) //delete word
					{
						newword[nChars-1]  = word[nChars -1];
						word[nChars-1] = '\0';
						nChars--;
					}

					word_buffer[nWords] = newword;
					nWords ++;
					printf("%d: word_buffer :%s, nWords :%d\n",__LINE__,word_buffer[nWords-1],nWords);
				}

			}


		}
		else if( curr== ';' ||  curr == '&' ||curr == '|' ||curr ==')')
		{
			if( (curr ==';' && prev ==';') || (curr =='&' && prev =='&' && prev_prev =='&')
			                 || (curr =='|' && prev =='|' && prev_prev =='|')) 
			{
				// ;; , &&&, |||, invalid operators

				fprintf(stderr, "%d: invalid operator\n", lineNumber);
				exit(1);
			}
			if(prev == '\n')
			{
				//when newline starts with operators
				fprintf(stderr, "%d: Cannot start a new line with operator\n", lineNumber);
				exit(1);
			}
			if(( curr == ';') || (curr == '&' && prev == '&')||(curr == '|' && prev == '|')||(curr ==')'))
			{
				printf("%d: you met operator, need to make copmlete command\n",__LINE__);
		
				if(curr == ';')
				{
					current_type = SEQUENCE_COMMAND;
					if(nWords == 0 && !was_subshell)
					{
						fprintf(stderr, "%d: No LHS\n", lineNumber);
						exit(1);
					}
				}
				else if(curr == '&' && prev == '&')
				{printf("%d: you met AND COMMAND, nWords = %d\n",__LINE__,nWords);
		
					current_type = AND_COMMAND;
					if(nWords == 0 && !was_subshell)
					{
						fprintf(stderr, "%d: No LHS\n", lineNumber);
						exit(1);
					}
				}
				else if(curr == '|' && prev == '|')
				{
					current_type = OR_COMMAND;
					if(nWords == 0 && !was_subshell)
					{
						fprintf(stderr, "%d: No LHS\n", lineNumber);
						exit(1);
					}
				}
				else if (curr ==')')
				{
					current_type = SUBSHELL_COMMAND;
	
					if(nWords == 0  && !was_subshell)
					{
						fprintf(stderr, "%d: No LHS\n", lineNumber);
						exit(1);
					}
				} 
	
				if( was_subshell && stack_size >0)
				{
					//if just now was a sibshell. prev ==')'
					current_command = command_stack [stack_size-1]; //contain subshell command
					pop(command_stack, stack_size);
					stack_size --;
					was_subshell= false;
				}
				else
				{
					// make simple command when you reached  '&&' '|' '||' ')'
					// add everything in word buffer into simple command
					//if there's input and output assign them too.
					//copy last word to word_buffer
	
					if( has_input )
					{
						if(nChars > wordsize)
						{
							wordsize = nChars;
							input = (char*) realloc(input,wordsize);
						}
		
						input = (char*) malloc(wordsize*sizeof(char));
						 strcpy(input, word);
		
						while(nChars > 0) //delete word or set everything to ''
						{
							word[nChars-1] = '\0';
							nChars--;
						}
						has_input = false;
					}
					else if (has_output)
					{
						if(nChars > wordsize)
						{
							wordsize = nChars;
							output = (char*) realloc(output,wordsize);
							//
						}
						
						output = (char*) malloc(wordsize*sizeof(char));
						strcpy(output, word);
						
						while(nChars > 0) //delete word
						{
							word[nChars-1] = '\0';
							nChars--;
						}
						has_output = false;
					}
					else
					{
						if(nChars >0)
						{
							//copy word to word_buffer
		
							//strcpy(word_buffer[nWords], word);
							//strcpy (word_buffer[nWords],newword);
							char* newword = (char*)malloc(20*sizeof(char));
		
							while(nChars > 0) //delete word
							{
								newword[nChars-1]  = word[nChars -1];
								word[nChars-1] = '\0';
								nChars--;
							}
		
							word_buffer[nWords] = newword;
							nWords ++;
						}
		
					}
		
					if(nWords >0)
					{printf("%d: nWords: %d, making a simple command\n",__LINE__, nWords);
						command_t new_command = (command_t)malloc(sizeof(command_t));
						char **words = (char**) malloc (maxwords * sizeof(char*));
						int k;
						for(k =0; k<nWords;k++)
						{
							words[k] = word_buffer[k];
							word_buffer[k] = NULL;
						}
						current_command= make_simple_command(new_command,words, input, output);
						input = NULL;
						output = NULL;
						nWords =0;
						//word_buffer = NULL:
						// shoudl reset input, output to NULL here or inside make_simple_command
					}
					
				}
	
	
	
			}
			
			if(stack_size > 0)
			{
				//combining LHS!!!
					
				while(stack_size >0 && (compare_operator(current_type, command_stack[stack_size-1]->type) <= 0))
				{
					command_t temp = combine_complete_command(command_stack[stack_size-1], current_command);
					pop(command_stack, stack_size);
					stack_size--;
					push(temp, command_stack, stack_size);
					stack_size ++;
					current_command = command_stack [stack_size-1]; //contain subshell command
					pop(command_stack, stack_size);
					stack_size --;
				}

				if( curr ==')')
				{
					command_t temp  = combine_complete_command(current_command, command_stack[stack_size-1]);
					push(temp, command_stack, stack_size-1);
					subshell_level --;
					was_subshell =true;
				}
				else 
				{printf("%d: making a complete command\n",__LINE__);
					//if it's end of line, you won't need to start a new command
					command_t new_command = (command_t)malloc(sizeof(command_t));
					command_t temp = make_complete_command(new_command,curr, command_stack[stack_size-1]);
					push(temp, command_stack, stack_size-1);
					//this push overwrites an entry; does not increase stack_size
				}

			}
			else
			{printf("%d: stack size>0,making a complete command\n",__LINE__);
				//don't need to combine, just put the simple command into a complete command
				command_t new_command = (command_t)malloc(sizeof(command_t));
				command_t temp = make_complete_command(new_command,curr, current_command);
				push(temp, command_stack, stack_size);
				stack_size++; 
			}
		}
		else if(curr == '(')
		{ printf("%d: making a complete command for subshell\n",__LINE__);
			if( curr =='(' && prev =='(') 
			{
				// '(('
				fprintf(stderr, "%d: invalid operator, No '((' allowed \n", lineNumber);
				exit(1);
			}
			subshell_level ++;
			command_t new_command = (command_t)malloc(sizeof(command_t));
			command_t temp  = make_complete_command(new_command,curr, current_command);
			push (temp, command_stack, stack_size);
			stack_size ++;

		}
		else if (curr =='<')
		{printf("%d: curr :%c\n",__LINE__,curr);
			if(prev == '\n')
			{
				fprintf(stderr, "%d: Cannot start a new line '<'\n", lineNumber);
				exit(1);
			}
			if(!is_valid(prev))
			{
				fprintf(stderr, "%d: '<', input must have a output\n", lineNumber);
				exit(1);
			}
			if(input != NULL)
			{
				fprintf(stderr, "%d: cannot have more than one '<' in a simple command\n", lineNumber);
				exit(1);
			}
			has_input = true;
			if (has_output)
			{
				if(nChars > wordsize)
				{
					wordsize = nChars;
					output = (char*) realloc(output,wordsize);
					//
				}
				
				output = (char*) malloc(wordsize*sizeof(char));
				strcpy(output, word);
				
				while(nChars > 0) //delete word
				{
					word[nChars-1] = '\0';
					nChars--;
				}
				has_output = false;
			}
			else
			{
				if(nChars >0)
				{
					//copy word to word_buffer

					//strcpy(word_buffer[nWords], word);
					//strcpy (word_buffer[nWords],newword);
					char* newword = (char*)malloc(20*sizeof(char));

					while(nChars > 0) //delete word
					{
						newword[nChars-1]  = word[nChars -1];
						word[nChars-1] = '\0';
						nChars--;
					}

					word_buffer[nWords] = newword;
					nWords ++;
				}

			}
		}
		else if (curr == '>')
		{printf("%d: curr :%c\n",__LINE__,curr);
			if(prev == '\n')
			{
				fprintf(stderr, "%d: Cannot start a new line '>'\n", lineNumber);
				exit(1);
			}
			if(!is_valid(prev))
			{
				fprintf(stderr, "%d: '>', input must have a output\n", lineNumber);
				exit(1);
			}
			if(output != NULL)
			{
				fprintf(stderr, "%d: cannot have more than one '>' in a simple command\n", lineNumber);
				exit(1);
			}
			has_output = true;

			if( has_input )
			{
				if(nChars > wordsize)
				{
					wordsize = nChars;
					input = (char*) realloc(input,wordsize);
				}

				input = (char*) malloc(wordsize*sizeof(char));
				 strcpy(input, word);

				while(nChars > 0) //delete word or set everything to ''
				{
					word[nChars-1] = '\0';
					nChars--;
				}
				has_input = false;
			}
			else
			{
				if(nChars >0)
				{
					//copy word to word_buffer

					//strcpy(word_buffer[nWords], word);
					//strcpy (word_buffer[nWords],newword);
					char* newword = (char*)malloc(20*sizeof(char));

					while(nChars > 0) //delete word
					{
						newword[nChars-1]  = word[nChars -1];
						word[nChars-1] = '\0';
						nChars--;
					}

					word_buffer[nWords] = newword;
					nWords ++;
				}

			}

		}

		else if (curr == '\n')
		{printf("%d: curr :%c\n",__LINE__,curr);
			//newline can not appear after '<' or '>'
			//newline can ONLY appear before '(' or ')'

			if(prev == '<' ||prev =='>')
			{
				fprintf(stderr, "%d: newline after '<' or '>'\n", lineNumber);
				exit(1);
			}

			lineNumber ++;

			//end of line 
			if( !was_subshell)
			{
			printf("%d: curr :%c\n",__LINE__,curr);
			printf("%d: nwords :%d\n",__LINE__,nWords);
				if( has_input )
				{
					if(nChars > wordsize)
					{
						wordsize = nChars;
						input = (char*) realloc(input,wordsize);
					}
				
					input = (char*) malloc(wordsize*sizeof(char));
					
					strcpy(input, word);
				
					while(nChars > 0) //delete word or set everything to ''
					{
						word[nChars-1] = '\0';
						nChars--;
					}
					has_input = false;
			
				}
				else if (has_output)
				{
					if(nChars > wordsize)
					{
						wordsize = nChars;
						output = (char*) realloc(output,wordsize);
						//
					}
					
					output = (char*) malloc(wordsize*sizeof(char));
					strcpy(output, word);
					
					while(nChars > 0) //delete word
					{
						word[nChars-1] = '\0';
						nChars--;
					}
					has_output = false;
				}
				else
				{printf("%d: nChar :%d\n",__LINE__,nChars);
					if(nChars >0)
					{printf("%d: curr :%c\n",__LINE__,curr);
						//copy word to word_buffer
				
						//strcpy(word_buffer[nWords], word);
						//strcpy (word_buffer[nWords],newword);
						char* newword = (char*)malloc(20*sizeof(char));
				
						while(nChars > 0) //delete word
						{printf("%d: curr :%c\n",__LINE__,curr);
							newword[nChars-1]  = word[nChars -1];
							word[nChars-1] = '\0';
							nChars--;
						}
				
						word_buffer[nWords] = newword;
						nWords ++;
						printf("%d: curr :%d\n",__LINE__,nWords);
					}
				
				}
				

				if(nWords >0 )
				{printf("%d: nWords: %d, making a simple command\n",__LINE__, nWords);
					command_t new_command = (command_t)malloc(sizeof(command_t));
					char **words = (char**) malloc (maxwords * sizeof(char*));
					int k;
					for(k =0; k<nWords;k++)
					{
						words[k] = word_buffer[k];
						word_buffer[k] = NULL;
					}
					current_command = make_simple_command(new_command,words, input, output);
					
					input = NULL;
					output = NULL;
					nWords =0;
					//word_buffer = NULL:
					// shoudl reset input, output to NULL here or inside make_simple_command
				}

				while (stack_size >0)
				{printf("%d: curr :%c\n",__LINE__,curr);
					command_t temp  = combine_complete_command(command_stack[stack_size-1], current_command);
					pop(command_stack, stack_size);
					stack_size--;
					push(temp, command_stack, stack_size);
					stack_size ++;
					current_command = command_stack [stack_size-1]; //contain subshell command
					pop(command_stack, stack_size);
					stack_size --;
				}
			
			}


		}
		else if (curr != ' ')
		{
			//anything invalid 
			//it's here just in case.. something is missing
			fprintf(stderr, "%d: Invalid Syntax\n", lineNumber);
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
	
	printf("%d: met EOF\n",__LINE__);
	if( has_input )
	{
	printf("%d: met EOF, has input\n",__LINE__);

		if(nChars > wordsize)
		{
			wordsize = nChars;
			input = (char*) realloc(input,wordsize);
		}
	
		input = (char*) malloc(wordsize*sizeof(char));
		
		strcpy(input, word);
	
		while(nChars > 0) //delete word or set everything to ''
		{
			word[nChars-1] = '\0';
			nChars--;
		}
		has_input = false;

	}
	else if (has_output)
	{printf("%d: met EOF, has output",__LINE__);
		if(nChars > wordsize)
		{
			wordsize = nChars;
			output = (char*) realloc(output,wordsize);
			//
		}
		
		output = (char*) malloc(wordsize*sizeof(char));
		strcpy(output, word);
		
		while(nChars > 0) //delete word
		{
			word[nChars-1] = '\0';
			nChars--;
		}
		has_output = false;
	}
	else
	{printf("%d: met EOF, need to make last simple command \n",__LINE__);
		if(nChars >0)
		{printf("%d: nChars:%d, you still have characters in word_\n",__LINE__,nChars);
			//copy word to word_buffer
	
			//strcpy(word_buffer[nWords], word);
			//strcpy (word_buffer[nWords],newword);
			char* newword = (char*)malloc(20*sizeof(char));
	
			while(nChars > 0) //delete word
			{
				newword[nChars-1]  = word[nChars -1];
				word[nChars-1] = '\0';
				printf("%d:copying from word : %c\n",__LINE__,newword[nChars-1]);
				nChars--;
			}
	
			word_buffer[nWords] = newword;
			nWords ++;
			printf("%d: word copied: nWords :%d\n",__LINE__,nWords);
		}
	
	}
	
				

	if(nWords >0)
	{	printf("%d: you have %d words in word_buffer left. Need to make simple command\n",__LINE__,nWords);
		command_t new_command = (command_t)malloc(sizeof(command_t));

		char **words = (char**) malloc(maxwords * sizeof(char*));
		int k;
		for(k =0; k<nWords;k++)
		{
			words[k]= word_buffer[k];
			
		}
		current_command = make_simple_command(new_command,words,  input, output);
		if(new_command -> type == SIMPLE_COMMAND)
		{
			printf("%d: just made simple command, new commadn type is simple\n",__LINE__);
		}
		if(current_command -> type == SIMPLE_COMMAND)
		{
			printf("%d: just made simple command, current commadn type is simple\n",__LINE__);
		}
		input = NULL;
		output = NULL;
		nWords = 0;
	}

	//at the EOF

	if(subshell_level != 0)
	{
		// if number of '(' and ')' are different.. at the end
		fprintf(stderr, "%d: Number of '(' and ')' are different\n", lineNumber);
		exit(1);
	}

	if(stack_size >0)
	{
		fprintf(stderr, "%d: dangling commands,,,stack is not empty at the end\n", lineNumber);
		exit(1);  
	}
	if(current_command == NULL)
	{
		fprintf(stderr, "%d:Nothing in the file\n", lineNumber);
		exit(1);	
	}

	//when curr == 'EOF'
	if(current_command != NULL)
	{
		printf("%d: You still have current_command left. Thsi current_command is the root command \n",__LINE__);
		
		//new command stream (new line)
		command_stream_t new_stream = (command_stream_t)malloc(sizeof(struct command_stream));
		new_stream->current_root_command =current_command;
		new_stream ->next_command_stream = NULL;
	
		if(head == NULL)
		{	printf("%d: head NULL, you are making the first command stream\n",__LINE__);
			head = new_stream;
			current_stream = new_stream;

		}
		else
		{	printf("%d: head not NULL, you are making more than one command stream\n",__LINE__);
			
			current_stream ->next_command_stream = new_stream;
			current_stream = new_stream;

		}
		current_command= NULL;
	}


	if( head == NULL)
	{
		fprintf (stderr, "head NULL\n");
		exit(1);
	}
	
	if( current_stream == NULL)
	{
		fprintf (stderr, "current stream NULL \n");
		exit(1);
	}

	return head;

}

command_t read_command_stream (command_stream_t s)
{	
	command_t output = s -> current_root_command;
	if (s -> next_command_stream == NULL)
		memset(s,0,sizeof(struct command_stream));
	else
		memcpy(s,s->next_command_stream,sizeof(struct command_stream));
	return output;
}

