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

#include"datatypes.hpp"

#include"llvm/IR/Value.h"

/** @brief Data common to all elements of the union Variable.
 *
 * Any Variable must at least have:
 * - An llvm::Value corresponding to its memory location, called `llvmStore`. 
 * - A corresponding Type to determine its data structure, called `type`.
 * - A std::string name by which it is referenced, called `name`.  
 */
struct tabic::VariableCommon
{
    VariableClass variableClass = VARIABLE_NONE; 

    struct
    {
        std::string name = "";  ///< The name by which this variable can be referenced. 
        Type* type = nullptr;   ///< The Type which dictates the data structure.  
    } parse; 

    struct
    {
        llvm::Value* llvmStore = nullptr; 
    } build; 
};

/** @brief A Variable which is allocated on the stack. 
 *
 * A StackedVariable is only accessible within the Block in which they are declared. 
 */
struct tabic::StackedVariable
{
    VariableCommon common; 

    struct
    {
        Block* hostBlock = nullptr; ///< The Block in which the StackedVariable is declared. 
    } parse;

    StackedVariable(Block* hostBlock)
    {
        common.variableClass = VARIABLE_STACKED; 
        parse.hostBlock = hostBlock; 
    }

    ~StackedVariable() {}
}; 

/** @brief A Variable which is allocated on the heap.
 *
 * A HeapedVariable is accessible anywhere, so long as the memory address is known. 
 * It cannot however be referenced by name outside of its initial scope. 
 */
struct tabic::HeapedVariable
{
    VariableCommon common; 

    struct
    {
        Block* hostBlock = nullptr; ///< The Block in which the HeapedVariable is declared. 
    } parse; 

    HeapedVariable(Block* hostBlock)
    {
        common.variableClass = VARIABLE_HEAPED; 
        parse.hostBlock = hostBlock;
    }

    ~HeapedVariable() {}
};

/** @brief A Variable which exists within a Context.
 *
 * A ContextVariable is accessible within any Function which has captured the corresponding Context.
 */
struct tabic::ContextVariable
{
    VariableCommon common; 

    struct
    {
        Context* hostContext = nullptr;     ///< The Context to which the ContextVariable belongs.
        Expression* initialiser = nullptr;  ///< The Expression which sets the initial value of the ContextVariable.
                                            ///This Expression must be static. 
    } parse; 

    struct
    {
        std::string fullName = "";          ///< The name by which this variable is referenced. 
        std::vector<Slab*> declaredIn = {}; ///< List of Slab for which the associated LLVM Module contains a declaration of the ContextVariable. 
                                            ///Declaring variables in multiple modules ensures the modules can be disassembled. 
        int used = 0; 
    } build; 

    ContextVariable(Context* hostContext)
    {
        common.variableClass = VARIABLE_CONTEXT;
        parse.hostContext = hostContext; 
    }

    ~ContextVariable() {}
};


/** @brief A Variable which exists within a Context.
 *
 * A ContextVariable is accessible within any Function which has captured the corresponding Context.
 */
struct tabic::DumpVariable
{
    VariableCommon common; 

    struct
    {
        Dump* hostDump = nullptr;           ///< The Dump to which this DumpVariable belongs.
        Expression* initialiser = nullptr;  ///< The Expression which sets the initial value of the DumpVariable.
                                            ///This Expression must be static. 
    } parse; 

    struct
    {
        std::string fullName = "";
        std::vector<Slab*> declaredIn = {}; ///< List of Slab for which the associated LLVM Module contains a declaration of the ContextVariable. 
                                            ///Declaring variables in multiple modules ensures the modules can be disassembled. 
    } build; 

    DumpVariable(Dump* hostDump)
    {
        common.variableClass = VARIABLE_DUMP;
        parse.hostDump = hostDump; 
    }

    ~DumpVariable() {}
};

/** @brief Data common to all elements of the union ValueRef. 
 */
struct tabic::ValueRefCommon
{
    ValueRefClass valueRefClass = VALUE_REF_NONE; 
    struct
    {
        ValueRef* parent = nullptr;      ///< The parent reference to this one. 
        Type* type = nullptr;            ///< The Type of the value we reference. 
        bool query = false;              ///< Whether or not the query operator `@` has been used.                     
    } parse;

    struct
    {
        llvm::Value* llvmStore = nullptr;   ///< The LLVM Value representing the memory location of the value we reference. 
    } build;
}; 

/** @brief A ValueRef representing a variable. 
 */
struct tabic::VariableRef
{
    ValueRefCommon common; 

    struct
    {
        Variable* variable = nullptr;       ///< The Variable associated with the VariableRef.
        Slab* hostSlab = nullptr;           ///< The Slab in which the VariableRef appears. 
    } parse; 

    VariableRef(Slab* hostSlab)
    {
        common.valueRefClass = VALUE_REF_VARIABLE; 
        parse.hostSlab = hostSlab;
    }
};

/** @brief A ValueRef representing the member of a CollectionType. 
 */
struct tabic::MemberRef
{
    ValueRefCommon common;

    struct
    {
        std::string memberName = "";        ///< The name of the referenced member. 
        int memberIndex = -1;               ///< The associated index in the master type. 
    } parse;

    MemberRef(ValueRef* parent)
    {
        common.valueRefClass = VALUE_REF_MEMBER; 
        common.parse.parent = parent;
    }
}; 

/** @brief A ValueRef representing the element of a VectorType.
 */
struct tabic::ElementRef
{
    ValueRefCommon common;

    struct
    {
        Expression* index = nullptr;        ///< Expression representing the index of the referenced element. 
    } parse; 

    ElementRef(ValueRef* parent)
    {
        common.valueRefClass = VALUE_REF_ELEMENT; 
        common.parse.parent = parent;
    }
};

/** @brief A ValueRef representing the row of a table with a particular ID. 
 */
struct tabic::RowRef
{
    ValueRefCommon common; 

    struct
    {
        std::string fieldName = "";         ///< Name of the table field. 
        Expression* id = nullptr;           ///< Expression representing the ID of the referenced row. 
        int fieldIndex = -1;                ///< Index for the referenced field. 
    } parse; 

    RowRef(ValueRef* parent)
    {
        common.valueRefClass = VALUE_REF_ROW; 
        common.parse.parent = parent;
    }
}; 

/**
 */
union tabic::ValueRef
{
    ValueRefCommon common; 
    VariableRef variable; 
    MemberRef member; 
    ElementRef element; 
    RowRef row; 
}; 

/** @brief Acts as a superstruct for all forms of Variable. 
 */
union tabic::Variable
{
    VariableCommon common;
    StackedVariable stacked; 
    HeapedVariable heaped; 
    ContextVariable context; 
    DumpVariable dump;
    void destroy()
    {
        switch(common.variableClass)
        {
            case VARIABLE_STACKED:
                delete (StackedVariable*) this;
                break;
            case VARIABLE_HEAPED:
                delete (HeapedVariable*) this;
                break;
            case VARIABLE_CONTEXT:
                delete (ContextVariable*) this;
                break;
            default:
                break;
        }
    }
}; 
