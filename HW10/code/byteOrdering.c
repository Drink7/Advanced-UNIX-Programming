#include <stdio.h>
int main(){

	// Big endian:		  MSB		       LSB
	// Byte addr		| 0x01 | 0x02 | 0x03 | 0x04 |
	// Byte content		| 0x10 | 0x23 | 0x87 | 0x56 |
	// Little endian:	  MSB		       LSB
	// Byte addr		| 0x04 | 0x03 | 0x02 | 0x01 |
	// Byte content		| 0x10 | 0x23 | 0x87 | 0x56 |	
	// i is a 4 byte integer
	int i = 0x10238756;
	
	// context is a pointer point to a char(1 byte)
	// &i is to fetch the address of i
	// use (char*) to a int address so that we can get the first byte of the integer i
	char *content;
	content = (char*) &i;
	printf("*content = 0x%x\n", *content);
	if(*content = 0x56){
		printf("This architecture is little endian order\n");
	}
	else{
		printf("This architecture is big endian order\n");
	}


	return 0;
}
