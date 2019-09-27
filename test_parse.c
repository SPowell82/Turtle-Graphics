#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>

/* Note: All test cases that are commented out were used during the process of
         of building this program, but cannot be used now because additional
         functions have been added */


void test_check_main(program *prog)
{
  program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw;

  assert(check_main(NULL) == 0);
  assert(prog->cw == 0); /* Ensures the first string scanned is the first in the input file */

  assert(strcmp(prog->list[prog->cw], "{") == 0);
  assert(strcmp(prog->list[prog->cw], "}") != 0);
  assert(strcmp(prog->list[prog->cw], "") > 0);

  if(strcmp(prog->list[prog->cw], "{") != 0){
    assert(check_main(prog) == false);
  }

  strcpy(test.list[temp], "");
   assert((check_main(&test) == false));
  strcpy(test.list[temp], "H");
   assert((check_main(&test) == false));
}

void test_check_instrctlist(program *prog)
{
  program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw;

  assert(prog->cw == 1); /* Ensures 'cw' has incremented */
  if(strcmp(prog->list[prog->cw], "}") == 0){
    assert(check_instrctlist(prog) == true);
  }

  strcpy(test.list[temp], "FD");
  strcpy(test.list[temp+1], "30"); /* Added after construction of the 'varnum' function */
  strcpy(test.list[temp+2], "LT");
  strcpy(test.list[temp+3], "10");
  strcpy(test.list[temp+3], "}");
  assert((check_instrctlist(&test)) == true);
}

void test_check_instruction(program *prog)
{
/*  program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw;*/
  assert(check_instruction(NULL) == false);

   if(strcmp(prog->list[prog->cw], "FD") == 0){
       assert(check_instruction(prog) != false);
   }
   if(strcmp(prog->list[prog->cw], "SET") == 0){
       assert(check_instruction(prog) == true);
   }
   if(strcmp(prog->list[prog->cw], "RT") == 0){
       assert(check_instruction(prog) != false);
   }
   if(strcmp(prog->list[prog->cw], "LT") == 0){
       assert(check_instruction(prog) == true);
   }

   /* These test cases can no longer be used after additional functions (i.e. varnum)
      were implemented as there are additional checks that must pass */

   /*strcpy(test.list[temp], "hcsd");
    assert((check_instruction(&test) != 1));
   strcpy(test.list[temp], "13SDSDS");
    assert((check_instruction(&test) == 0));
   strcpy(test.list[temp], "FD");
    assert((check_instruction(&test) == 1));
   strcpy(test.list[temp], "FDFDFDFDFDFD");
    assert((check_instruction(&test) == 0));
   strcpy(test.list[temp], "LT");
    assert((check_instruction(&test) == 1));
   strcpy(test.list[temp], "SET");
    assert((check_instruction(&test) == 1));*/

}

void test_forward_move(program *prog)
{
  /*program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw; */
  /* These test cases were altered after the 'varnum' function was implemented
                          -- Explained further in test_parse.txt --   */

  /*if(forward_move(prog) == 0){
    assert(strcmp(prpg->list[prog->cw], "FD") != 0);
  }
  if(forward_move(prog) == 1){
    assert(strcmp(prog->list[prog->cw], "FD") == 0);
  }*/

   if(forward_move(prog) == 1){
     assert(strcmp(prog->list[prog->cw-2], "FD") == 0);
   } /* This ensures that the current word has incremented properly */
   if(forward_move(prog) == 1){
     assert(strcmp(prog->list[prog->cw-2], "RT") != 0);
   }
/*   strcpy(test.list[temp], "FD");
    assert((forward_move(&test) == 1));
   strcpy(test.list[temp], "FDFDFDFDFDFD");
    assert((forward_move(&test) == 0));
   strcpy(test.list[temp], "RT");
    assert((forward_move(&test) != 1));
   strcpy(test.list[temp], "  ");
    assert((forward_move(&test) == 0));*/

}

void test_left_turn(program *prog)
{
/*  program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw;*/

  /*if(left_turn(prog) == 0){
    assert(strcmp(prpg->list[prog->cw], "LT") != 0);
  }
  if(left_turn(prog) == 1){
    assert(strcmp(prog->list[prog->cw], "LT") == 0);
  }*/

  if(left_turn(prog) == 1){
    assert(strcmp(prog->list[prog->cw-2], "LT") == 0);
  }
  if(left_turn(prog) == 1){
    assert(strcmp(prog->list[prog->cw-2], "LI") != 0);
  }
/*
  strcpy(test.list[temp], "LT");
   assert((left_turn(&test) == 1));
  strcpy(test.list[temp], "K");
   assert((left_turn(&test) == 0));
  strcpy(test.list[temp], "RT");
   assert((left_turn(&test) != 1));
  strcpy(test.list[temp], " { } ");
   assert((left_turn(&test) == 0));*/
}

void test_right_turn(program *prog)
{
  /*  program test;
    int temp;

    test.list[0][0] = '\0';
    temp = prog->cw, test.cw = prog->cw;*/

  /*if(left_turn(prog) == 0){
    assert(strcmp(prpg->list[prog->cw], "LT") != 0);
  }
  if(left_turn(prog) == 1){
    assert(strcmp(prog->list[prog->cw], "LT") == 0);
  }*/
  if(right_turn(prog) == 1){
    assert(strcmp(prog->list[prog->cw-2], "RT") == 0);
  }
  if(right_turn(prog) == 1){
    assert(strcmp(prog->list[prog->cw-2], "LI") != 0);
  }

/*  strcpy(test.list[temp], "RT");
   assert((left_turn(&test) == 1));
  strcpy(test.list[temp], "K");
   assert((left_turn(&test) == 0));
  strcpy(test.list[temp], "T");
   assert((left_turn(&test) != 1));
  strcpy(test.list[temp], " 436545645456 ");
   assert((left_turn(&test) == 0));*/

}

void test_varnum(program *prog)
{
   program test;
   int temp;

   test.list[0][0] = '\0';
   temp = prog->cw, test.cw = prog->cw;

   strcpy(test.list[temp], "hcsd");
    assert((varnum(&test) == 0));
   strcpy(test.list[temp], "13");
    assert((varnum(&test) == 1));
   strcpy(test.list[temp], "S");
    assert((varnum(&test) != 0));
   strcpy(test.list[temp], "L");
    assert((varnum(&test) == 1));
   strcpy(test.list[temp], "k");
    assert((varnum(&test) == 0));
   strcpy(test.list[temp], "(");
    assert((varnum(&test) == 0));

}

void test_is_number(program *prog)
{
  program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw;

  strcpy(test.list[temp], "1.5");
   assert((is_number(&test) == 1));
  strcpy(test.list[temp], "13");
   assert((is_number(&test) == 1));
  strcpy(test.list[temp], "S");
   assert((is_number(&test) == 0));
  strcpy(test.list[temp], "w");
   assert((is_number(&test) == 0));
  strcpy(test.list[temp], "48834893289");
   assert((is_number(&test) == 1));
  strcpy(test.list[temp], "8374373788743783487387");
   assert((is_number(&test) == 1));

}

void test_var(program *prog)
{
  program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw;

  strcpy(test.list[temp], "A");
   assert((var(&test) == 1));
  strcpy(test.list[temp], "F");
   assert((var(&test) == 1));
  strcpy(test.list[temp], "CAT");
   assert((var(&test) == 0));
  strcpy(test.list[temp], "1.22");
   assert((var(&test) == 0));
  strcpy(test.list[temp], "48834893289");
   assert((var(&test) == 0));
  strcpy(test.list[temp], "k");
   assert((var(&test) == 0));
  strcpy(test.list[temp], "AHDBDHDBDJDJJKAJJ");
    assert((var(&test) == 0));
  strcpy(test.list[temp], "");
    assert((var(&test) == 0));
}
void test_is_operand(program *prog)
{
  program test;
  int temp;

  test.list[0][0] = '\0';
  temp = prog->cw, test.cw = prog->cw;

  switch(prog->list[prog->cw][0]){
     case ADDITION: /* Hash defined Ascii values */
     assert(strcmp(prog->list[prog->cw], "+") == 0);
     break;
     case SUBTRACTION:
     assert(strcmp(prog->list[prog->cw], "-") == 0);
     break;
     case DIVISION:
     assert(strcmp(prog->list[prog->cw], "/") == 0);
     break;
     case MULTIPLICATION:
     assert(strcmp(prog->list[prog->cw], "*") == 0);
     break;
     default: assert(is_operand(prog) == false);/* The string is not an operand */
  }

  strcpy(test.list[temp], "A");
   assert((is_operand(&test) == 0));
  strcpy(test.list[temp], "++");
   assert((is_operand(&test) == 0));
  strcpy(test.list[temp], "!");
   assert((is_operand(&test) == 0));
  strcpy(test.list[temp], "/");
   assert((is_operand(&test) == 1));
  strcpy(test.list[temp], "*");
   assert((is_operand(&test) == 1));
  strcpy(test.list[temp], "k");
   assert((is_operand(&test) == 0));
  strcpy(test.list[temp], "-");
    assert((is_operand(&test) == 1));
}

void test_is_polish(program *prog)
{
  program test, test2;
  int temp, hold;

  temp = prog->cw, test.cw = prog->cw;
  hold = test2.cw = prog->cw;

  strcpy(test.list[temp], "A");
  strcpy(test.list[temp+1], "5");
  strcpy(test.list[temp+2], "*");
  strcpy(test.list[temp+3], ";");
  assert((is_polish(&test)) == 1);
  assert(test.cw > hold); /* Ensures 'cw' has incremented properly */

  strcpy(test2.list[temp], "2");
  strcpy(test2.list[temp+1], "+");
  strcpy(test2.list[temp+2], "4");
  strcpy(test2.list[temp+3], ";");
  assert((is_polish(&test2)) != 0);

}

void test_is_set(program *prog)
{
  program test, test2, test3;
  int temp;

  temp = prog->cw;
  test.cw = test2.cw = test3.cw = prog->cw;

  strcpy(test.list[temp], ""); /* The 'SET' function increments by 1 at the start */
  strcpy(test.list[temp+1], "A");
  strcpy(test.list[temp+2], ":=");
  strcpy(test.list[temp+3], "10");
  strcpy(test.list[temp+4], "/");
  strcpy(test.list[temp+5], "2");
  strcpy(test.list[temp+6], ";");
  assert((is_set(&test)) == true);

  strcpy(test2.list[temp], "");
  strcpy(test2.list[temp+1], "CAT");
  strcpy(test2.list[temp+2], ":=");
  strcpy(test2.list[temp+3], "10");
  strcpy(test2.list[temp+4], "/");
  strcpy(test2.list[temp+5], "2");
  strcpy(test2.list[temp+6], ";");
  assert((is_set(&test2)) == false);

  strcpy(test3.list[temp], "");
  strcpy(test3.list[temp+1], "C");
  strcpy(test3.list[temp+2], ":");
  strcpy(test3.list[temp+3], "10jbjbjbjb");
  strcpy(test3.list[temp+4], "/");
  strcpy(test3.list[temp+5], "2");
  strcpy(test3.list[temp+6], ";");
  assert((is_set(&test3)) == false);

}

void test_is_do(program *prog)
{
  program test, test2;
  int temp;

  temp = prog->cw;
  test.cw = test2.cw = prog->cw;

  strcpy(test.list[temp], ""); /* The 'DO' function increments by 1 at the start */
  strcpy(test.list[temp+1], "C");
  strcpy(test.list[temp+2], "FROM");
  strcpy(test.list[temp+3], "A");
  strcpy(test.list[temp+4], "TO");
  strcpy(test.list[temp+5], "100");
  strcpy(test.list[temp+6], "{");
  strcpy(test.list[temp+7], "FD");
  strcpy(test.list[temp+8], "65");
  strcpy(test.list[temp+9], "}");
  assert((is_do(&test)) == true);

  strcpy(test2.list[temp], "");
  strcpy(test2.list[temp+1], "C");
  strcpy(test2.list[temp+2], "x");
  strcpy(test2.list[temp+3], "A");
  strcpy(test2.list[temp+4], "TO");
  strcpy(test2.list[temp+5], "100");
  strcpy(test2.list[temp+6], "{");
  strcpy(test2.list[temp+7], "FD");
  strcpy(test2.list[temp+8], "65");
  strcpy(test2.list[temp+9], "}");
  assert((is_do(&test2)) == false);
}
