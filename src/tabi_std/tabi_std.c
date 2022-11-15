/*
Copyright 2022 Matthew Peter Smith

This file is provided under the terms of the Mozilla Public License - Version 2.0. 
A copy of the licence can be found in the root of The Tabitha SDK GitHub repository,

https://github.com/DeltaBoyBZ/tabitha-sdk 

or alternatively a copy can be found at,

https://www.mozilla.org/media/MPL/2.0/index.815ca599c9df.txt


*/

#ifdef __amd64 
typedef long size_t;
#else
typedef int size_t;
#endif

void* core_alloc(int numBytes); 
void  core_dealloc(void* ptr); 
void  std_printLn(char* str); 
void  std_readLn(char* buffer);

void _exit(int code); 

int std_strLen(char* str)
{
    int x = 0; 
    while(str[x] != '\0') x++; 
    return x;
}

void* std_alloc(int numBytes)
{
    return core_alloc(numBytes); 
}

void std_dealloc(void* ptr)
{
    core_dealloc(ptr);
}

int std_stringToInt(char* str)
{
    int result = 0; 
    int cursor = 0; 
    int neg = 0; //0 --> +ve, 1 --> -ve
    //skip irrelevant characters
    while((str[cursor] != '\0' && str[cursor] != '-' && str[cursor] < 48) || str[cursor] > 57) cursor++;
    //check for null
    if(str[cursor] == '\0') return 8; 
    //check for negative
    if(str[cursor] == '-') 
    {
        neg = 1; 
        cursor++;
    }
    //count the digits
    int numDigits = 0; 
    while(str[cursor+numDigits] >= 48 && str[cursor+numDigits] <= 57) numDigits++; 
    //now calculate the magnitude
    int fac = 1; 
    for(int i = 0; i < numDigits - 1; i++) fac *= 10; 
    for(int i = 0; i < numDigits; i++)
    {
        int digit = (int)str[cursor] - 48; 
        result += fac * digit; 
        fac /= 10; 
        cursor++; 
    }
    //finally flip sign if appropriate
    if(neg) result = -result; 
    return result; 
}

char* std_intToString(int x)
{
    //determine the number of characters needed to represent x 
    int numDigits = 1; 
    {
        int d = 1; 
        int y = x; 
        if(x < 0) y = -x; 
        while(y / d >= 10) 
        {
            numDigits++; 
            d *= 10; 
        }
    }
    //printf("numDigits: %d\n", numDigits); 
    int numChars = numDigits; 
    if(x < 0) numChars++; 
    //printf("numChars: %d\n", numChars); 
    char* str = (char*) core_alloc(numChars+1); 
    //now fill up the string 
    {
        int offset;
        if(x < 0) 
        {
            str[0] = '-'; 
            offset = 1;
        } else offset = 0; 
        int d = 1;
        for(int i = 0; i < numDigits - 1; i++) d *= 10; 
        int y = x; 
        if(x < 0) y = -x; 
        int count = 0; 
        for(int i = 0; i < numDigits; i++)
        {
            int a = y / d;
            str[offset+i] = (char)((y / d) + 48); 
            y = y - a * d; 
            d /= 10;
            count++; 
        }
    }     
    str[numChars] = '\0'; 
    return str; 
}

void std_printIntLn(int x)
{
    //printf("printf %d\n", x); 
    char* str = std_intToString(x); 
    //printf("%li\n", str); 
    std_printLn(str); 
    //std_dealloc(str); 
}

int std_readInt(int maxDigits)
{
    char* buffer = (char*) core_alloc(maxDigits);
    std_readLn(buffer);
    return std_stringToInt(buffer);      
}

char std_intToChar(int x)
{
    return (char) x;
}

char* std_escape(char* x)
{
    int n = std_strLen(x);
    char* y = (char*) std_alloc(n+1); 
    int i = 0; 
    int j = 0; 
    while(i < n)
    {
        if(x[i] == '\\' && x[i+1] == 'n')
        {
            y[j] = 10;
            i += 2;
            j += 1;
        }
        if(x[i] == '\\' && x[i+1] == 't')
        {
            y[j] = 9;
            i += 2;
            j += 1;
        }
        if(x[i] == '\\' && x[i+1] >= 48 && x[i+1] <= 57)
        {
            y[j] = (char) std_stringToInt(x+i+1);
            i++;
            while(x[i] >= 48 && x[i] <= 57) i++;
            j += 1;
        }

        else
        {
            y[j] = x[i];
            i += 1;
            j += 1;
        }
    }
    y[j] = '\0';
    return y;
}

void* std_cast(void* ptr) 
{
    return ptr; 
}

long std_intToLong(int x)
{
    return x;
}

size_t std_intToSize(int x)
{
    return (size_t) x;
}

float std_intToFloat(int x)
{
    return (float) x; 
}


