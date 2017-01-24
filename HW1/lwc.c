#include<stdio.h>
#include<string.h>

// parameters
int nFileIndex = 0;
int nLine;
int nWord;
int nCount;
char szFileContent[1000000];
int nReturnValue = 0;

int nTotalLine = 0;
int nTotalWord = 0;
int nTotalCount = 0;

int FileLWC(int test, int i, char* argv[]){
	
	int error = 0;
	FILE *fileInput;
	nFileIndex = i;
	
	nLine = 0;
	nWord = 0;
	nCount = 0;
	
	fileInput = fopen(argv[nFileIndex], "r");
			
	// file exist
	if(fileInput){
		while(fgets(szFileContent, 1000000, fileInput) != NULL){
			
			// -l Line 
			nLine++;
		}
		
		// Total (multiple)
		nTotalLine = nTotalLine + nLine;		
		
		
		// seek to the beginning of the file
		fseek(fileInput, 0, SEEK_SET);
		while(fscanf(fileInput, "%s", szFileContent) != EOF){
			
			// -w Word
			nWord++;
		}
		
		nTotalWord = nTotalWord + nWord;
		
		// -c Count
		fseek(fileInput, 0, SEEK_END);
		nCount = ftell(fileInput);
		
		nTotalCount = nTotalCount + nCount;
		
		error = 0;
		fclose(fileInput);
	}
	
	// file not existed (error)
	else{
		if(test==1){
			error = -1;
			nReturnValue = 1;
			fprintf(stderr, "wc: %s: No such file or directory\n", argv[i]);
		}
	}
	
	return error;
}
int main(int argc, char* argv[]){
	
	
	char szerror;
	
	// display
	int nl = 0;
	int nw = 0;
	int nc = 0;
	int ntotal = 0;
	int nDigits;
	int isMultiple = -1;
	
	// temp variables
	int i, j, tmp, tmpDigit;
	int nlen, nlen1;
	int error = 0;
	
	
	for(i = 1; i < argc; i++){
		
		// search the file 
		if(argv[i][0] == '-'){
			
			nlen1 = strlen(argv[i]);
			for(j = 1; j < nlen1; j++){
				if(argv[i][j] == 'l'){
					nl = 1;
				}
				else if(argv[i][j] == 'w'){
					nw = 1;
				}
				else if(argv[i][j] == 'c'){
					nc = 1;
				}
				else{
					ntotal = -1;
					szerror = argv[i][j];
					break;
				}
			}
			
			// Invalid option(error)
			if(ntotal == -1){
				nReturnValue = 1;
				fprintf(stderr, "wc: invalid option -- '%c'\n", szerror);
				fprintf(stderr, "Try 'wc --help' for more information.\n");
				break;
			}
		}
	}
	
	if(ntotal != -1){
		
		// first get the biggest digits
		for(i = 1; i < argc ;i++){
			if(argv[i][0] != '-'){
				error = FileLWC(0, i, argv);
			}
		}
		
		// calculate the count digit
		nDigits = 0;
		tmp = nTotalCount;
		
		if(tmp == 0){
			nDigits = 1;
		}
		else{		
			while(tmp != 0)
			{
				tmp /= 10;             // n = n/10
				++nDigits;
			}
		}
		nTotalLine = 0;
		nTotalWord = 0;
		nTotalCount = 0;
				
		// then show
		for(i = 1; i < argc; i++){
			
			error = -1;
			// search the file 
			if(argv[i][0] != '-'){
				error = FileLWC(1, i, argv);
				isMultiple ++;
			}
			
			// if not error
			if(error != -1){
				
				// display
				ntotal = nl + nw + nc;
				if(ntotal == 1){
					if(nl == 1){
						printf("%d %s\n", nLine, argv[nFileIndex]);
					}
					else if(nw == 1){
						printf("%d %s\n", nWord, argv[nFileIndex]);
					}
					else{
						printf("%d %s\n", nCount, argv[nFileIndex]);
					}
				}
				else{
					if(nl == 1 ){
						tmp = nLine;
						tmpDigit = 0;
						if(tmp == 0){
							tmpDigit = 1;
						}
						else{		
							while(tmp != 0)
							{
								tmp /= 10;
								++tmpDigit;
							}
						}
						
						for(j = 0; j < nDigits - tmpDigit; j++){
							printf(" ");
						}
						printf("%d ", nLine);
					}
					if(nw == 1){
						tmp = nWord;
						tmpDigit = 0;
						if(tmp == 0){
							tmpDigit = 1;
						}
						else{		
							while(tmp != 0)
							{
								tmp /= 10;
								++tmpDigit;
							}
						}
						
						for(j = 0; j < nDigits - tmpDigit; j++){
							printf(" ");
						}
						printf("%d ", nWord);
					}
					if(nc == 1){
						tmp = nCount;
						tmpDigit = 0;
						if(tmp == 0){
							tmpDigit = 1;
						}
						else{		
							while(tmp != 0)
							{
								tmp /= 10;
								++tmpDigit;
							}
						}
						
						for(j = 0; j < nDigits - tmpDigit; j++){
							printf(" ");
						}
						printf("%d ", nCount);
					}
					printf("%s\n", argv[nFileIndex]);
				}
			}
		}
	}
	
	if(isMultiple > 0){
		nDigits = 0;
		tmp = nTotalCount;
				
		if(tmp == 0){
			nDigits = 1;
		}
		else{		
			while(tmp != 0)
			{
				tmp /= 10;             // n = n/10
				++nDigits;
			}
		}
		
		// line
		tmp = nTotalLine;
		tmpDigit = 0;
		if(tmp == 0){
			tmpDigit = 1;
		}
		else{		
			while(tmp != 0)
			{
				tmp /= 10;
				++tmpDigit;
			}
		}
						
		for(j = 0; j < nDigits - tmpDigit; j++){
			printf(" ");
		}
		printf("%d ", nTotalLine);
		
		// word
		tmp = nTotalWord;
		tmpDigit = 0;
		if(tmp == 0){
			tmpDigit = 1;
		}
		else{		
			while(tmp != 0)
			{
				tmp /= 10;
				++tmpDigit;
			}
		}
					
		for(j = 0; j < nDigits - tmpDigit; j++){
			printf(" ");
		}
		printf("%d ", nTotalWord);
		
		// count 
		printf("%d total\n", nTotalCount);
	}
	return nReturnValue;
}
