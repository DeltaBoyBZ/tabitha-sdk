/*
Copyright 2022 Matthew Peter Smith

This file is provided under the terms of the Mozilla Public License - Version 2.0. 
A copy of the licence can be found in the root of The Tabitha SDK GitHub repository,

https://github.com/DeltaBoyBZ/tabitha-sdk 

or alternatively a copy can be found at,

https://www.mozilla.org/media/MPL/2.0/index.815ca599c9df.txt

*/

void* core_alloc(long numBytes); 
void  core_memcpy(void* dest, void* src, long numBytes); 

/** @brief Initialised an allocated table by setting all of the 'use' field to zero. 
 */
void core_table_init(void** table, int numRows)
{
    int* idField = *(int**)table; 
    int* useField = *((int**)table + 1);
    for(int i = 0; i < numRows; i++) useField[i] = 0; 
}

/** @brief Figures out the smallest unused id and inserts it into the first unused row. 
 *
 * Returns the inserted row number, and stores the id in an argument. 
 */
int core_table_insertRow(void** table, int numRows, int* id)
{
    int* idField = *(int**)table; 
    int* useField = *((int**)table + 1);
    int newRow = -1; 
    int count[numRows]; 
    for(int i = 0; i < numRows; i++) count[i] = 0; 
    for(int i = 0; i < numRows; i++)
    {
        if(newRow == -1 && useField[i] == 0) newRow = i; 
        if(useField[i] == 1) count[idField[i]]++; 
    }
    if(newRow == -1) return newRow;
    int id0; 
    for(int i = 0; i < numRows; i++)
    {
        if(count[i] == 0) 
        {
            id0 = i; 
            break;
        }
    }
    if(id) *id = id0; 
    idField[newRow] = id0; 
    useField[newRow] = 1; 
    return newRow;
}

/** @brief Returns the row associated with the given id, or returns the first free row (now with the correct id) if not found. 
 */
int core_table_getRowByID(void** table, int numRows, int id)
{
    int* idField  = *(int**)table; 
    int* useField = *((int**)table + 1);
    int newRow = -1; 
    for(int i = 0; i < numRows; i++)
    {
        if(newRow == -1 && useField[i] == 0) newRow = i; 
        if(useField[i] == 1 && idField[i] == id) return i; 
    }
    if(newRow == -1) return newRow; 
    idField[newRow] = id; 
    useField[newRow] = 1; 
    return newRow; 
}

/** @brief Deletes the row of a given ID from the table (if it even exists). 
 *
 * To delete a row, is to indicate that the row is not being used.
 */
void core_table_deleteRowByID(void** table, int numRows, int id)
{
    int* idField  = *(int**)table; 
    int* useField = *((int**)table + 1);
    for(int i = 0; i < numRows; i++)
    {
        if(id == idField[i]) 
        {
            useField[i] = 0; 
        }
    }
}

/** @brief Counts the number of used rows in a table. 
 */ 
int core_table_getNumUsed(void** table, int numRows)
{
    int* useField = *((int**)table + 1);
    int numUsed = 0; 
    for(int i = 0; i < numRows; i++)
    {
        if(useField[i] == 1) numUsed++;
    }
    return numUsed;
}

/** @brief Moves all used rows to the top of the table.
 */
void core_table_crunch(void** table, int numRows, int numFields, int* fieldSizes, int* topmost)
{
    int numUsed = core_table_getNumUsed(table, numRows);  
    if(numUsed >= numRows) return;
    int* idField  = *(int**)table; 
    int* useField = *((int**)table + 1);
    void* fields[numFields]; 
    for(int i = 0; i < numFields; i++) 
    {
        fields[i] = (void*)*((int**)table + 2 + i); 
    }
    for(int i = 0; i < numUsed; i++)
    {
        if(useField[i] == 0)
        {
            for(int j = numUsed; j < numRows; j++)
            {
                if(useField[j] == 1)
                {
                    idField[i] = useField[j];
                    useField[i] = useField[j];
                    useField[j] = 0; 
                    for(int k = 0; k < numFields; k++) core_memcpy(fields[k] + fieldSizes[k]*i, fields[k] + fieldSizes[k]*j, fieldSizes[k]); 
                    break;
                }
            }
        }
    }
    if(topmost) *topmost = idField[0]; 
}


/** @brief Makes copies of a vectors elements (which are themselves vectors) and replaces the original elements with the copies. 
 *
 * This is called when passing e.g. a variable of type Vec[Vec[Int, 5], 5] by values.
 *
 * @param src The pointer to elements of the vector whose elements we are replacing with copies. 
 * @param ptrSize The size of the memory address to the vector pointer. 
 * @param elemSize The size of the elements of the subvector
 * @param numPtr The number of vector pointers in the main vector. 
 * @param numElem The number of elements in each subvector. 
 */
void core_subvector_copy(void*** vec, int ptrSize, int elemSize, int numPtr, int numElem)
{
    for(int i = 0; i < numPtr; i++)
    {
        void** subVecPtr = *(vec + i*ptrSize); 
        void*  subVec    = *subVecPtr;
        void* subVecCopy = core_alloc(elemSize*numElem); 
        core_memcpy(subVecCopy, subVec, elemSize*numElem); 
        *subVecPtr = subVecCopy; 
    }
}
