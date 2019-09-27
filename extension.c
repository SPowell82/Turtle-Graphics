#include "interpreter.h"

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
    ERROR("No opening bracket '{' at the start..\n");
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
  prog->table_size = START_SIZE;
  prog->num_strings = 0; /* Number of strings used to store instructions */
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
  prog->store = (program *)calloc(1, sizeof(program));/* A new 'program' to store instructions */
  prog->store->hash_array = (hashtable *)calloc(START_SIZE, sizeof(hashtable));
  if((prog->store == NULL) || (prog->store->hash_array == NULL)){
    ERROR("Space allocation failed...\n");
  }
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
  if((strsame(prog->list[prog->cw], "STORE"))){
   if(Store_instructions(prog)){ /* Instructions have been stored successfully */
     return true;
   }
  }

  if((strsame(prog->list[prog->cw], "EXECUTE"))){
   if(Execute_instruction(prog)){
     return true;
   }
  }
  else if((forward_move(prog)) || (left_turn(prog)) || (right_turn(prog))){
    return true;
  }

  return false;
}

int Store_instructions(program *prog)
{
  int cell, i;
  i=0;

  if(Valid_name(prog)){ /* Name is three characters or longer */
     if(prog->num_strings >= (prog->table_size/2)){ /* Resise the hash table when its half full */
       resize_table(prog);
     }
     cell = hash(prog->table_size, prog->list[prog->cw]); /* Find the cell for insertion */

     if(prog->store->hash_array[cell].word == NULL){
      prog->store->hash_array[cell].word = (char *)calloc(MAXWORD, sizeof(char));
       if(prog->store->hash_array[cell].word == NULL){
        ERROR("Space allocation failed...\n");
       }
       strcpy(prog->store->hash_array[cell].word, prog->list[prog->cw]); /* STORE the current string */
         prog->cw++, prog->num_strings++; /* Increment the number of strings */

         if(strsame(prog->list[prog->cw], "{")){
          prog->cw++;
           while(!strsame(prog->list[prog->cw], "END")){ /* Signifies the end of the instruction list */
            strcpy(prog->store->hash_array[cell].instruc_list[i], prog->list[prog->cw]);/* Store in instructions in the hash table */
             i++, prog->cw++;
           }
           prog->cw++;
           if(strsame(prog->list[prog->cw], "}")){/* Accounts for double brackets in nested 'do-loops' */
           prog->cw++;
            }
           return 1;
         }
      }

    if(cell_available(&prog->store->hash_array[cell].next, prog, cell)){/* Checks if a linked list from that cell is present */
      return 1;
    }

  }

  return 0;
}

int cell_available(hashtable **hash, program *prog, int cell)
{
  int i;
  i = 0;

  if((*hash) == NULL){ /* There is no linked list */
   (*hash) = (hashtable *)calloc(1, sizeof(hashtable));
   (*hash)->word = (char *)calloc(MAXWORD, sizeof(char));
   if(((*hash) == NULL) || ((*hash)->word == NULL)){
     ERROR("Space allocation failed...\n");
   }

   strcpy((*hash)->word, prog->list[prog->cw]); /* Store the string name */
    prog->cw++, prog->num_strings++;
     if(strsame(prog->list[prog->cw], "{")){
      prog->cw++;
      while(!strsame(prog->list[prog->cw], "END")){
      strcpy((*hash)->instruc_list[i], prog->list[prog->cw]);/* Save ths instructions in the hash table (linked list) */
      i++, prog->cw++;
      }

      prog->cw++;
      if(strsame(prog->list[prog->cw], "}")){
       prog->cw++;
      }
      return 1;
     }
  }
  else{
  return cell_available(&(*hash)->next, prog, cell);/* Check for the next node */
}

  return 0;
}

int Execute_instruction(program *prog)/* Executes instructions of a given string */
{
  int k, cell;
  program hold;
  initialise_contents(&hold);
  k = 0;
  (&hold)->SDL = (*prog).SDL;/* Targets the initial SDL window */
  prog->cw++;

  cell = hash(prog->table_size, prog->list[prog->cw]);

    if(prog->store->hash_array[cell].word != NULL){ /* If theres a word in the cell, check if its the current executing string */
     if(strsame(prog->store->hash_array[cell].word, prog->list[prog->cw])){
      while(*(prog->store->hash_array[cell].instruc_list[k]) != '\0'){ /* Copy the instructions into a tempory 'program' */
      strcpy(hold.list[k], prog->store->hash_array[cell].instruc_list[k]);
      k++;
      }

      check_instrctlist(&hold); /* Execute the instructions */
      prog->cw++;
      return 1;
     }

     if(Use_linkedlist(&prog->store->hash_array[cell].next, &prog, cell)){/* If the word in the cell is not the same, check the linked lists */
       return 1;
     }

    }


 return 0;
}

int Use_linkedlist(hashtable **hash, program **prog, int cell)
{

 int row;
 program hold;
 initialise_contents(&hold);
 hold.SDL = (*prog)->SDL;/* Target the initial SDL window */
 row = 0;

  if((*hash) != NULL){/* If the linked list node is not empty */
    if(strsame((*hash)->word, (*prog)->list[(*prog)->cw])){

     while((*(*hash)->instruc_list[row]) != '\0'){
     strcpy(hold.list[row], (*hash)->instruc_list[row]);
     row++;
     }

     check_instrctlist(&hold); /* Execute the instructions */
     (*prog)->cw++;
     return 1;
    }

   if((*hash)->next != NULL){/* If the nodes not empty, but contains a different string, search other nodes */
      return Use_linkedlist(&(*hash)->next,&(*prog),cell);
    }
  }

  return 0;
}


bool Valid_name(program *prog)
{
  int i, length;
  prog->cw++;
  length = strlen(prog->list[prog->cw]);

  if(length >= 3){/* A Valid_name is minimum three characters long */
   for(i=0; i<length-1; i++){
    if(prog->list[prog->cw][i] < 'A' || prog->list[prog->cw][i] > 'Z'){
     return false;
    }
   }
   return true;
  }

  return false;/* The string is less than three characters long */
}

int hash(unsigned int sz, char *s)/* A provided hash function */
{
  unsigned long hash = 5381;
  int c;

  while((c = (*s++))){
    hash = 33 * hash ^ c;
  }

  return (int)(hash%sz);
}

void resize_table(program *prog)
{
  int cell, i;
  program *new_table;
  program temp;
  cell = 0, i = 0;

  new_table = (program *)calloc(1, sizeof(program));
  initialise_contents(new_table);
  new_table->store = (program *)calloc(1, sizeof(program));
  new_table->table_size = prog->table_size * GROWTH_FACTOR;/* GROWTH_FACTOR = 4 */

  new_table->store->hash_array = (hashtable *)calloc(new_table->table_size, sizeof(hashtable));
  if(new_table == NULL || new_table->store == NULL || new_table->store->hash_array == NULL ){
    ERROR("Space allocation failed...\n");
  }
  new_table->num_strings = prog->num_strings;

  while(*(prog->list[prog->cw]) != '\0'){/* Copy all current strings into the new 'program' list */
   strcpy((*new_table).list[i], prog->list[prog->cw]);
   prog->cw++;
   i++;
  }

   while(cell < prog->table_size){
    if(prog->store->hash_array[cell].word != NULL){
     Insert_strings(prog, &new_table, cell);/* Copies all STORED strings and their instructions */
      if(prog->store->hash_array[cell].next != NULL){/* Copies all STORED strings in linked lists */
       Insert_linkedlists(&(prog->store->hash_array[cell].next), &new_table);

      }
    }
    cell++;
  }
  (*new_table).SDL = (*prog).SDL; /* Targets the initial SDL window */

  temp = (*prog); /* Swaps the pointers of the old and new 'program' containing the larger hash table */
  (*prog) = (*new_table);
  (*new_table) = temp;

  free_table(&new_table);/* Frees the old table/'program' */
}


void Insert_strings(program *prog, program **new_table, int cell)
{
   int new_cell, i;
   i = 0;

   new_cell = hash((*new_table)->table_size, prog->store->hash_array[cell].word);/* Obtains the new array cell number for the string in the linked list */

   (*new_table)->store->hash_array[new_cell].word = (char *)calloc(MAXWORD, sizeof(char));
   if((*new_table)->store->hash_array[new_cell].word == NULL){
     ERROR("Space allocation failed...\n");
   }
   strcpy((*new_table)->store->hash_array[new_cell].word, prog->store->hash_array[cell].word);/* Copies the word into the new cell */

   while(*(prog->store->hash_array[cell].instruc_list[i]) != '\0'){/* Copies the stored instruction list into the new array */
     strcpy((*new_table)->store->hash_array[new_cell].instruc_list[i], prog->store->hash_array[cell].instruc_list[i]);
     i++;
   }

   return;
}

int Insert_linkedlists(hashtable **next, program **new_table)/* Copies the STORED strings/instructions in linked lists */
{
  int i, cell;
  i = 0;

  cell = hash((*new_table)->table_size, (*next)->word);

  if((*new_table)->store->hash_array[cell].word == NULL){/* If the array cell in the new table is empty, copy the string and instructions into that cell */
   (*new_table)->store->hash_array[cell].word = (char *)calloc(MAXWORD,sizeof(char));
   if((*new_table)->store->hash_array[cell].word == NULL){
     ERROR("Space allocation failed...\n");
   }
   strcpy((*new_table)->store->hash_array[cell].word, (*next)->word);

   while(*((*next)->instruc_list[i]) != '\0'){/* Copy the instructions */
     strcpy((*new_table)->store->hash_array[cell].instruc_list[i], (*next)->instruc_list[i]);
     i++;
   }

  }
  else{
    Add_New_Link(&(*new_table)->store->hash_array[cell].next, &(*next));/* If the cell in the new table is not empty (A string has been copied in), form a linked list */
  }

  if((*next)->next != NULL){/* If there are additional linked lists in the old table, go to them and copy their strings/instructions */
    return Insert_linkedlists(&(*next)->next, &(*new_table));
  }

  return 1;
}

int Add_New_Link(hashtable **new_table_next, hashtable **input)/* Forms a linked list if the array cell in the new table is occupied */
{
  int i;
  i = 0;

  if((*new_table_next) == NULL){/* If a linked list has not been formed */
    (*new_table_next) = (hashtable *)calloc(1, sizeof(hashtable));
     (*new_table_next)->word = (char *)calloc(MAXWORD,sizeof(char));
     if((*new_table_next) == NULL || (*new_table_next)->word == NULL){
       ERROR("Space allocation failed...\n");
     }

     strcpy((*new_table_next)->word, (*input)->word);

     while(*((*input)->instruc_list[i]) != '\0'){/* Copy the instruction list  */
      strcpy((*new_table_next)->instruc_list[i], (*input)->instruc_list[i]);
      i++;
     }
  }
  else{
    return Add_New_Link(&(*new_table_next)->next, &(*input));/* If a linked list has been formed, make another one */
  }

  return 0;
}

void free_table(program **table)/* Frees the old table */
{
  int cell;
  cell = 0;

  while(cell < (*table)->table_size){
   if((*table)->store != NULL){
    if((*table)->store->hash_array[cell].word != NULL){
     if((*table)->store->hash_array[cell].next != NULL){
      free_linkedlist(&(*table)->store->hash_array[cell].next);
     }
   free((*table)->store->hash_array[cell].word);
    }
   }
   cell++;
  }

  free((*table)->store->hash_array);
  free((*table)->store);
  free((*table)->turt);
  free((*table));
  table = NULL;

  return;
}

void free_linkedlist(hashtable **node)/* Frees the linked lists in each cell of the old table */
{
  if((*node)->next == NULL){
    if((*node)->word != NULL){
      free((*node)->word);
    }
    free((*node));
    return;
  }
  else{
    free_linkedlist(&(*node)->next);
    free((*node)->word);
    free((*node));
  }

  return;
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
    if(set->variable == prog->list[prog->cw][0]){/* If its a variable, check if its been SET (i.e. in the 'SET' linked lists) */
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

  if(variable_check(prog) == true){ /* A variable has been used */
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
