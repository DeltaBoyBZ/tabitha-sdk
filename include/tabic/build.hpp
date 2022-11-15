/*
Copyright 2022 Matthew Peter Smith

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include"tabic/model/model.hpp"

namespace tabic
{
    /** @brief Builds the given Bundle.
     *
     * To build a Bundle, is to construct all of the LLVM Modules which correspond to it.
     *
     * @param bundle The Bundle to be built.
     */
    void buildBundle(Bundle* bundle);

    /** @brief Build the given CollectionType.
     *
     * @param type The CollectoinType to be built.
     */
    void buildCollectionType(CollectionType* type);

    /** @brief Build the give Type, no matter what it is.
     *
     * Also skips the building process if it appears the Type is built already.
     * @param type The Type to be built.
     */
    void buildType(Type* type);

    /** @brief Build the given TabithaFunction.
     *
     * @param function The TabithaFunction to be built. 
     */
    void buildTabithaFunction(TabithaFunction* function);

    /** @brief Allocates all stack variables in this block and any sub-block.
     *
     * @param block The Block to scan for variable.
     */
    void allocateStackVariables(Block* block);

    /** @brief Allocates stack memory to store a given Type and returns the address. 
     *
     * @param type The Type according to which the memory is to be structured. 
     * @param hostFunction The TabithaFunction in which the data is declared. 
     * @param name The name of the variable (empty if it is a sub object). 
     */
    llvm::Value* allocateStackType(Type* type, TabithaFunction* hostFunction, std::string name = "");

    /** @brief Allocates the memory taken up by stack vector elements (this cannot be done in the stack_alloc block because the number of elements is evaluated at runtime).
     * 
     * @param vecType - The VectorType of the variable to which the elements belong. 
     * @param hostFunction - The TabithaFunction in which the vector is declared.
     */
    llvm::Value* allocateStackVectorElements(Type* vecType, TabithaFunction* hostFunction);

    /** @brief Allocates the memory taken up by stack table fields. 
     *
     * @param tableType - The TableType of the variable to which the fields belong. 
     * @param hostFunction - The TabithaFunction
     */
    void allocateStackTableFields(Type* tableType, TabithaFunction* hostFunction, llvm::Value* store);

    /** @brief Allocates the vectors buried within a stack-allocated collection type. 
     *
     * This needs to be a function because the search is recursive.
     *
     * @param collectionType The CollectionType whose members we scan for vectors (and other collection types). 
     * @param store The `llvmStore` associated with the data of Type \p collectionType.
     */
    void allocateStackSubvectors(CollectionType* collectionType, llvm::Value* store, TabithaFunction* hostFunction);

    /** @brief Allocates heap memeory to store a given Type and returns the address. 
     *
     * @param type The Type according to which the memory is to be structured. 
     * @param hostFunction The TabithaFunction in which the data is declared.
     * @param name The name of the variable (empty if it is a sub object). 
     */
    llvm::Value* allocateHeapType(Type* type, TabithaFunction* hostFunction, std::string name = ""); 

    void allocateContextType(Type* type, Slab* hostSlab, llvm::Value* store, std::string name = ""); 

    /** @brief Allocates to the stack the handles to heap variables declared in the block and its children. 
     *
     * This is essentially invisible to the programmer, but it is vital to do things this way because of loops etc.
     */
    void allocateHeapHandles(Block* block);

    /** @brief Allocates the vectors buried within a heap-allocated collection type.
     *
     * This needs to be a function because the search is recursive. 
     *
     * @param collectionType The CollectionType whose members we scan for vectors (and other collection types). 
     * @param store The `llvmStore` associated with the data of Type \p collectionType
     * @param hostFunction The TabithaFunction which hosts the data. 
     */
    void allocateHeapSubvectors(CollectionType* collection, llvm::Value* store, TabithaFunction* hostFunction); 

    void allocateContextSubvectors(CollectionType* collection, llvm::Value* store, Slab* hostSlab);

    /** @brief Creates a stack copy of a vector. 
     */
    llvm::Value* copyVector(Type* type, llvm::Value* arg, TabithaFunction* hostFunction); 

    /** @brief
     */
    void copySubvectors(Type* type, llvm::Value* store, TabithaFunction* hostFunction);

    /** @brief Builds the given HeapedDeclaration.
     *
     * To build a HeapedDeclaration, is to allocate the associated HeapedVariable.
     * 
     * @param heapedDeclaration The HeapdedDeclaration to be built. 
     */
    void buildHeapedDeclaration(HeapedDeclaration* heapedDeclaration);
   
    /** @brief Register the given Function.
     *
     * To register a function, is to create the associated LLVM Function. 
     * This step is necessarily separate to Function building, because functions reference eachother. 
     */
    void registerFunction(Function* function);

    /** @brief Build the given Context.
     *
     * @param context The Context to be built.
     */
    void buildContext(Context* context); 

    /** @brief Build the given Dump.
     *
     * @param dump The Dump to be built. 
     */
    void buildDump(Dump* dump); 

    /** @brief Builds the given Expression.
     *
     * To build an Expression, is to get its corresponding LLVM value. 
     *
     * @param expression The Expression to be built.
     */
    void buildExpression(Expression* expression);

    /** @brief Build the given Block.
     *
     * @param block THe Block the be built.
     */
    void buildBlock(Block* block); 

    /** @brief Build the given Conditional. 
     *
     * @param conditional The Conditional to be built. 
     */
    void buildConditional(Conditional* conditional);

    /** @brief Build the given Branch.
     *
     * @param branch The Branch to be built. 
     */
    void buildBranch(Branch* branch);

    /** @brief Build the given Loop. 
     *
     * @param loop the Loop to be built. 
     */
    void buildLoop(Loop* loop);

    /** @brief Build the given ValueRef.
     *
     * To build a ValueRef, is to establish its corresponding common.build.llvmStore. 
     *
     * @param valueRef The ValueRef to be built. 
     */
    void buildValueRef(ValueRef* valueRef, ValueRef* fine); 

    /** @brief Build the given VectorSet. 
     *
     * @param vectorSet The VectorSet to be built. 
     */
    void buildVectorSet(VectorSet* vectorSet);

    /** @brief Build the given TableInsert.
     *
     * @param tableInsert The TableInsert to be built.
     */
    void buildTableInsert(TableInsert* tableInsert); 

    /** @brief Build the given TableDelete. 
     *
     * @param tableDelete The TableDelete to be built.
     */
    void buildTableDelete(TableDelete* tableDelete);

    /** @brief Build the given TableMeasure. 
     *
     * @param tableMeasure The TableMeasure to be built.
     */
    void buildTableMeasure(TableMeasure* tableMeasure); 

    /** @brief Build the given TableCrunch. 
     *
     * @param tableCrunch The TableCrunch to be built.
     */
    void buildTableCrunch(TableCrunch* tableCrunch); 

    /** @brief Builds the given Label.
     *
     * @param label The Label to be built. 
     */
    void buildLabel(Label* label); 

    /** @brief Builds the given Unheap.
     *
     * @param unheap The Unheap to be built.
     */
    void buildUnheap(Unheap* unheap);

    /** @brief Deallocate the memory corresponding to the given Type. 
     *
     * @param type - The type according to which the data should be deallocated.
     * @param store - The memory address of the data to be deallocated. 
     * @param hostFunction - The Function in which the relevant `unheap` statement appears. 
     */
    void deallocType(Type* type, llvm::Value* store, Function* hostFunction, bool deallocBase = true); 

    /** @brief Deallocates the memory used by subvectors of a CollectionType (as well as any subvectors in sub-collection types). 
     *
     * @param type - The CollectionType which may or may not have vector members. 
     * @param store - The memory address of the variable of type \p type
     * @param hostFunction - The Function in which the relevant `unheap` statement appears. 
     */
    void deallocSubvectors(Type* type, llvm::Value* store, Function* hostFunction); 
}

