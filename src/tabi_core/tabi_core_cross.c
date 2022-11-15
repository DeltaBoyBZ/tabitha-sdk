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
#include<memory.h>

void _tabi_init(); 
void _tabi_destroy(); 
int _tabi_main();

/** @brief Allocates memory on the heap. 
 * 
 * @param numBytes The size (in bytes) of the memory to be allocated.
 */
void* core_alloc(int numBytes)
{
    return malloc(numBytes);
}

/** brief Frees memory from the heap.
 *
 * @param ptr The address of the memory to be freed.
 * @param numBytes The size of the memory to be freed. Ignored in the cstdlib implementation of tabi_core.
 */
void core_dealloc(void* ptr)
{
    free(ptr); 
}

void core_memcpy(void* dest, void* src, int numBytes)
{
    memcpy(dest, src, numBytes);
}

int main()
{
    _tabi_init();
    int code = _tabi_main();  
    _tabi_destroy(); 
    return code;
}


