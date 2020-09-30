#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include "generateCode.h"
#define MAX 100 //number of items in stack

char *stack[MAX]; //stack of strings(variables)
int tos = -1; //top of stack
int varct = -1; //the number of variables in that scope

int block_varct[MAX];
int place = 0;

int exp_check = 0; //check if expression is +, -, *, or /
int LABEL_NUM = 0; //the TRUE or FALSE label number used for multiple IF's or loops

//make the stack
void generate(Node* parse_tree, FILE *out){
	
	if(parse_tree == NULL){ //end of tree (at a leaf when function was called)

		return;
	}
	else if(strcmp(parse_tree->nonterminal, "program") == 0){

		//go through first child (vars)
		generate(parse_tree->child1, out);

		//keep track of number of variables defined
		block_varct[place] = varct;

		//go through second child (block) and so on
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
		
		//pop variables in global scope
		pop(parse_tree->token->instance, out);
		
		//Put STOP at end and create a global temp variable
		//the TEMP variable will be used throughout 
		fprintf(out, "STOP\nTEMP 0\nTEMP2 0\n");
	}
	else if(strcmp(parse_tree->nonterminal, "block") == 0){
		
		
		
		if(parse_tree->child1 == NULL){
			//start varct if vars is empty
			varct = -1; 
		}
		
		
		//go through first child (vars)
		generate(parse_tree->child1, out);

		
		//keep track of number of variables defined
		place++; //add to place for more vartct's
		block_varct[place] = varct;
		
		
	
		//go through second child (stats) and so on
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	
		// once done with a block
		// pop variables in that scope
	
		pop(parse_tree->token->instance, out);
	

	}
	else if(strcmp(parse_tree->nonterminal, "vars") == 0){
		//start varct
		varct = -1; 
		
		//put in stack
		push(parse_tree->token->instance, parse_tree->token, out);
		
		//go through first child (mvars) and so on
		generate(parse_tree->child1, out);
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "mvars") == 0){
		//put in stack
		push(parse_tree->token->instance, parse_tree->token, out);
		
		//go through first child (mvars) and so on
		generate(parse_tree->child1, out);
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "expr") == 0){
		
		//go through first child and so on
		generate(parse_tree->child1, out);
		
		//see if there is a plus token
		//if so put ADD in asm file
		if(strcmp(check_id[parse_tree->token->id], "PLUS_tk") == 0){
			//fprintf(out, "ADD "); 
			exp_check = 1;
			//after ADD would be another argument defined later
		}
		
		
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "M") == 0){
		
		//go through first child and so on
		generate(parse_tree->child1, out);
		
		//see if there is a minus token
		//if so put SUB in asm file
		if(strcmp(check_id[parse_tree->token->id], "MINUS_tk") == 0){
			//fprintf(out, "SUB "); 
			exp_check = 2;
			//after SUB would be another argument defined later
		}
		
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "T") == 0){
		
		//go through first child and so on
		generate(parse_tree->child1, out);
		
		//see if there is a times token
		//if so put MULT in asm file
		if(strcmp(check_id[parse_tree->token->id], "TIMES_tk") == 0){
			//fprintf(out, "MULT "); 
			exp_check = 3;
			//after MULT would be another argument defined later
		}
		
		//see if there is a divide token
		//if so put DIV in asm file
		if(strcmp(check_id[parse_tree->token->id], "DIV_tk") == 0){
			//fprintf(out, "DIV "); 
			exp_check = 4;
			//after DIV would be another argument defined later
		}
		
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "F") == 0){
		
		if(strcmp(check_id[parse_tree->token->id], "MINUS_tk") == 0 && exp_check == 0){
			//load negative one in case its a number
			fprintf(out, "LOAD -1\n"); 
			exp_check = 5;
		}	
		else if(strcmp(check_id[parse_tree->token->id], "MINUS_tk") == 0 && exp_check > 0){ //we have negation
			// now check for expressions
			
			if(exp_check == 1){ //means we had addition
				//so change expression to subtraction
				exp_check = 2;
			}
			else if(exp_check == 2){ //means we had subtraction
				//so change expression to addition
				exp_check = 1;
			}
			else if(exp_check == 3){ //means we had multiplication
				//since temp should already have been defined
				//we load temp and multiply by -1
				fprintf(out, "LOAD TEMP\nMULT -1\nSTORE TEMP\n");
			}
			else if(exp_check == 4){ //means we had division
				//since temp should already have been defined
				//we load temp and multiply by -1
				fprintf(out, "LOAD TEMP\nMULT -1\nSTORE TEMP\n");
			}
			else if(exp_check == 5){ //means we had another negation
			    //so put MULT -1 in asm file
				fprintf(out, "MULT -1\n"); 
				exp_check = 5;
			}
		}
		
		//go through first child and so on
		generate(parse_tree->child1, out);

		
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "R") == 0){
		
		
		//see if token is identifier
		if(strcmp(check_id[parse_tree->token->id], "ID_tk") == 0){
			//check to see if identifier has been defined
			if(find(parse_tree->token->instance) == -1) {
				printf("ERROR: '%s' not defined\n", parse_tree->token->instance);
				printf("Located at line %d\n", parse_tree->token->line);
				exit(0);
			}
			
			//write to asm file
			if(exp_check == 0) //just an id
				fprintf(out, "STACKR %d\nSTORE TEMP\n", find(parse_tree->token->instance));	
			else if(exp_check == 1) // + 
				fprintf(out, "STACKR %d\nSTORE TEMP2\nLOAD TEMP\nADD TEMP2\nSTORE TEMP\n", find(parse_tree->token->instance));	
			else if(exp_check == 2) //  - 
				fprintf(out, "STACKR %d\nSTORE TEMP2\nLOAD TEMP\nSUB TEMP2\nSTORE TEMP\n", find(parse_tree->token->instance));
			else if(exp_check == 3) // * 
				fprintf(out, "STACKR %d\nSTORE TEMP2\nLOAD TEMP\nMULT TEMP2\nSTORE TEMP\n", find(parse_tree->token->instance));
			else if(exp_check == 4) //   /
				fprintf(out, "STACKR %d\nSTORE TEMP2\nLOAD TEMP\nDIV TEMP2\nSTORE TEMP\n", find(parse_tree->token->instance));
			else if(exp_check == 5) //negation
				fprintf(out, "STACKR %d\nMULT -1\nSTORE TEMP\n", find(parse_tree->token->instance));
		}
		
		//see if token is number
		if(strcmp(check_id[parse_tree->token->id], "NUM_tk") == 0){
			// write to asm file
			if(exp_check == 0)//just an id
				fprintf(out, "LOAD %s\nSTORE TEMP\n", parse_tree->token->instance);	
			else if(exp_check == 1)// +
				fprintf(out, "ADD %s\nSTORE TEMP\n", parse_tree->token->instance);
			else if(exp_check == 2)//- 
				fprintf(out, "SUB %s\nSTORE TEMP\n", parse_tree->token->instance);
			else if(exp_check == 3)//* 
				fprintf(out, "MULT %s\nSTORE TEMP\n", parse_tree->token->instance);
			else if(exp_check == 4)//  /
				fprintf(out, "DIV %s\nSTORE TEMP\n", parse_tree->token->instance);
			else if(exp_check == 5)//negation
				fprintf(out, "MULT %s\nSTORE TEMP\n", parse_tree->token->instance);
		}
		
		exp_check = 0;
		
		//go through other nonterminals child and so on (don't care right now)
		//we only care about static semantics
		generate(parse_tree->child1, out);
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
		
		
	}
	else if(strcmp(parse_tree->nonterminal, "in") == 0){
		//check to see if identifier has been defined
		if(find(parse_tree->token->instance) == -1) {
				printf("ERROR: '%s' not defined\n", parse_tree->token->instance);
				printf("Located at line %d\n", parse_tree->token->line);
				exit(0);
		}
		
		int n = find(parse_tree->token->instance);
		
		//READ in number and STORE it in stack
		fprintf(out, "READ TEMP\nLOAD TEMP\nSTACKW %d\n", n);
		
		
		//go through other nonterminals child and so on (don't care right now)
		//we only care about static semantics
		generate(parse_tree->child1, out);
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "out") == 0){
		
		//go through other nonterminals child and so on (don't care right now)
		//we only care about static semantics
		generate(parse_tree->child1, out);
		
		fprintf(out, "STORE TEMP\nWRITE TEMP\n"); 
		
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "if") == 0){
		
		
		
		generate(parse_tree->child1, out);
		
		//after first expression (child 1) TEMP should be made
		//LOad the TEMP and store into TEMP2
		fprintf(out,"LOAD TEMP\nSTORE TEMP2\n"); 
		
		//get second expression (child 3) and another TEMP will be made
		generate(parse_tree->child3, out);
		
		//So subtract TEMP from TEMP2
		fprintf(out, "LOAD TEMP2\nSUB TEMP\nSTORE TEMP\n");
		
		
		//IN second child (R0) we check how we are comparing
		// based on the symbol we determine which BR to use and which label to call
		generate(parse_tree->child2, out);
		
		
		//before fourth child make a label to do something if true
		fprintf(out, "TRUE%d: ", LABEL_NUM);
		LABEL_NUM++; //add to the TRUE LABEL NUMBER
		generate(parse_tree->child4, out);
		
		
		LABEL_NUM--;
		//after fourth child make a label to do nothing if FALSE
		fprintf(out, "FALSE%d: NOOP\n", LABEL_NUM);
		

	}
	else if(strcmp(parse_tree->nonterminal, "loop") == 0){
		

		
		generate(parse_tree->child1, out);
		
		//after first expression (child 1) TEMP should be made
		//LOad the TEMP and store into TEMP2
		fprintf(out,"LOAD TEMP\nSTORE TEMP2\n"); 
		
		//get second expression (child 3) and another TEMP will be made
		generate(parse_tree->child3, out);
		
		//So subtract TEMP from TEMP2
		fprintf(out, "LOAD TEMP2\nSUB TEMP\nSTORE TEMP\n");
		
		//IN second child (R0) we check how we are comparing
		// based on the symbol we determine which BR to use and which label to call
		generate(parse_tree->child2, out);
		
		//put label before child4 so in asm code it keeps running
		fprintf(out, "TRUE%d: ", LABEL_NUM);
		LABEL_NUM++;
		
		generate(parse_tree->child4, out);
		
		//subtract when out of possible other loop
		LABEL_NUM--;
		
		//after code is done check the condition!!!!! AGAIN!!!!
		generate(parse_tree->child2, out);
		
		//after fourth child make a label to do nothing if FALSE
		fprintf(out, "FALSE%d: NOOP\n", LABEL_NUM);
		
		
	}
	
	
	else if(strcmp(parse_tree->nonterminal, "assign") == 0){
		//check to see if identifier has been defined
		if(find(parse_tree->token->instance) == -1) {
				printf("ERROR: '%s' not defined\n", parse_tree->token->instance);
				printf("Located at line %d\n", parse_tree->token->line);
				exit(0);
		}
		
		//go through other nonterminals child and so on (don't care right now)
		//we only care about static semantics
		generate(parse_tree->child1, out);
		
		int n = find(parse_tree->token->instance); //get place
		
		fprintf(out, "STACKW %d\n", n);
		
		
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	else if(strcmp(parse_tree->nonterminal, "RO") == 0){
		
		//check for type of comparing
		
		if(strcmp(check_id[parse_tree->token->id], "GREAT_EQU_GREAT_tk") == 0){
			//we have a >=>
					
			//check for =
			if(parse_tree->child1 == NULL){
				//we just have a >=>
				// it's only true if positive
				fprintf(out, "BRPOS TRUE%d\nBR FALSE%d\n", LABEL_NUM, LABEL_NUM);
			}
			else {
				//we have a >=> =
				// only true if positive or equal to
				fprintf(out, "BRZPOS TRUE%d\nBR FALSE%d\n", LABEL_NUM, LABEL_NUM);
				
			}
		}
		
		if(strcmp(check_id[parse_tree->token->id], "LESS_EQU_LESS_tk") == 0){
			//we have a <=<
			
			//check for =
			if(parse_tree->child1 == NULL){
				//we just have a <=<
				//only true when negative
				fprintf(out, "BRNEG TRUE%d\nBR FALSE%d\n", LABEL_NUM, LABEL_NUM);
			}
			else {
				//we have a <=< =
				//only true when negative or equal to
				fprintf(out, "BRZNEG TRUE%d\nBR FALSE%d\n", LABEL_NUM, LABEL_NUM);
			}	
		}
		
		if(strcmp(check_id[parse_tree->token->id], "TWOEXC_tk") == 0){
			//we have a !!
			//only true when it's not zero
			//otherwise false
			fprintf(out, "BRZERO FALSE%d\nBR TRUE%d\n", LABEL_NUM, LABEL_NUM);			
		}
		
		if(strcmp(check_id[parse_tree->token->id], "EQU_tk") == 0){
			//we have a =
			
			//check for another =
			if(parse_tree->child1 != NULL){
				//we just have a = =
				//only true when zero
				fprintf(out, "BRZERO TRUE%d\nBR FALSE%d\n", LABEL_NUM, LABEL_NUM);
			}
			//else there should have been an error earlier in program
		}
		
		//RO would only have one child
		//we already checked the one child
		//so there is no need to call generate() for more children after
		
	}
	
	
	else { 
		//go through other nonterminals children
		generate(parse_tree->child1, out);
		generate(parse_tree->child2, out);
		generate(parse_tree->child3, out);
		generate(parse_tree->child4, out);
	}
	
	return;
}

void push(char* string, TOKEN *tk, FILE *out){

	//printf("PUSH Place: %d\n", place);

	if(check_me(string) == 1){ //has not been defined
		if(tos == MAX - 1){ //stack overflow
			printf("ERROR stack overflow\n");
			printf("Too many variables defined\n");
			exit(0);
			
		}
		else { // have not reached MAX number in stack
			tos++;
			varct++;
			//printf("\tPUSH: %s\n", string);
			strcpy(stack[tos], string); 
			
			//print to asm file
			fprintf(out, "PUSH\n");
			return;	
		}	
	}
	else if(check_me(string) == -1){ //has already been defined ERROR
		printf("ERROR '%s' has already been defind in scope!\n", string);
		printf("Located at line: %d\n", tk->line);
		exit(0);
	}
	
	return;
}

void pop(char* string, FILE *out){
	
	
	//printf("POP Place: %d\n", place);
	
	if(tos == -1){
		return; 
	}
	else {
		
		if(place == 0){ //means we are done with local variables in blocks
		
			//pop the globals
			while(tos != -1){
				//printf("\tGLOBAL_POP: %s\n", stack[tos]);
				tos--;
				//print to asm file
				fprintf(out, "POP\n");
			}
		}
		else { //pop the locals
			
			
			while(block_varct[place] != -1){
				
				//printf("\tPOP: %s\n", stack[tos]);
				tos--;
				block_varct[place]--;
				//print to asm file
				fprintf(out, "POP\n");
			}
		}
	}
	
	place--;
	
	return;
}

//check if defined more than once
int check_me(char* string){
	int check = 1;
	
	if(tos != -1){ //means we are not at the bottom of stack
		
		//stack_ct is the number of variables in that scope
		int stack_ct;
		stack_ct = varct;
	
		//scope is the place where we are at in the stack
		int scope = tos;
	
		while(stack_ct != -1){
			//means variable has been defined in that scope
			if(strcmp(string, stack[scope]) == 0){
				check = -1;	
			}
			scope--;
			stack_ct--;
		}
	}
	
	return check;
}

//see if variable is in stack then if so
//find the distance of a variable in the stack
int find(char* string){
	//if distance is -1 then not found
	//else found the variable
	int distance = -1;
	
	int top_of_stack = tos; //used to loop through stack
	

	
	while(top_of_stack != -1){
		if(strcmp(string, stack[top_of_stack]) == 0){			
			//found string
			distance = tos - top_of_stack; //set distance from top of stack
	
			break;
		}
	
		top_of_stack--;
	}
	

	return distance;
}

void print_stack(){

	while(tos != 0){
		printf("%s\n", stack[tos]);
		tos--;	
	}
	
	return;
}