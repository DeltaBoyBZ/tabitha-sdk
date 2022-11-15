/*
Copyright 2022 Matthew Peter Smith

This file is provided under the terms of the Mozilla Public License - Version 2.0. 
A copy of the licence can be found in the root of The Tabitha SDK GitHub repository,

https://github.com/DeltaBoyBZ/tabitha-sdk 

or alternatively a copy can be found at,

https://www.mozilla.org/media/MPL/2.0/index.815ca599c9df.txt


*/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<math.h>

/*
void std_printIntLn(int x)
{
    printf("%d\n", x); 
}
*/

void std_printFloatLn(float x)
{
    printf("%f\n", x); 
}

void std_printCharLn(char x)
{
    printf("%c\n", x); 
}

void std_printTruthLn(bool x)
{
    if(x) printf("true\n"); 
    else  printf("false\n"); 
}

void std_printLongLn(long x)
{
    printf("%li\n", x);
}

void std_printPtrLn(void* x)
{
    printf("%li\n", (long)x); 
}

void std_printLn(char* x)
{
    printf("%s\n", x); 
}

char* std_readFile(char* filename)
{
    FILE* file = fopen(filename, "r"); 
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file); 
    fseek(file, 0, SEEK_SET);
    char* contents = (char*) malloc(fileSize+1); 
    fread(contents, 1, fileSize, file); 
    fclose(file); 
    contents[fileSize] = '\0';
    return contents;
}

void std_writeFile(const char* filename, const char* contents)
{
    FILE* file = fopen(filename, "w");
    fwrite(contents, 1, strlen(contents), file); 
    fclose(file); 
}

void std_appendFile(const char* filename, const char* text)
{
    FILE* file = fopen(filename, "a");
    fwrite(text, 1, strlen(text), file); 
    fclose(file); 
}

float std_cos(float x)
{
    return cos((double)x);
}

float std_sin(float x)
{
    return sin((double)x);
}


int std_charToInt(char x)
{
    return (int)x; 
}

/*
int std_readInt()
{
    int x = 0;
    char input[20];
    scanf("%19s", input); 
    x = atoi(input);
    return x;
}
*/

float std_readFloat()
{
    float x = 0.0;
    char input[20]; 
    scanf("%19s", input); 
    x = atof(input); 
    return x; 
}

char std_readChar()
{
    char x = '\0'; 
    scanf("%c", &x); 
    return x;
}

void std_readLn(char* buffer)
{
    scanf("%s", buffer); 
}

    
