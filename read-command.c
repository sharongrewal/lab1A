// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

//read_char retrieves the next char
char read_char (int (*get_next_byte) (void *), void *get_next_byte_argument)
{
  return (char) (*get_next_byte)(get_next_byte_argument);
}

//is_valid checks if simple command
bool is_valid(char c)
{ // check if it is a valid character for simple commands
  // it does not include the eight special token for complete commands
  if (isalnum(c) || c == '!' || c == '%' || c == '+' || c == ',' || c == '-' ||\
 c == '.'
      || c =='/' || c == ':' || c == '@' || c == '^' || c == '_')
    return true;
  else
    return false;
}

//checks if valid operator
bool is_valid_op(char c)
{
  if(c == ';' || c == '|' || c == '&' || c == '(' || c == ')' || c == '>'
    ||  c == '<')
    return true;
  else
    return false;

}

command_stream_t make_command_stream(int (*get_next_byte)(void*), void *get_next_byte_argument)
{

	int i = 0; //basic iterator for going through for loops
	int lineNumber = 1; //lineNumber for error messages, lines start on 1 (not 0)
	
	char curr; //most recently retrieved char
	char prev = ' '; //previous char
	char prev_prev = ' '; //two chars before curr
	
	//"word" holding data structures
	char * word;
	int wordsize = 500;
	int nChars = 0; //keeps track of number of chars within word
	
	//allocate memory for word
	word = (char*)malloc(sizeof(char)*wordsize);
	if(word == NULL) //check if malloc failed
	{
		fprintf(stderr, "MALLOC FAILED");
		exit(1);
	}
	
	curr = read_char(get_next_byte, get_next_byte_argument);
	if(curr == 0)
	{
	   exit(1); //do we exit on an empty file?	
	}
	
	while(curr != EOF)
	{
		if(curr == '\n')
		{
			lineNumber++;
		}
		//check for invalid syntax
		if(curr != prev) 
		{
			if(is_valid_op(curr) && is_valid_op(prev))
			{ //operators next to each other
			 //i.e. (& or >|
			 fprintf(stderr, "%d: Invalid syntax.", lineNumber);
			 exit(1);
			}
		}
		
		if(is_valid_op(curr) && prev == curr && prev_prev == curr)
		{ //more than one operator in a row
		  //i.e. ((( or &&&
			fprintf(stderr, "%d: Invalid syntax.", lineNumber);
			exit(1);
		}
		
		if(is_valid_op(curr) && prev == '\n')
		{ //newline may not preced an operator except for (, )
			if(curr != '(' && curr != ')')
			{
				fprintf(stderr, "%d: Invalid syntax.", lineNumber);
				exit(1);
			}
		}
		
		//comments
		if(curr == '#' && (prev == ' ' || prev == '\n' || prev == '\t'))
		{
			while(curr != '\n')
			{
				if(curr == EOF)
					break;
				curr = read_char(get_next_byte, get_next_byte_argument);
			}
			curr = read_char(get_next_byte, get_next_byte_argument);
		}
		
		//retrieves simple commands and stores them into word
		if(is_valid(curr))
		{
			if(nChars == wordsize) //must reallocate
			{
			 char * temp = (char*)realloc(word, wordsize + sizeof(char)*wordsize*2);
			 if(temp == NULL)
			 {
			 	fprintf(stderr, "REALLOC FAILED");
			 	exit(1);
			 }
			 word = temp;
			 wordsize *= 2;
			}
			word[nChars] = curr;
			nChars++;
		}
		
		//checks for newline ==> sequence command case
		if(prev == '\n' && is_valid(curr) && is_valid(prev_prev))
		{
			//SEQUENCE_COMMAND
		}
		
		if(curr == '(')
		{
			//SUBSHELL_COMMAND
		}
		
	
		if(prev == '|' && prev_prev != prev && curr != prev)
		{
			//PIPE_COMMAND
		
		}
	
		if(curr == '&' && prev == curr)
		{
			//AND_COMMAND
		
		}
		
		if(curr == '|' && prev == '|')
		{
			//OR_COMMAND
		
		}
		prev_prev = prev;
		prev = curr;
		curr = read_char(get_next_byte, get_next_byte_argument);
	}
	
	return 0;
}

command_t read_command_stream(command_stream_t s)
{
	return 0;
}
	
	
	
	
	
	

