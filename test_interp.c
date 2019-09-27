#include "interpreter.h"

/* Note: For the majority of functions in parse.c, I have not changed their
         functionality in interpreter.c. However, some functions required
         additional code for interpretation, so may include additional tests */

void test_initialise_contents(program *prog)
{
   int x, y, direction;

   x = prog->turt->x, y = prog->turt->y;
   direction = prog->turt->direction;

   assert(x == 500);/* Unable to compare floating points directly using '=='  */
   assert(y == 400);
   assert(direction == 0);
   assert(prog->cw == 0);
   assert(prog->turt != NULL);
   assert(prog->set != NULL);
   assert(prog->set->variable == 0);
   assert(prog->is_do == NULL);
}

void test_PushAndPop()/* Ensures numbers are manipulated properly on the stack  */
{
  program test;
  int test_val, test_val2, temp;
  test_val = 10, test_val2 = 22;

  initialise_contents(&test);
  test.list[0][0] = '\0';

  push(&test, test_val);
  temp = (int) test.turt->stk[test.turt->top-1].i; /* Force cast required due to double output */
  assert(temp == 10);/* I have used minus 1 because the push operation adds 1 to the top */
  push(&test, test_val2);
  temp = (int) test.turt->stk[test.turt->top-1].i;
  assert(temp == 22);
  temp = pop(&test);
  assert(temp == 22);
  temp = pop(&test);
  assert(temp == 10);
}

void test_calulate_sum()/* Ensures numbers are calculated on the stack properly */
{
  program test;
  int temp, test_val, test_val2, value;
  test_val = 20, test_val2 = 5;

  initialise_contents(&test);
  test.list[0][0] = '\0';
  temp = 0, test.cw = 1; /* 1 because the calculate_sum function minuses 1 from the current string */

  push(&test, test_val);
  push(&test, test_val2);
  strcpy(test.list[temp], "+");
  calculate_sum(&test); /* Pushes the sum onto the stack */
  value = (int) test.turt->stk[test.turt->top-1].i; /* Force cast required because of 'double' return */
  assert(value == 25);

  push(&test, test_val2);
  strcpy(test.list[temp], "/"); /* 25/5 */
  calculate_sum(&test);
  value = (int) test.turt->stk[test.turt->top-1].i;
  assert(value == 5);

  push(&test, test_val);
  strcpy(test.list[temp], "*"); /* 5*20 */
  calculate_sum(&test);
  value = (int) test.turt->stk[test.turt->top-1].i;
  assert(value == 100);
}

void test_save_set()/* This function has been extended to test the 'in_Set' function as well */
{
  program test;
  int test_val,temp;
  char character;

  character = 'A'; /* Variable to be saved */
  test_val = 20; /* Value of the variable */

  initialise_contents(&test);
  test.list[0][0] = '\0';

  save_set(test.set, character, test_val);
  assert(test.set->variable == character);/* Ensures the correct character has been saved */
  temp = (int) test.set->variable_value;
  assert(temp == test_val);     /* Ensures the correct value has been saved */

  test.list[test.cw][0] = 'A'; /* 'In_Set' will then search the linked list for 'A', and if its present, return its value */
  temp = (int) in_set(test.set, &test);
  assert(temp == test_val); /* 'A' was assigned to 20 above */
}

void test_is_polish_ext()/* Tests the additional features of the function */
{
  /* Tests all the operations of 'is_polish' (push, pop, in_set and calculate sum work together) */

  program test; /*test2;*/
  int test_val, temp, temp_value; /*temp2*/
  char character;

  character = 'A', test_val = 20, temp = 0; /*temp2 = 0;*/

  initialise_contents(&test);
  test.list[0][0] = '\0';
  /*initialise_contents(&test2);
  test2.list[0][0] = '\0';*/

  save_set(test.set, character, test_val);/* SETs A to value 20*/
  strcpy(test.list[temp], "5");
  strcpy(test.list[temp+1], "A");
  strcpy(test.list[temp+2], "*");
  strcpy(test.list[temp+3], ";"); /* 5*20 */
  is_polish(&test); /* Executes push, pop, in_set and calculate_sum */

  temp_value = (int) pop(&test);
  assert(temp_value == 100);

/*  strcpy(test2.list[temp2], "5");     This test ensures that the 'is_polish'
  strcpy(test2.list[temp2+1], "B");     function can also identify variables
  strcpy(test2.list[temp2+2], "*");     that have not been SET.
  strcpy(test2.list[temp2+3], ";");
  is_polish(&test2); */
}

void test_variable_store()
{
  program test;
  char character;
  character = 'Z';

  initialise_contents(&test);
  test.list[0][0] = '\0';

  variable_store(&test, character);
  assert(test.is_do != NULL);
  assert(test.is_do->variable != 0); /* The initially set value */
  assert(test.is_do->next == NULL);
  assert(test.is_do->variable == character);
}

void test_save_value()
{
  program test;
  char character;
  int temp;
  character = 'Z';

  initialise_contents(&test);
  test.list[0][0] = '\0';
  test.cw = 0;

  variable_store(&test, character);/* Stores the variable 'Z' */
  test.list[test.cw][0] = '1'; /* Random integer */
  assert(variable_check(&test) == false); /* The current string (5) is not a variable */
  save_value(&test, test.is_do, character); /* Saves 5 as start point  */
  temp = (int) test.is_do->variable_value;
  assert(temp != 0); /* The initially set value */
  assert(temp == 1);

  test.list[test.cw][0] = '9'; /* Random integer */
  test.is_do->initialise = 1;
  save_value(&test, test.is_do, character);
  temp = (int) test.is_do->variable_value;
  assert(temp != 0);
  assert(temp == 9);
}

void test_find_distance()
{
  program test;
  int test_val;
  char character;

  character = 'A'; /* Variable to be saved */
  test_val = 20; /* Value of the variable */

  initialise_contents(&test);
  save_set(test.set, character, test_val);
  test.list[test.cw][0] = 'A';/* Ensures 'find_distance' can detect a SET variable */
  assert((find_distance(&test, test.set)) == 20);
  assert((find_distance(&test, test.set)) != 0);

  test.list[test.cw][0] = '9';/* Ensures 'find_distance' can detect an integer */
  assert((find_distance(&test, test.set)) == 9);
  assert((find_distance(&test, test.set)) != 20);

/* This ensures 'find_distance' can detect an unSET variable, but has been commented
   out because the program will report an error */

/*  test.list[test.cw][0] = 'Z';
  assert((find_distance(&test, test.set)) == 20);
  assert((find_distance(&test, test.set)) != 0);*/
}
