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


command_t make_simple_command (char** word_buffer, command_t current_command, 

bool has_input, bool has_output, char* i, char* o, int nWords)
{ // words, input, output
  //read from word_buffer
  //clear buffer
  if(has_input)
  {
    current_command -> input = i; 
  }

  if(has_output)
  {
   current_command -> output = o; 
  }


  // for loop?
    int c = 0;
    for(; c < nWords; c++)
    {
       current_command -> u.word[c] = word_buffer[c];
    }
}

command_t make_complete_command (char curr, command_t stack)
{ // for complete commands
  // type
  // only LHS
  //curr is the operator
  command_t new_command = (command_t)malloc(sizeof(struct command));
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
      stack[stack_size-1] = NULL; //should it be stack_num-1?
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


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
         void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  
  bool was_subshell = false; //check if immediately before was an end of subshell 
  bool has_input =false; // current simple commadn has input
  bool has_output = false; //current simple command has output

  
  int lineNumber = 0; //keeping track of line number for error message
  int subshell_level =0; //level of subshell
  
  char curr; //current character
  char prev = ' '; //previous character
  char prev_prev =' '; //two characters before curr
  
  //word holding data structures
  char ** word_buffer; //contains the list of words in a line?
  char * word; //contains a word
  int wordsize = 50; //max number of chars in a word
  int nChars = 0; //number of chars
  int nWords = 0; //number of words
  
  //allocate memory for word_buffer and word
  word_buffer = (char**)malloc(sizeof(char*)*wordsize);
  word = (char*) malloc(sizeof(char)*wordsize);
  
  curr = read_char(get_next_byte, get_next_byte_argument);
  if(curr == EOF)
  {
    return 0;
    
  }    //FILE IS EMPTY

  char* input = (char*) malloc(wordsize*sizeof(char)); //space for input storing //needed for make_simple_command
  char* output = (char*) malloc(wordsize*sizeof(char)); //space for output storing

  command_stream_t root = (command_stream_t)malloc(sizeof(struct command_stream));
  command_stream_t current_stream = (command_stream_t)malloc(sizeof(struct command_stream));

  current_stream = root; 
  

//check if the enum is needed
  enum command_type current_type = SIMPLE_COMMAND; //what is command_type? has it been declared?

  command_t current_command = (command_t)malloc(sizeof(command_t));

  //stack
  int stack_size = 0;
  int max_stack_size = 501;
  command_t* command_stack = (command_t*)malloc(sizeof(command_t)*max_stack_size);

  if((stack_size +1) == max_stack_size)
  {
    max_stack_size = 2 * max_stack_size;
    command_stack = (command_t*)realloc(command_stack, max_stack_size);
  }

    

  while(curr != EOF)
  {
    if(is_valid(curr))
    {
      
      
      if(prev_prev =='\n' && prev =='\n')
      {
        //new command stream (new line)
          command_stream_t new_stream = (command_stream_t)malloc(sizeof(struct command_stream));
          current_stream->current_root_command = current_command;
          current_stream->next_command_stream = new_stream;
      }
      if (prev =='&' && is_valid (prev_prev) )
      {
         fprintf(stderr, "%d: invalid operator. Must have two '&'.", lineNumber);
        exit(1); 
      }
      if (is_valid(prev_prev) && prev =='\n')
      {
         // A \n B == A ; B
         
        command_t new_simple_command = (command_t)malloc(sizeof(command_t));
        current_command = make_simple_command(word_buffer, new_simple_command, has_input,has_output, input, output, nWords);
        has_input = false;
        has_output = false;
        current_command = make_complete_command(';', current_command);
        push(current_command, command_stack, stack_size);
        stack_size ++;
      }
      else 
      {
        if(nChars == wordsize) //NEED TO REALLOC!
        {        
          char * temp = (char*)realloc(word, sizeof(char)*wordsize*2);
          word = temp;
          wordsize *= 2;
        }
        word[nChars] = curr;
       nChars++;     
       }
    }
    else if( curr ==' ' && is_valid (prev))
    {
      if( has_input )
      {
        if(nChars > wordsize)
        {
          input = (char*) realloc(input,wordsize);
          //
        }
        strcpy(input,word);
        while(nChars >= 0) //delete word or set everything to ''
        {
          word[nChars] = '0';
          nChars--;
        }
      }
      else if (has_output)
      {
        if(nChars > wordsize)
        {
          output = (char*) realloc(output,wordsize);
          //
        }
        strcpy(output, word);
        while(nChars >= 0) //delete word
        {
          word[nChars] = '0';
          nChars--;
        }
      }
      else
      {
         //copy word to word_buffer
         int i = 0;
         i = nChars;
         char* newword = (char*) malloc (sizeof(char)*nChars); //word length of that copied word...???
         strcpy(newword,word);
         
          while(i >= 0) //delete word
         {
          word[i] = '0';
          i--;
         }
         
         for( i = 0; i < nChars; i++) //copy word into word_buffer: word_buffer
         {
           if(nWords == wordsize) //REALLOCATE!
           {
             char** word_buffer = (char**)realloc(word_buffer, wordsize*2);
             wordsize *= 2;
           }
           word_buffer[nWords] = &newword[i];
           nWords++;
         }
         
    
      }
    }
    else if (curr== ';' ||  curr == '&' ||curr == '|' ||curr ==')')
    {
      // operator
      if( (curr ==';' && prev ==';') || (curr =='&' && (prev !='&' || prev_prev =='&'))
                                     || (curr =='|' && (prev =='|' || prev_prev =='|'))) 
      {
        // ;; , &&&, |||

        fprintf(stderr, "%d: invalid operator", lineNumber);
        exit(1);
      }
      if(prev == '\n')
      {
        fprintf(stderr, "%d: Cannot start a new line with operator", lineNumber);
        exit(1);
      }
      else if(curr == ';')
      {
        current_type = SEQUENCE_COMMAND;
        if(word_buffer == NULL && !was_subshell)
        {
          fprintf(stderr, "%d: No LHS", lineNumber);
          exit(1);
        }
      }
      else if(curr == '&' && prev == '&')
      {
        current_type = AND_COMMAND;
        if(nWords == 0 && !was_subshell)
        {
          fprintf(stderr, "%d: No LHS", lineNumber);
          exit(1);
        }
      }
      else if(curr == '|' && prev != '|')
      {
        current_type = PIPE_COMMAND;
        if(nWords == 0  && !was_subshell)
        {
          fprintf(stderr, "%d: No LHS", lineNumber);
          exit(1);
        }
      }
      else if(curr == '|' && prev == '|')
      {
        current_type = OR_COMMAND;
        if(nWords == 0 && (!was_subshell))
        {
          fprintf(stderr, "%d: No LHS", lineNumber);
          exit(1);
        }
      }
      else if (curr ==')')
      {
        current_type = SUBSHELL_COMMAND;

        if(nWords == 0  && !was_subshell)
        {
          fprintf(stderr, "%d: No LHS", lineNumber);
          exit(1);
        }
      } 

      if( was_subshell)
      {
        current_command = command_stack [stack_size-1];
        pop(command_stack, stack_size);
        stack_size --;
        was_subshell= false;
      }
      else
      {
        // make simple command when you reached  '&&' '|' '||' ')'
        // add everything in word buffer into simple command
        //if there's input and output assign them too.

        command_t new_simple_command = (command_t)malloc(sizeof(command_t));
        current_command = make_simple_command(word_buffer, new_simple_command, has_input,has_output, input, output, nWords);
        has_input = false;
        has_output = false;
        // shoudl reset input, output to NULL here or inside make_simple_command
      }
      if(stack_size > 0)
      {
        while(stack_size >0 && (compare_operator(current_type, command_stack[stack_size-1]->type) <= 0))
        {
            current_command = combine_complete_command(command_stack[stack_size-1], current_command);
            pop(command_stack, stack_size);
            stack_size--;
        }

        if( curr ==')')
        {
          current_command = combine_complete_command(curr, command_stack[stack_size-1]);
          push(current_command, command_stack, stack_size-1);
          subshell_level --;
          was_subshell =true;
        }
        else 
        {
          //if it's end of line, you won't need to start a new command
          current_command = make_complete_command(curr, command_stack[stack_size-1]);
          push(current_command, command_stack, stack_size-1);
         //this push overwrites an entry; does not increase stack_size
        }
      
      }
      else
      {
        current_command = make_complete_command(curr, current_command);
        push(current_command, command_stack, stack_size);
        stack_size++; 
      }

    } 
    else if(curr == '(')
    {
      if( curr =='(' && prev =='(') 
      {
        // '(('
        fprintf(stderr, "%d: invalid operator, No '((' allowed ", lineNumber);
        exit(1);
      }
      subshell_level ++;
      current_command = make_complete_command(curr, current_command);
      push (current_command, command_stack, stack_size);
      stack_size ++;
    }
      
    else if(curr == '<')
    {
      if(prev == '\n')
      {
        fprintf(stderr, "%d: Cannot start a new line '<'", lineNumber);
        exit(1);
      }
      if(!is_valid(prev))
      {
        fprintf(stderr, "%d: '<', input must have a output", lineNumber);
        exit(1);
      }
      has_input = true;
    }
    else if (curr =='>')
    {
      if(prev == '\n')
      {
        fprintf(stderr, "%d: Cannot start a new line '>'", lineNumber);
        exit(1);
      }
      if(!is_valid(prev))
      {
        fprintf(stderr, "%d: '>', output must have a input", lineNumber);
        exit(1);
      }
      has_output =true;
    }

    else if(curr == '\n')
    {
      //one newline is treated as a ';'
      //newline can not appear after '<' or '>'
      //newline can ONLY appear before '(' or ')'

      lineNumber ++;
      
      //end of line 
      if( !was_subshell)
      {
        command_t new_simple_command = (command_t)malloc(sizeof(struct command));
        current_command = make_simple_command(word_buffer, new_simple_command, has_input,has_output, input, output, nWords);
        has_input = false;
        has_output = false;

        while (stack_size >0)
        {
          current_command = combine_complete_command(command_stack[stack_size-1], current_command);
          pop(command_stack, stack_size);
          stack_size--;
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
else if(curr == '#' && (prev == ' ' || prev == '\n' || prev =='\t')) 
    {//check if prev_prev == ' ' or '\n' or '\t'
      //ignore up to but not including newline or EOF
      while(curr != '\n' || curr  != EOF)
      {
        curr = read_char(get_next_byte, get_next_byte_argument);
      }
      
    }
    else if(curr != ' ')
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

  if(subshell_level != 0)
  {
     // if number of '(' and ')' are different.. at the end
    fprintf(stderr, "Number of '(' and ')' are different");
    exit(1);
  }

  //when curr == 'EOF'
  current_stream->current_root_command = current_command;

  //if you free it how do you return it????
  //are we really need to free it? 
  // shouldn't some of them freed after read_command_stream read it?
  
  /*free(current_stream);
  free(current_command);
  free(command_stack);
  free(new_stream); */


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

  free(s);
  return output;
}
