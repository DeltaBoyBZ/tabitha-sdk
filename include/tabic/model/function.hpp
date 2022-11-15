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

#include "datatypes.hpp"

#include"type.hpp"
#include"statement.hpp"

#include "llvm/IR/Function.h" 

typedef enum FunctionParseStatus
{
    FUNCTION_PARSE_INCOMPLETE, FUNCTION_PARSE_COMPLETE 
} FunctionParseStatus; 

/** @brief Data common to all elements of the union Function. 
 *
 * All functions have at least:
 * - a corresponding llvm::Function*, which we call `llvmFunction`. 
 * - a Type* of value which the function returns when called, which we call `returnType`.    
 * - a `fullName`, as it appears in LLVM modules
 */
struct tabic::FunctionCommon
{
    FunctionClass functionClass = FUNCTION_NONE;
    FunctionParseStatus parseStatus = FUNCTION_PARSE_INCOMPLETE;

    struct
    {
        Type* returnType = nullptr; 
    } parse; 

    struct
    {
        std::string fullName = "";
        llvm::Function* llvmFunction = nullptr; 
    } build; 
}; 

/** @brief A function as defined in a Tabitha slab. 
 *
 * They explicitly belong to a single `hostSlab`. 
 * They have a `mainBlock`. 
 * They may or may not have `arguments`, and may or may not return a value with a `returnType`. 
 */
struct tabic::TabithaFunction
{
    FunctionCommon common; 

    struct {
        Slab* hostSlab = nullptr; ///< The Slab to which this function belongs. 
        std::string name = ""; ///< The name by which this function is refered within the slab.  
        ASTNode node = nullptr; ///< The PEG AST node which defines this function. 
    } create;

    struct
    {
        Block* mainBlock = nullptr;                 ///< The Block which contains all other Block in the TabithaFunction.
        std::vector<Context*> captures = {};        ///< The list of Context which the TabithaFunction captures (and may therefore use). 
        std::vector<StackedVariable*> args = {};    ///< The list of arguments, which are effectively StackedVariable. 
    } parse; 

    struct 
    {
        llvm::Value* stackState = nullptr; 
    } build;

    TabithaFunction(ASTNode node, Slab* hostSlab)
    {
        common.functionClass = FUNCTION_TABITHA; 
        create.node = node; 
        create.hostSlab = hostSlab;
    }

    ~TabithaFunction()
    {
        cleanBlock(parse.mainBlock); 
    }
};

/** @brief A function defined in an object linked with the executable. 
 *
 * These are not defined in the Bundle itself. 
 */
struct tabic::ExternalFunction
{
    FunctionCommon common; 

    struct
    {
        Slab* hostSlab = nullptr;           ///< The Slab in which this ExternalFunction is declared. 
        std::string internalName = "";      ///< The name by which this ExternalFunction is referenced within the Slab. 
        ASTNode node = nullptr;             ///< The ASTNode which defines the ExternalFunction declaration. 
    } create;

    struct
    {
        std::string externalName = "";      ///< The name of the function in its object definition. 
        std::vector<Type*> args = {};       ///< The list of Type which describe the arguments of the ExternalFunction.
    } parse; 

    ExternalFunction(ASTNode node, Slab* hostSlab)
    {
        common.functionClass = FUNCTION_EXTERNAL; 
        create.node = node; 
        create.hostSlab = hostSlab; 
    }

    ~ExternalFunction()
    {
    }
};

/** @brief A function which assists in Tabitha's core functionality. 
 *
 * There are a limited number of these, as they are not designed to be created
 * or referenced by users. 
 */
struct tabic::CoreFunction
{
    struct
    {
        std::string name = "";      ///< The name of the CoreFunction. 
    } create; 

    struct
    {
    } parse; 

    struct
    {
        llvm::FunctionType* functionType = nullptr;     ///< The LLVM FunctionType corresponding to the function. 
    } build;

    CoreFunction(std::string name)
    {
        create.name = name; 
    }

    ~CoreFunction()
    {
    }
};

/** @brief The in-memory representative of a function. 
 *
 * Functions come in three forms: Tabitha Functions, External Functions, and Core Functions. 
 */
union tabic::Function
{
    FunctionCommon common; 
    TabithaFunction tabitha; 
    ExternalFunction external; 

    void destroy()
    {
        //delete the return type if it is NOT top-level
        if(common.parse.returnType->common.create.topLevel) common.parse.returnType->destroy();
        //delete the corresponding llvm Function
        if(common.build.llvmFunction) delete common.build.llvmFunction;
        //The deletion of a Function is sensitive to its common.functionClass
        switch(common.functionClass)
        {
            case FUNCTION_TABITHA:
                delete (TabithaFunction*) this; 
                break;
            case FUNCTION_EXTERNAL:
                delete (ExternalFunction*) this;
                break;
            case FUNCTION_CORE:
                delete (CoreFunction*) this; 
                break;
            default:
                break;
        }
    }
};

namespace tabic
{
    class TabiCore
    {
        public:
            static CoreFunction TABLE_INIT;         ///< Corresponds to `core_table_init`.
            static CoreFunction TABLE_INSERT;       ///< Corresponds to `core_table_insert`.
            static CoreFunction TABLE_GET_BY_ID;    ///< Corresponds to `core_table_getRowByID`.
            static CoreFunction TABLE_DELETE_BY_ID; ///< Corresponds to `core_table_deleteRowByID`.
            static CoreFunction TABLE_GET_NUM_USED; ///< Corresponds to `core_table_getNumUsed`.
            static CoreFunction TABLE_CRUNCH;       ///< Corresponds to `core_table_crunch`.
            static CoreFunction ALLOC;              ///< Corresponds to `core_alloc`.
            static CoreFunction DEALLOC;            ///< Corresponds to `core_dealloc`.
            static CoreFunction MEMCPY;             ///< Corresponds to `core_memcpy`. 
            static CoreFunction SUBVECTOR_COPY;     ///< Corresponds to `core_subvector_copy`
    }; 

    class LLVMIntrinsic
    {
        public:
            static llvm::Function* STACKSAVE; 
            static llvm::Function* STACKRESTORE;
    };
}

