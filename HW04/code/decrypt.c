#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<shadow.h>
#include<crypt.h>
#include<omp.h>
char dictionary[50][50];
char input[2000];
char salt[200];
char password[1800];
char Encrypted[2000];
char encrypted[1800];
char dictionaryComposed[200];
int main(int argc, char* argv[]){
	int dictionaryCount = 0;
	int i, j, k, salt_index;
	int saltCntFlag = 0;
	int isFind = 0;	

	// read dictionary content
	FILE *fp = fopen("john.txt", "r");
	FILE *fp2 = fopen(argv[1], "r"); 
	while(fscanf(fp, "%s", dictionary[dictionaryCount]) != EOF){
		dictionaryCount++;
	}
		
	fscanf(fp2, "%s", input);		
	
	// parse
	// first catch salt
	for(i = 0; input[i] != ':'; i++){}
	
	// catch algorithm
	salt_index = i + 1;
	
	while(saltCntFlag < 3){
		salt[i - salt_index] = input[i];
		if(input[i] == '$')saltCntFlag ++;
		i++;
	}
	salt_index = i;
	
	// catch password
	for(i = salt_index; input[i] !=':'; i++){
		password[i - salt_index] = input[i];
	}
	strcpy(Encrypted, salt);
	strcat(Encrypted, password);
	printf("input: %s\n", input);
	printf("salt:  %s\n", salt);
	printf("password: %s\n", password);	
	printf("Encrypted: %s\n", Encrypted);
	// start matching
	// the password is composed of three random words from the given dictionary file
	//#pragma omp parallel for
	for(i = 0; i < dictionaryCount; i++){
		for(j = i+1; j < dictionaryCount; j++){
		//	#pragma omp parallel for
			for(k = j+1; k < dictionaryCount; k++){
				// ijk
				strcpy(dictionaryComposed, dictionary[i]);
				strcat(dictionaryComposed, dictionary[j]);
				strcat(dictionaryComposed, dictionary[k]);
				if(strcmp(crypt(dictionaryComposed, salt), Encrypted) == 0){
					printf("%s\n", dictionaryComposed);
					isFind = 1;
					break;
				}
				
				//ikj
				strcpy(dictionaryComposed, dictionary[i]);
                                strcat(dictionaryComposed, dictionary[k]);
                                strcat(dictionaryComposed, dictionary[j]);
                                if(strcmp(crypt(dictionaryComposed, salt), Encrypted) == 0){
                                        printf("%s\n", dictionaryComposed);
                                        isFind = 1;
					break;
                                }
				//jik
				strcpy(dictionaryComposed, dictionary[j]);
                                strcat(dictionaryComposed, dictionary[i]);
                                strcat(dictionaryComposed, dictionary[k]);
                                if(strcmp(crypt(dictionaryComposed, salt), Encrypted) == 0){
                                        printf("%s\n", dictionaryComposed);
                                        isFind = 1;
					break;
                                }
				//jki
				strcpy(dictionaryComposed, dictionary[j]);
                                strcat(dictionaryComposed, dictionary[k]);
                                strcat(dictionaryComposed, dictionary[i]);
                                if(strcmp(crypt(dictionaryComposed, salt), Encrypted) == 0){
                                        printf("%s\n", dictionaryComposed);
                                        isFind = 1;
					break;
                                }
				//kij
				strcpy(dictionaryComposed, dictionary[k]);
                                strcat(dictionaryComposed, dictionary[i]);
                                strcat(dictionaryComposed, dictionary[j]);
                                if(strcmp(crypt(dictionaryComposed, salt), Encrypted) == 0){
                                        printf("%s\n", dictionaryComposed);
                                        isFind = 1;
					break;
                                }
				//kji
				strcpy(dictionaryComposed, dictionary[k]);
                                strcat(dictionaryComposed, dictionary[j]);
                                strcat(dictionaryComposed, dictionary[i]);
                                if(strcmp(crypt(dictionaryComposed, salt), Encrypted) == 0){
                                        printf("%s\n", dictionaryComposed);
                                        isFind = 1;
					break;
                                }		
			}
			if(isFind == 1)break;
		}
		if(isFind == 1)break;
	}	
	return 0;
}
