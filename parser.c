#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>
#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "node.h"

#define MAX 1024 //MAX is defined as 1024 (string shouldn't be bigger than that)

TOKEN *tk;

Node* parser(FILE *fp){ //takes in file to send to scanner

	scan_file(fp); //set token array

	Node *root = NULL;
	
	//print_token_array();
	
	
	tk = scanner(); //get first token
	

	root = program();
	
	tk = scanner(); //should get EOF token

	if(strcmp(check_id[tk->id], "EOF_tk") == 0){

		printf("Tree built successfully!\n");

	}
	else {

		printf("ERROR: No EOF_tk detected!\n");
		printf("First BEGIN should just end with only 'END'\n");
		printf("The token '%s' should not be here\n", tk->instance);
		printf("Location at Line: %d\n", tk->line);
		exit(0);
	}


	return root;
}

Node* setNode(char *label){ //label is the nonterminal the node was created from
	Node *node = NULL;
	//malloc for a node structure
	node = malloc(sizeof(Node));
	node->nonterminal = malloc(sizeof(char*));
	strcpy(node->nonterminal, label);
	
	node->token = malloc(sizeof(TOKEN));
	node->token->instance = malloc(sizeof(char*)*MAX);
	
	node->child1 = NULL;
	node->child2 = NULL;
	node->child3 = NULL;
	node->child4 = NULL;
	
	return node;
}


Node* program(){
	Node* PROGRAM = setNode("program");

	PROGRAM->child1 =  vars();
	
	PROGRAM->child2 = block();

	return PROGRAM;
}

Node* block(){
	Node* BLOCK = setNode("block");

	if(strcmp(check_id[tk->id], "BEG_tk") == 0){
		tk = scanner();		
	}
	else {
		printf("~ERROR Program or Statement should start with BEGIN\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);
	}

	BLOCK->child1 = vars();
	

	if(strcmp(check_id[tk->id], "COLON_tk") == 0){
		tk = scanner();

	}
	else {
		printf("~ERROR Program should have ':' after BEGIN or VOID id.\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);

	}

	//according to CFG grammer there should be at least one token here in stats	
	BLOCK->child2 = stats(); 
			
		
	tk = scanner();

	if(strcmp(check_id[tk->id], "END_tk") == 0){
	
		return BLOCK; //success

	}
	else {

		printf("~ERROR Program should end with 'END'\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);
	}	



}


Node* vars(){	

	Node* VARS = setNode("vars");
	
	if(strcmp(check_id[tk->id], "VOID_tk") == 0){
		

		tk = scanner();
	
		
		if(strcmp(check_id[tk->id], "ID_tk") == 0){
			VARS->token = tk;
			VARS->child1 = mvars();
			return VARS;

		}
		else {
			printf("~ERROR: '%s' is Not a Valid Identifier after VOID\n", tk->instance);
			printf("~Located at line: %d\n", tk->line);
			exit(0);
		}
	}
	else { //means empty
		return NULL; //so child equals null
	}
}

Node* mvars(){

	Node* MVARS = setNode("mvars");

	tk = scanner();

	if(strcmp(check_id[tk->id], "ID_tk") == 0){
		MVARS->token = tk;
		MVARS->child1 = mvars();
		return MVARS;
	}
	else if(strcmp(check_id[tk->id], "BEG_tk") == 0){
		return NULL;
	}
	else if(strcmp(check_id[tk->id], "COLON_tk") == 0){
		return NULL;

	}
	else{
		printf("~ERROR: '%s' is Not a Valid Identifier after VOID\n", tk->instance);
		printf("~Located at line: %d\n", tk->line);
		exit(0);
	}
	
}


Node* expr(){
	
	Node* EXPR = setNode("expr");
	
	EXPR->child1 = M();
	
	tk = scanner();
	
	if(strcmp(check_id[tk->id], "PLUS_tk") == 0){ //if it's a plus
		EXPR->token = tk;
		
		EXPR->child2 = expr();
		return EXPR;

	}
	else {
		get_previous();
		return EXPR;
	}
	

}

Node* M(){
	
	Node* MM = setNode("M");
	
	MM->child1 = T();
	
	tk = scanner();
	
	if(strcmp(check_id[tk->id], "MINUS_tk") == 0){ //if it's a minus
		MM->token = tk;
		
		MM->child2 = M();
		return MM;
	}
	else {
		get_previous();
		return MM;
	}
	
}

Node* T(){
	Node* TT = setNode("T");
	
	TT->child1 = F();
	

	tk = scanner();
				

	if(strcmp(check_id[tk->id], "TIMES_tk") == 0){ //if it's a multiplication

		TT->token = tk;
	
		TT->child2 = T();
		return TT;
	}
	
	else if(strcmp(check_id[tk->id], "DIV_tk") == 0){ //if it's a divide
		TT->token = tk;
		TT->child2 = T();
		return TT;
	}
	
	else{

		get_previous();
		return TT;
	}
}

Node* F(){
	Node* FF = setNode("F");
	
	tk = scanner();
	
	if(strcmp(check_id[tk->id], "MINUS_tk") == 0){
		//if it's a negation
		  
		FF->token = tk;
	
		FF->child1 = F();
		return FF;
	}
	else {
		
		FF->child1 = R();
		return FF;
	}
	
}

Node* R(){
	Node* RR = setNode("R");
	
	
	if(strcmp(check_id[tk->id], "LPAR_tk") == 0){ //if it's a left paranthesis 

		RR->token = tk;
		RR->child1 = expr();
		tk = scanner();
		if(strcmp(check_id[tk->id], "RPAR_tk") == 0){ //if it's a right paranthesis 

			return RR;
		}
		else{
			printf("~ERROR no closing paranthesis.\n");
			printf("~Located at line: %d\n", tk->line);
			exit(0);
		}
	}

	if(strcmp(check_id[tk->id], "ID_tk") == 0){ //if it's an identifier
		RR->token = tk;
		
		return RR; //should already have the token

	}
	
	else if(strcmp(check_id[tk->id], "NUM_tk") == 0){ //if it's a number
   
		RR->token = tk;

		return RR; //should already have the token

	}
	else {
		printf("~ERROR no identifier or number detected.\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);
		
		
	}

}

Node* stats(){
	Node* STATS = setNode("stats");

	STATS->child1 = stat();
	
	tk = scanner();
	if(strcmp(check_id[tk->id], "COLON_tk") == 0){
		
		
			
		STATS->child2= mStat();
		
		return STATS;

	}
	else {
		printf("~ERROR '%s' should not be after statment\n", tk->instance);
		printf("~Program should have ':' after statement\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);

	}


}


Node* mStat(){
	Node* MSTAT = setNode("mStat");
	
	tk = scanner();
	
	if(strcmp(check_id[tk->id], "END_tk") == 0){ // <mStat> is empty
		get_previous(); //so we can check end token in <block>
		return NULL;
	}

	else if(strcmp(check_id[tk->id], "EOF_tk") == 0){ //mStat is empty
		get_previous();
		return NULL;
	}

	else {
		MSTAT->child1 = stat();

		tk = scanner();

		if(strcmp(check_id[tk->id], "COLON_tk") == 0){ //multiple statements 

			MSTAT->child2 = mStat();

			return MSTAT;	
		}

		else {
			printf("~ERROR '%s' should not be after statment\n", tk->instance);
			printf("~Program should have ':' after statement\n");
			printf("~Located at line: %d\n", tk->line);
			exit(0);

		}

		//return MSTAT;
	}


}



Node* stat(){
	Node* STAT = setNode("stat");
	
	if(strcmp(check_id[tk->id], "INP_tk") == 0){
		STAT->child1 = in();
		return STAT;

	}

	else if(strcmp(check_id[tk->id], "OUT_tk") == 0){

		STAT->child1 = out();
		return STAT;


	}

	else if(strcmp(check_id[tk->id], "BEG_tk") == 0){

		STAT->child1 = block();
		return STAT;


	}

	else if(strcmp(check_id[tk->id], "IF_tk") == 0){

		STAT->child1 = if_nterm();
		return STAT;


	}

	else if(strcmp(check_id[tk->id], "FOR_tk") == 0){

		STAT->child1 = loop();
		return STAT;

	}

	else if(strcmp(check_id[tk->id], "ID_tk") == 0){

		STAT->child1 = assign();
		return STAT;

	}

	else {
		printf("~ERROR '%s' should not be here\n", tk->instance);
		printf("~Program should have statement\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);

	}
}


Node* in(){
	Node* IN = setNode("in");
	
	tk = scanner();
	if(strcmp(check_id[tk->id], "ID_tk") == 0){
		IN->token = tk;
		
		return IN;
	}
	else {
		printf("~ERROR '%s' should not be after INPUT\n", tk->instance);
		printf("~Program should have Identifier\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);
	}

}

Node* out(){
	Node* OUT = setNode("out");
	OUT->child1 = expr();
	return OUT;
}


Node* if_nterm(){ // <if> named different because if taken
	Node* IF = setNode("if");
	
	tk = scanner();

	if(strcmp(check_id[tk->id], "LPAR_tk") == 0){ //if it's a left paranthesis 

		IF->child1 = expr();

		IF->child2 = RO();

		IF->child3 = expr();

		tk = scanner();

		if(strcmp(check_id[tk->id], "RPAR_tk") == 0){ //if it's a right paranthesis 

			tk = scanner();

			IF->child4 = block();

			return IF;
		}
		else{
			printf("~ERROR no closing paranthesis.\n");
			printf("~Located at line: %d\n", tk->line);
			exit(0);
		}	
	}
	else {

		printf("~ERROR no paranthesis after IF\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);
	}


}


Node* loop(){
	Node* LOOP = setNode("loop");

	tk = scanner();

	if(strcmp(check_id[tk->id], "LPAR_tk") == 0){ //if it's a left paranthesis 

		LOOP->child1 = expr();

		LOOP->child2 = RO();

		LOOP->child3= expr();

		tk = scanner();

		if(strcmp(check_id[tk->id], "RPAR_tk") == 0){ //if it's a right paranthesis 

			tk = scanner();

			LOOP->child4 = block();

			return LOOP;
		}
		else{
			printf("~ERROR no closing paranthesis.\n");
			printf("~Located at line: %d\n", tk->line);
			exit(0);
		}	
	}
	else {

		printf("~ERROR no paranthesis after FOR\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);
	}

}

Node* assign(){
	Node* ASSIGN = setNode("assign");
	
	//get id token again to set to assign node
	get_previous();
	tk = scanner();
	
	ASSIGN->token = tk;
	
	tk = scanner();

	if(strcmp(check_id[tk->id], "EQU_tk") == 0){ //means we have = token
		
		ASSIGN->child1 = expr();
	
		
		return ASSIGN;
 
	}

	else{
		printf("~ERROR identifier needs assinment '='.\n");
		printf("~Located at line: %d\n", tk->line);
		exit(0);

	}

}


Node* RO(){
	Node* OP = setNode("RO"); //op used because RO is function name
	tk = scanner();

	if(strcmp(check_id[tk->id], "GREAT_EQU_GREAT_tk") == 0){ //means we have >=> token
	
		OP->token = tk;
	
		tk = scanner();
		if(strcmp(check_id[tk->id], "EQU_tk") == 0){ //means we have >=> = tokens
			Node* OP_CHILD = setNode("RO");
			OP_CHILD->token = tk;
			OP->child1 = OP_CHILD;
			
			tk = scanner();
			if(strcmp(check_id[tk->id], "ID_tk") == 0 || strcmp(check_id[tk->id], "NUM_tk") == 0 ){
				//if token is id or number success
				get_previous();				
				return OP;
			}
			else {// error
				printf("~ERROR invalid operator\n");
				printf("~Located at line: %d\n", tk->line);
				exit(0);
				
			}
		}

		else { //just a >=>
			get_previous(); //goes back to previous token place
			return OP;

		}
	}


	

	else if(strcmp(check_id[tk->id], "LESS_EQU_LESS_tk") == 0){ //means we have <=< token

		OP->token = tk;
		
		tk = scanner();
		if(strcmp(check_id[tk->id], "EQU_tk") == 0){ //means we have <=< = tokens
			Node* OP_CHILD = setNode("RO");
			OP_CHILD->token = tk;
			OP->child1 = OP_CHILD;
		
			tk = scanner();
			if(strcmp(check_id[tk->id], "ID_tk") == 0 || strcmp(check_id[tk->id], "NUM_tk") == 0 ){
				//if token is id or number success
				get_previous();
				
				return OP;
			}
			else {// error
				printf("~ERROR invalid operator\n");
				printf("~Located at line: %d\n", tk->line);
				exit(0);
				
			}
		}

		else { //just a <=<
			get_previous(); //goes back to previous token place
			return OP;

		}
	}


	else if(strcmp(check_id[tk->id], "TWOEXC_tk") == 0){ //means we have !! token
	
		OP->token = tk;
		
		tk = scanner();
		if(strcmp(check_id[tk->id], "ID_tk") == 0 || strcmp(check_id[tk->id], "NUM_tk") == 0 ){
			//if token is id or number success
			get_previous();
			
			return OP;
		}
		else {// error
			printf("~ERROR invalid operator\n");
			printf("~Located at line: %d\n", tk->line);
			exit(0); 
			
		}
		
	}

	else if(strcmp(check_id[tk->id], "EQU_tk") == 0){ //means we have = = tokens
		OP->token = tk;
	
		tk = scanner();
		if(strcmp(check_id[tk->id], "EQU_tk") == 0){ //means we have = = tokens
			Node* OP_CHILD = setNode("RO");
			OP_CHILD->token = tk;
			OP->child1 = OP_CHILD;
			
			//make sure we have second equal sign
		}
		else {
			printf("~ERROR equal sign should follow another equal sign in operator '= ='\n");
			printf("~Located at line: %d\n", tk->line);
			exit(0);
		}
		//check if id or num follows
		tk = scanner();
		if(strcmp(check_id[tk->id], "ID_tk") == 0 || strcmp(check_id[tk->id], "NUM_tk") == 0 ){
			//if token is id or number success
			get_previous();
			
			return OP;
		}
		else {// error
			printf("~ERROR invalid operator\n");
			printf("~Located at line: %d\n", tk->line);
			exit(0);
			
		}
		
 
	}

	else {
		printf("~ERROR '%s' is an invalid operator.\n", tk->instance);
		printf("~Located at line: %d\n", tk->line);
		exit(0);
	}

}