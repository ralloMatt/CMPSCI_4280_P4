#ifndef GENERATE_H
#define GENERATE_H 
#include "node.h" 

//geneate code while checking static semantics
void generate(Node*, FILE*);

//push onto stack
void push(char*, TOKEN*, FILE*);

//pop off of stack
void pop(char*, FILE*);

//used to see if a variable has been defined more than once in stack
int check_me(char*);

//used to see if variable has been defined
int find(char*); 

void print_stack();
#endif