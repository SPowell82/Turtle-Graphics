# Turtle-Graphics

A Turtle Graphics program which uses SDL to display the images. In order to move the turtle the instructions 
are as followed:

FD - Forward 
RT - Right Turn
LT - Left Turn
SET - Assigns a value to a specified variable
DO - Runs an instruction in a loop for a specified amount of time (similar to a for-loop) 

Examples for the 'SET' and 'DO' instructions:

SET A := 5 * 2 ---- This will set the value 10 to the variable A 

// NESTED LOOP WITH DO-LOOP FUNCTIONALITY

{
  DO A FROM 1 TO 50 {
     FD A
     RT 30
     DO B FROM 1 TO 8 {
        SET C := A 5 / ; // C wiill be assigned to A / 5 
        FD C
        RT 45
     }
   }
   
}

The standard program is contained within interpreter.c, while extention.c contains a program with additional 
functionality which allows the user to store moves for the turtle to use.
