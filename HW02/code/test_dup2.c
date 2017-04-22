/****************

This sample test case will redirect the standard output (fp = 1) to a file. 

Please change <your_function_name> to your dup2 function name. 

2016.9.25 

****************/
#include <unistd.h> /*Included for dup(2) and write(2)*/
#include <stdio.h>
#include <stdlib.h> /*Included for exit()*/
#include "hw2.h"
#include <errno.h>
#include <fcntl.h>

typedef int (*FunDup2)(int, int);
 
int main(void){

    //Usage FunDup2 mdup2 = <your_dup2_function_name>;
    FunDup2 mdup2 = mydup2;

    //Open a file to write
    int file = open("myfile.txt", O_WRONLY | O_CREAT);
    if(file < 0)return 1;

    char buff[] = "Pen-Pineapple-Apple-Pen\n";

    //redirect standard output
    int newfd = -1;
    if((newfd = mdup2(file,1)) < 0)return 1;
    
    //now printf will print the string to file
    printf("%s\n", buff);
    
    close(file);
 
    return 0;
}
