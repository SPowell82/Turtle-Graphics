#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#define MILLISECONDDELAY 20
#define INPROGRESS 1
#define COMPLETE 0
#define BASE_10 10
#define FLAG -1
#define ADDITION 43 /* Ascii values for the operand symbols */

#define SUBTRACTION 45
#define DIVISION 47
#define MULTIPLICATION 42
#define PI 3.14159265358979323846
#define START_SIZE 2
#define GROWTH_FACTOR 2

#define MAXFILE_WORDS 200
#define MAXSTR_LENGTH 20
#define MAXWORD 30
#define MAXFILENAME 100
#define TWO_ARGS 2
#define strsame(A,B) (strcmp(A,B) == 0)
#define ERROR(STR) {fprintf(stderr, STR); exit(2);}

#define INITIALISE prog->is_do->initialise /* A way to distinguish between setting the target value and variable_value */
#define VARIABLE_VAL is_do->variable_value
#define TARGET is_do->target_value
#define TARGET_VALUE 1
#define DO_LOOP prog->is_do->loop_variable

#define WIN_WIDTH 1000
#define WIN_HEIGHT 800
#define ERROR(STR) {fprintf(stderr, STR); exit(2);}

struct SDL_win{
  SDL_Window *window;
  SDL_Renderer *render;
  SDL_Texture *display;
  SDL_bool finished;
};
typedef struct SDL_win SDL_win;

struct node{/* Linked list to save the values for later use */
  char variable;
  double variable_value;
  int initialise;
  int start_position;
  int loop_variable;
  double target_value;
  struct node *next;
};
typedef struct node node;

struct stackelem{
   double i;
};
typedef struct stackelem elem;

struct turtle{
  double x;
  double y;
  double direction;
  elem stk[100];
  int top;
};
typedef struct turtle turtle;

struct hashtable{ /* Used in extension */
  char *word;
  char instruc_list[MAXFILE_WORDS][MAXSTR_LENGTH];
  struct hashtable* next;
};
typedef struct hashtable hashtable;

struct program{
  char list[MAXFILE_WORDS][MAXSTR_LENGTH];
  int cw;
  int table_size; /* Extension */
  int num_strings; /* Extension */
  SDL_win *SDL;
  turtle *turt;
  node *set;
  node *is_do;
  hashtable *hash_array; /* Extension */
  struct program *store;
};
typedef struct program program;

/* Parser and Interpreter functions */
int varnum(program *prog);
int forward_move(program *prog);
int right_turn(program *prog);
int left_turn(program *prog);
void push(program *prog, double n);
bool check_instruction(program *prog);

int add_linkedlist(node *is_do, char character);
int find_distance(program *prog, node *link);
double Check_do_LL(node *temp, program *prog);
double in_set(node *a, program *prog);
int Execute_do_loop(program *prog, node *is_do, char character);
bool is_do(program *prog);
int in_do(node *link, program *prog);
double save_value(program *prog, node *is_do, char character);

bool is_operand(program *prog);
int is_polish(program *prog);
bool is_set(program *prog);
bool var(program *prog);
bool check_main(program *prog);
bool check_instrctlist(program *prog);
void Execute_SDL(program *prog, int old_x, int new_x, int old_y, int new_y);
bool variable_check(program *prog);

double pop(program *prog);
void calculate_sum(program *prog);
int is_number(program *prog);
double save_set(node *a, char character, double sum);
void variable_store(program *prog, char character);
int in_set_linkedlist(node *point, program *prog);
void initialise_contents(program *prog);
void Free_Memory_Spaces(program *prog);
void free_links(node *link);

/*  SDL Functions */

void Draw_line(SDL_win *sw, int old_x, int old_y, int new_x, int new_y);
void initialise_SDL(SDL_win *sw);
void SDL_UpdateScreen(SDL_win *sw);
int Use_SDL_Event();
void Use_SDL_SetDrawColour(SDL_win *sw, int num1, int num2, int num3);

/* Extension Functions */

int Use_linkedlist(hashtable **hash, program **prog, int cell);
int cell_available(hashtable **hash, program *prog, int cell);
int Execute_instruction(program *prog);
int hash(unsigned int sz, char *s);
bool Valid_name(program *prog);
int Store_instructions(program *prog);
void resize_table(program *prog);
void Insert_strings(program *prog, program **new_table, int cell);
int Insert_linkedlists(hashtable **next, program **new_table);
int Add_New_Link(hashtable **new_table_next, hashtable **input);
void free_table(program **table);
void free_linkedlist(hashtable **node);

/* Test functions */

void test_find_distance();
void test_save_value();
void test_variable_store();
void test_is_polish_ext();
void test_save_set();
void test_initialise_contents(program *prog);
void test_check_main(program *prog);
void test_check_instrctlist(program *prog);
void test_check_instruction(program *prog);
void test_forward_move(program *prog);
void test_left_turn(program *prog);
void test_right_turn(program *prog);
void test_varnum(program *prog);
void test_is_number(program *prog);
void test_var(program *prog);
void test_is_operand(program *prog);
void test_is_polish(program *prog);
void test_is_set(program *prog);
void test_is_do(program *prog);
void test_PushAndPop();
void test_calulate_sum();
