#include "interpreter.h"

int main(int argc, char *argv[])
{
  FILE *fpr;
  program prog;
  int i;

  prog.cw = 0;
  for(i=0; i<MAXFILE_WORDS; i++){
    prog.list[i][0] = '\0';
  }
  i=0;

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

  if(check_main(&prog) == false){
    ERROR("No opening bracket '{' at the start..\n");
  }

  return 0;
}

bool check_main(program *prog)
{
  if(prog == NULL){
    return 0;
  }

  if(!strsame(prog->list[prog->cw], "{")){
    return false;
  }
  prog->cw = prog->cw + 1;

  if(check_instrctlist(prog)){
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
  else{
    printf("Instruction type was incorrect after '%s\n",prog->list[prog->cw-1]);
    exit(2);
  }

  return false;
}

bool check_instruction(program *prog)
{
  if(prog == NULL){
    return false;
  }

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

   if((forward_move(prog)) || (left_turn(prog)) || (right_turn(prog))){
    return true;
  }

  return false;
}

int forward_move(program *prog)
{
  if((strsame(prog->list[prog->cw], "FD"))){
   prog->cw++;
   if(varnum(prog)){
    prog->cw++;
     return 1;
   }
    prog->cw--;
  }
  return 0;
}

int left_turn(program *prog)
{
  if((strsame(prog->list[prog->cw], "LT"))){
   prog->cw++;
    if(varnum(prog)){
      prog->cw++;
      return 1;
     }
     prog->cw--;
  }
  return 0;
}

int right_turn(program *prog)
{
  if((strsame(prog->list[prog->cw], "RT"))){
   prog->cw++;
    if(varnum(prog)){
      prog->cw++;
      return 1;
     }
     prog->cw--;
  }
  return 0;
}

bool is_do(program *prog)
{
  prog->cw++;
   if(var(prog)){
    prog->cw++;
     if((strsame(prog->list[prog->cw], "FROM"))){
       prog->cw++;
       if(varnum(prog)){
         prog->cw++;
          if((strsame(prog->list[prog->cw], "TO"))){
            prog->cw++;
             if(varnum(prog)){
               prog->cw++;
                if((strsame(prog->list[prog->cw], "{"))){
                  prog->cw++;
                   if(check_instrctlist(prog)){
                     return true;
                   }
                }
             }
          }
       }
     }
  }
  return false;
}
bool is_set(program *prog)
{
  prog->cw++;
  if(var(prog)){
    prog->cw++;
     if((strsame(prog->list[prog->cw], ":="))){
       prog->cw++;
        if(is_polish(prog)){
         return true;
        }
    }
  }

  return false;
}

int is_polish(program *prog)
{

  if((strsame(prog->list[prog->cw], ";"))){
    prog->cw++;
    return 1;
  }

  else if(is_operand(prog)){
    prog->cw = prog->cw + 1;
     return is_polish(prog);
  }
  else if(varnum(prog)){
    prog->cw = prog->cw + 1;
     return is_polish(prog);
  }
  else{
    printf("Incorrect Polish expression after %s\n", prog->list[prog->cw-1]);
    exit(2);
  }

  return 0;
}

bool is_operand(program *prog)
{
  if((strsame(prog->list[prog->cw], "+")) || (strsame(prog->list[prog->cw], "-"))){
    return true;
  }
  else if((strsame(prog->list[prog->cw], "*")) || (strsame(prog->list[prog->cw], "/"))){
    return true;
  }
  return false;
}


int varnum(program *prog)
{
  if(((is_number(prog))) || ((var(prog)))){
    return 1;
  }

  return 0;
}

int is_number(program *prog)
{
  int i, length;
  length = strlen(prog->list[prog->cw]);

  for(i=0; i<length; i++){
   if(prog->list[prog->cw][i] < '0' || prog->list[prog->cw][i] > '9'){
    if((prog->list[prog->cw][i] == '.') && (prog->list[prog->cw][i+1] >= '0') && (prog->list[prog->cw][i+1]) <= '9'){ /* Allows decimal values such as 1.5, but not 1.e */
     return 1;
    }
    return 0;
   }
  }

  return 1;
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
