#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "parser.h"  
#include "treePrint.h"
#include "generateCode.h"

#define MAX 1024 //MAX is defined as 1024 (string shouldn't be bigger than that)

int main(int argc, char *argv[]) {

	char full_filename[MAX]; // file name with extention
	char filename[MAX]; // filename without extention


	if(argc == 1){ //read from keyboard
		char buf[MAX]; // create a buffer to store the list of words

		///new idea
		strcpy(filename, "out"); // filename is temp
		strcpy(full_filename, "out.datE02"); // filename with extention

		FILE *fp_temp = NULL; //create a temporary file pointer for writing to a file

		// creating temporary file called temp.datE02
		fp_temp = fopen(full_filename, "w+"); //create file, delete content if exists


		if(isatty(0)){ //reading from the KEYBOARD!!!!
			printf("Reading from the keyboard. Note: Only accepts one line.\n");
			printf("Example( Enter Program: BEGIN VOID Y: INPUT Y: END\n");
			printf("Enter Program: ");
			fgets(buf, MAX, stdin);
			printf("\nTokens entered from keyboard: %s\n", buf); // for testing purposes
			fputs(buf, fp_temp); 

		}

		fclose(fp_temp); // close that temporary file because we are done with it for now
	}

		else if( argc == 2){ // read from file

			strcpy(filename, argv[1]); //copy argument to filename (for outputs later)
			strcpy(full_filename, argv[1]); // copy argument to full filename
			strcat(full_filename, ".datE02"); // full filename with extention

			//printf("filename is %s, full filename is %s\n\n", filename, full_filename);

		}
			else { // Error too many arguments given
				printf("ERROR: Too many arguments given!\n");
				return 0;
			}


	
	FILE *fp = NULL; // create file pointer
	if(fp = fopen(full_filename, "r")){ // means there is a file and open it  

		//send the file to parser.c
		//parser.c sends file to scanner.c
		//scanner.c reads the file and gets the characters
		//scanner.c determines what kind of token those characters are
		//then scanner.c creates array of tokens
		//afterwords parser.c uses the token array to determine grammer
		//then parser.c creates the tree

		printf("File is %s \n\n", full_filename);

		Node* tree = NULL;
		tree = parser(fp); //send file pointer parser.c
		
		//Print tree
		//traversePreorder(tree, 1);

		char out_file[MAX]; //the asm file to be produced
		strcpy(out_file, filename);
		strcat(out_file, ".asm"); // full filename with extention
		FILE *output = NULL;
		output = fopen(out_file, "w+"); //create file, delete content if exists
		
		
		//Generate the code (write to the asm file)
		generate(tree, output);
		printf("\nCode generated Successfully\n");

		fclose(output);
		fclose(fp); // close the main file
	}
	else
		printf("ERROR: File does not exist!!!\n");



	printf("\nEnd of Program!!!\n");
	return 0;
}