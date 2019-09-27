#include "interpreter.h"

/* Note: The function of this program uses linked lists to store all the values
         so that the user is able to set multiple values at the start of the
         program, and use them further down the instruction list. Therefore, as
         an example, a 'do-loop' node will consists of a variable (e.g. 'A'), a
         start value, and a target */

int main(int argc, char *argv[])
{
  FILE *fpr;
  program prog;
  int i;
  SDL_win sw;

  for(i=0; i<MAXFILE_WORDS; i++){
    prog.list[i][0] = '\0';
  }
  i=0;

  initialise_contents(&prog);
  if(argc != TWO_ARGS){
    ERROR("Incorrect number of arguments...\n");
  }

  if((fpr = fopen(argv[1], "r")) == NULL){
    ERROR("Failed to open file....\n");
  }

  while(fscanf(fpr, "%s", prog.list[i]) == 1){
    if(i == MAXFILE_WORDS){
      ERROR("Instruction list too large...\n");
    }
    i++;
  }
  prog.SDL = &sw; /* For use in the SDL function further below */

  initialise_SDL(&sw); /* Located in 'formsdl.c' */

  if(!check_main(&prog)){
    ERROR("No opening bracket '{' at the start...\n");
  }

  SDL_Quit();
  atexit(SDL_Quit); /* Clears the graphics system */
  Free_Memory_Spaces(&prog);
  fclose(fpr);

  return 0;
}

void initialise_contents(program *prog)/* initiaise contents of the structre */
{
  prog->cw = 0;
  prog->turt = (turtle *)calloc(1, sizeof(turtle));
  prog->set = (node *)calloc(1, sizeof(node));
  if((prog->turt == NULL) || (prog->set == NULL)){
    ERROR("Space allocation failed...\n");
  }
  prog->set->variable = 0;
  prog->turt->x = 500;
  prog->turt->y = 400;
  prog->turt->top = 0;
  prog->turt->direction = 0;
  prog->is_do = NULL;
}

bool check_main(program *prog)
{
  if(!strsame(prog->list[prog->cw], "{")){
    return false;
  }
  prog->cw = prog->cw + 1;
  if((check_instrctlist(prog)) == true){
    return true;
  }

  return false;
}

bool check_instrctlist(program *prog)
{
  if(check_instruction(prog)){
    check_instrctlist(prog);
  }

  if(strsame(prog->list[prog->cw], "}")){
   return true;
  }

  printf("The instruction type '%s' was incorrectly used...\n",prog->list[prog->cw]);
  exit(2);
}

bool check_instruction(program *prog)
{

  if((strsame(prog->list[prog->cw], "SET"))){
    if(is_set(prog)){
      return true;
    }
  }
  if((strsame(prog->list[prog->cw], "DO"))){
    if(is_do(prog)){
     return true;
    }
  }
  else if((forward_move(prog)) || (left_turn(prog)) || (right_turn(prog))){
    return true;
  }

  return false;
}

int forward_move(program *prog)
{
  int distance;
  int old_x, old_y, new_x, new_y;

 if((strsame(prog->list[prog->cw], "FD"))){
  prog->cw++;
   if(varnum(prog)){
    prog->cw--;
     distance = find_distance(prog, prog->set); /* Returns the value of a variable or integer */
      old_x = prog->turt->x, old_y = prog->turt->y;
       prog->turt->y = prog->turt->y + sin(prog->turt->direction* PI/180)*distance;
        prog->turt->x = prog->turt->x + cos(prog->turt->direction* PI/180)*distance;
         new_x = prog->turt->x, new_y = prog->turt->y;
          Execute_SDL(prog, old_x, old_y, new_x, new_y);
           return 1;
    }
  }

  return 0;
}

int left_turn(program *prog)
{
  char *temp;
  long angle;

  if((strsame(prog->list[prog->cw], "LT"))){
   prog->cw++;
    if(varnum(prog)){
     angle = strtol(prog->list[prog->cw-1], &temp, BASE_10);/* Converts the string to a long int */
      prog->turt->direction = prog->turt->direction + (-angle); /* Left turn */
       return 1;
     }
  }
  return 0;
}

int right_turn(program *prog)
{
  char *temp;
  long angle;

  if((strsame(prog->list[prog->cw], "RT"))){
   prog->cw++;
    if(varnum(prog)){
     angle = strtol(prog->list[prog->cw-1], &temp, BASE_10);/* Converts the string to a long int */
      prog->turt->direction = prog->turt->direction + angle; /* Right turn */
       return 1;
     }
  }
  return 0;
}

int find_distance(program *prog, node *set)
{
  long move, distance;

  if(var(prog)){
    if(set->variable == prog->list[prog->cw][0]){/* If its a variable, check if its been SET (i.e. in the 'SET' linked_lists) */
     move = set->variable_value;
      prog->cw++;
       return move; /* Return the value of that variable */
    }
    else if(set->next != NULL){
     return find_distance(prog, set->next);/* Continue searching the 'SET' linked list */
    }
    else if(in_do(prog->is_do, prog)){ /* If the variable is in the 'do-loop' linked list */
     move = in_do(prog->is_do, prog); /* Return the variables value */
      prog->cw++;
       return move;
    }
    printf("The variable %c has not been 'SET'\n",prog->list[prog->cw][0]);
    exit(2);
  }

  distance = atof(prog->list[prog->cw]);  /* If its not a variable, return the number */
  prog->cw++;

  return distance;
}

int in_do(node *do_loop, program *prog)/* Searches the 'do-loop' linked list */
{
  if(do_loop == NULL){ /* The variable has not been found */
    return 0;
  }
  else if(do_loop->variable == prog->list[prog->cw][0]){ /* If the variable in the node is the same as the current string */
    return do_loop->variable_value;
  }
  return in_do(do_loop->next, prog);

  ERROR("Value cannot be found\n");
}

void Execute_SDL(program *prog, int old_x, int old_y, int new_x, int new_y)
{
  Use_SDL_SetDrawColour(prog->SDL, 255,0,0); /* These numbers are for the colour RED */
  Draw_line(prog->SDL,old_x,old_y,new_x,new_y);

  SDL_UpdateScreen(prog->SDL);
  if(Use_SDL_Event()){ /* The user can exit SDL */
    SDL_Quit();
    atexit(SDL_Quit);
  }
  SDL_Delay(MILLISECONDDELAY);
}

bool is_set(program *prog)
{
  char character;
  double sum;

  prog->cw++;
  if(var(prog)){
   character = prog->list[prog->cw][0];/* The current variable to be SET */
    prog->cw++;
     if((strsame(prog->list[prog->cw], ":="))){
      prog->cw++;
       if(is_polish(prog)){
        prog->turt->top--; /* The last instruction in 'is_polish' is 'push', which adds 1 to 'top' */
         sum = prog->turt->stk[prog->turt->top].i; /* The calculated sum on the stack */
          save_set(prog->set, character, sum); /* This saves the value of the 'SET' instruction in a linked list for later use */
           prog->cw++;
            return true;
       }
     }
   }
   ERROR("Grammar incorrect in 'SET' instruction list\n");
}

int varnum(program *prog)
{
  if(((is_number(prog))) || ((var(prog)))){
   prog->cw = prog->cw + 1;
    return 1;
  }

  return 0;
}

int is_number(program *prog)/* Checks if the input is a number */
{
  long i,length;
  double decimal_num, tmp;

  length = strlen(prog->list[prog->cw]);

  for(i=0; i<length; i++){
   if(prog->list[prog->cw][i] < '0' || prog->list[prog->cw][i] > '9'){
    if((prog->list[prog->cw][i] == '.') && (prog->list[prog->cw][i+1] >= '0') && (prog->list[prog->cw][i+1]) <= '9'){ /* Allows decimal values such as 1.5, but not 1.e */
      decimal_num = atof(prog->list[prog->cw]);  /* Check that there are numbers either side*/
     return decimal_num;
    }
    return 0;
   }
  }

  tmp = atof(prog->list[prog->cw]);/* Converts number to double */
  return tmp;
}

bool var(program *prog)
{
  if((prog->list[prog->cw][0]) >= 'A' && (prog->list[prog->cw][0]) <= 'Z'){
   if((prog->list[prog->cw][1]) == '\0'){ /* Strings such as DOG cannot be used */
     return true;
   }
  }

  return false;
}

int is_polish(program *prog)
{
  double temp;
  temp = FLAG;

  if((strsame(prog->list[prog->cw], ";"))){
    return 1;
  }

  if(is_operand(prog)){/* If its an operand, calculate the sum using a stack */
   calculate_sum(prog);
   return is_polish(prog);
  }

  if(is_number(prog)){
   temp = atof(prog->list[prog->cw]); /* Converts number to double */
    push(prog, temp), prog->cw++; /* Push the value onto the stack */
     return is_polish(prog);
  }

  else if(var(prog)){ /* If its a variable, check for it in the linked lists */
    if((in_set(prog->set, prog)) > FLAG){/* If its in the SET linked list */
     temp = in_set(prog->set, prog);
    }
    else if(prog->is_do != NULL && DO_LOOP == INPROGRESS){/* If its in the 'DO' Linked List */
     temp = Check_do_LL(prog->is_do, prog);
    }
    if(temp <= FLAG){/* If the variable has neither been SET or currently used in a 'do-loop' */
      printf("The variable %c has not been SET to a value\n", prog->list[prog->cw][0]);
      exit(2);
    }
    push(prog, temp);/* Push the variables value onto the stack */
    prog->cw++;
    return is_polish(prog);
  }

  ERROR("Incorrect Polish expression...\n");
}

bool is_operand(program *prog)
{
  if((strsame(prog->list[prog->cw], "+")) || (strsame(prog->list[prog->cw], "-"))){
   prog->cw++;
    return true;
  }
  else if((strsame(prog->list[prog->cw], "*")) || (strsame(prog->list[prog->cw], "/"))){
   prog->cw++;
    return true;
  }
  return false;
}

void calculate_sum(program *prog)
{
  double value1, value2, sum;
  value1 = pop(prog), value2 = pop(prog);

  if(varnum(prog)){/* If the current string is a variable or number, then RPN has not been used (i.e. 5 * 2) */
    ERROR("Incorrect RPN used...\n");
  }

  switch(prog->list[prog->cw-1][0]){ /* Negative 1, because 'is_operand' adds 1 */
    case ADDITION: /* Hash defined Ascii values */
     sum = value2 + value1;
     break;
    case SUBTRACTION:
     sum = value2 - value1;
     break;
    case DIVISION:
     sum = value2 / value1;
     break;
    case MULTIPLICATION:
     sum = value2 * value1;
     break;
    default: ERROR("Failure to identify operand...\n");
  }

  push(prog, sum); /* Push the sum onto the stack */
  return;
}

void push(program *prog, double value)
{
  prog->turt->stk[prog->turt->top].i = value;
  prog->turt->top++;
}

double pop(program *prog)
{
  prog->turt->top--;
  return prog->turt->stk[prog->turt->top].i;
}

double in_set(node *set, program *prog)
{
  char variable;

  if(set == NULL){
    return FLAG;/* The Variable is not in the 'SET' linked list  */
  }
  variable = prog->list[prog->cw][0];

  if(variable == set->variable){
    return set->variable_value;
  }
  else {
    return in_set(set->next, prog);
  }

  return 0;
}

double Check_do_LL(node *do_loop, program *prog)/* Search the 'do-loop' linked list */
{
  char current_char;
  current_char = prog->list[prog->cw][0];

  if(do_loop == NULL){return FLAG;}/* If its not in the do-loop linked list */

  if(do_loop->variable == current_char){/* If the variable is in 'do-loop' Linked list return its value */
    return do_loop->variable_value;
  }

  return Check_do_LL(do_loop->next, prog);

  return 0;
}

double save_set(node *set, char character, double sum)/* Searches for the variable/character, and saves the sum value in its linked list node */
{
  if(set->variable == character){ /* Check if the variables occupied */
    set->variable_value = sum;
  }
  if(set->variable == 0){ /* If no character has been SET to the linked list node */
   set->variable = character;
    set->variable_value = sum;
     set->next = (node *)calloc(1, sizeof(node));
      if(set->next == NULL){
        ERROR("Space allocation failed...\n");
      }
      return 1;
  }
  else{
    save_set(set->next, character, sum);
  }

  return 0;
}

bool is_do(program *prog)
{
  char character;

  prog->cw++;
  if(var(prog)){
   character = prog->list[prog->cw][0];
   variable_store(prog, character);
    if((strsame(prog->list[prog->cw], "FROM"))){
     prog->cw++;
      if(varnum(prog)){
       prog->cw--;
       save_value(prog, prog->is_do, character);
        if((strsame(prog->list[prog->cw], "TO"))){
         prog->cw++;
          if(varnum(prog)){
           prog->cw--;
           INITIALISE = TARGET_VALUE, DO_LOOP = INPROGRESS;
            save_value(prog, prog->is_do, character);
             if((strsame(prog->list[prog->cw], "{"))){
              prog->cw++;
               Execute_do_loop(prog, prog->is_do, character);
               INITIALISE = COMPLETE;
                return true;
              }
           }
         }
       }
     }
   }

  return false;
}

void variable_store(program *prog, char character)
{
  if(prog->is_do == NULL){
   prog->is_do = (node *)calloc(1, sizeof(node));
   if(prog->is_do == NULL){
      ERROR("Space allocation failed...\n");
   }
   prog->is_do->variable = character, prog->is_do->initialise = 0;
    prog->cw++;
     return;
  }

  add_linkedlist(prog->is_do, character);
  prog->cw++;
  return;
}

int add_linkedlist(node *is_do, char character)
{
  node *new_space;

  if(is_do->next == NULL){
   new_space = is_do->next = (node *)calloc(1, sizeof(node));
   if(is_do->next == NULL){
      ERROR("Space allocation failed...\n");
   }
   new_space->variable = character, new_space->initialise = 0;
   return 0;
  }
  return add_linkedlist(is_do->next, character);

  ERROR("Space allocation for 'do-loop' failed...\n");
}

double save_value(program *prog, node *is_do, char character)
{
  double number;

  if(is_do == NULL){
    ERROR("Variable for 'do-loop' cannot be found\n");
  }

  if(variable_check(prog) == true){/* A variable has been used */
    return 1;
  }

  else if(is_do->variable == character){
    number = atof(prog->list[prog->cw]);
    if(INITIALISE == TARGET_VALUE){
     is_do->target_value = number;
    }
    else{
     is_do->variable_value = number;
    }
    prog->cw++;
    return 1;
  }

  return save_value(prog, is_do->next, character);

  return 0;
}

bool variable_check(program *prog)
{
  if(var(prog)){/* If its a variable (i.e. 'A'), search the 'SET' linked list and assign the value */
   if(in_set_linkedlist(prog->set, prog)){/* Check if the character has been 'SET' */
    prog->cw++;
     return true;
    }
  ERROR("Variable used in 'do-loop' has not been set to a value...\n");/* I have assumed all variables must be 'SET' */
  }

  return false;
}

int in_set_linkedlist(node *set, program *prog)/* Allows the ‘do-loop’ operation to search the SET linked list, and if the variable is present, copy its value to the required place */
{
  char character;

  if(set == NULL){
    ERROR("Character in 'do-loop' has not been SET...\n");
  }
  character = prog->list[prog->cw][0];

  if(character == set->variable){ /* If the character is in the SET linked list */
    if(INITIALISE == TARGET_VALUE){ /* If the value to be saved is the target_value */
     prog->is_do->target_value = set->variable_value;
    }
    else{
     prog->is_do->variable_value = set->variable_value;
    }
    return 1;
  }
  else{
    return in_set_linkedlist(set->next, prog); /* Search for the character in the SET LL */
  }

  return 0;
}

int Execute_do_loop(program *prog, node *is_do, char character)
{
  if(is_do->variable != character){ /* Finds the start and target values in the linked list for the character (e.g. 'A') in use */
    return Execute_do_loop(prog, is_do->next, character);
  }
  is_do->start_position = prog->cw;/* Saves the location of the first instruction */

  while(is_do->variable_value <= is_do->target_value){
   check_instrctlist(prog); /* Executes all instructions in the list */
    if(is_do->variable_value < is_do->target_value){ /* If the current variable value is less than the target, go back to the first instruction */
     prog->cw = is_do->start_position;
    }

    is_do->variable_value++;
  }

  return 0;
}

void Free_Memory_Spaces(program *prog)
{
  if(prog->is_do != NULL){
    if(prog->is_do->next != NULL){
      free_links(prog->is_do->next);
    }
    free(prog->is_do);
  }

  if(prog->set != NULL){
    if(prog->set->next != NULL){
      free_links(prog->set->next);
    }
    free(prog->set);
  }

  free(prog->turt);
}

void free_links(node *link)
{
  if(link->next == NULL){
    free(link);
    return;
  }
  else{
    free_links(link->next);
    free(link);
  }

  return;
}
