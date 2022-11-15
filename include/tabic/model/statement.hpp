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

#include<vector>

/** @brief Data common all all elements of the union Statement. 
 */
struct tabic::StatementCommon
{
    StatementClass statementClass = STATEMENT_NONE;

    struct
    {
        Block* hostBlock = nullptr;         ///< The Block which contains the Statement.
        TabithaFunction* hostFunction = nullptr;   ///< The Function which contains the Statement.
        ASTNode node;                       ///< The PEG AST node which defines the Statement.
    } parse;
};

/** @brief A collection of statmements. 
 *
 * Blocks define their own scope, which is captured by its children but
 * inaccessible to its parents. 
 */
struct tabic::Block
{
    StatementCommon common; 
    struct
    {
        std::vector<Statement*> statements = {};
        std::map<std::string, Variable*> variables; 
    } parse;

    Block(ASTNode node, Block* parentBlock, TabithaFunction* hostFunction)
    {
        common.statementClass = STATEMENT_BLOCK;
        common.parse.node = node; 
        common.parse.hostBlock = parentBlock; 
        common.parse.hostFunction = hostFunction; 
    }
}; 
/** @brief A Statement which gives the caller of common.parse.hostFunction 
 * a value.
 */
struct tabic::Return
{
    StatementCommon common; 

    struct
    {
        Expression* expression = nullptr; 
    } parse;

    Return(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_RETURN;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
    }
};

/** @brief A Statement which creates a stack-allocated variable, i.e. StackedVariable, within
 * the current Block scope. 
 */
struct tabic::StackedDeclaration
{
    StatementCommon common; 

    struct
    {
        StackedVariable* variable = nullptr;    ///< The StackedVariable this StackedDeclaration declares.
        Expression* initialiser = nullptr;      ///< The Expression which is first assigned to the StackedVariable.
    } parse;

    StackedDeclaration(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_STACKED_DECLARATION;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction;
    }
};

/** @brief A Statement which creates a heap-allocated variable, i.e. HeapedVariable.
 */ 
struct tabic::HeapedDeclaration
{
    StatementCommon common; 

    struct
    {
        HeapedVariable* variable = nullptr;     ///< The HeapedVariable this HeapedVariable declares. 
        Expression* initialiser = nullptr;      ///< The Expression which is first assigned to the HeapedVariable. 
    } parse;

    HeapedDeclaration(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_HEAPED_DECLARATION;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
    }
};

/** @brief A Statement which stores an Expression value in an already allocated variable.
 */
struct tabic::Assignment
{
    StatementCommon common; 

    struct
    {
        ValueRef* ref = nullptr;       ///< The ValueRef to which the value is being assigned. 
        Expression* expression = nullptr;   ///< The Expression representing the value being assigned. 
    } parse;

    Assignment(Block* hostBlock)
    {
        common.statementClass = STATEMENT_ASSIGNMENT;
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
    }
};

/** @brief An Expression which dictates whether or not an associated Block should be executed. 
 */
struct tabic::ConditionBlockPair
{
    Expression* expression = nullptr; 
    Block* block = nullptr; 
};

/** @brief A Statement which executes a child Block which executes if a condition is true. 
 */
struct tabic::Conditional
{
    StatementCommon common;

    struct
    {
        ConditionBlockPair pair;    ///< The ConditionBlockPair which makes up the Conditional.  
    } parse; 

    Conditional(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_CONDITIONAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock;
        common.parse.hostFunction = hostBlock->common.parse.hostFunction;
    }
};

/** @brief A Statement consisting of pairs of Expression and Block, along with an optional `otherwise` Block. 
 *
 * At most one Block will execute depending on the truth of the conditions.
 * If no conditions are met, and an `otherwise` Block is present, it is that block which executes.
 */
struct tabic::Branch
{
    StatementCommon common; 

    struct
    {
        std::vector<ConditionBlockPair> twigs = {};     ///< The collection of possible Block to execute in the Branch, along with their associated condition. 
        Block* otherwiseBlock = nullptr;                ///< The Block which executes if all conditions fail.
    } parse;

    Branch(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_BRANCH;
        common.parse.node = node;
        common.parse.hostBlock = hostBlock;
        common.parse.hostFunction = hostBlock->common.parse.hostFunction;
    }
};

/** @brief A Statement consisting of a Block is executes repeatedly so long as a condition is met.
 *
 * It is possible for the statement to execute zero times. 
 */
struct tabic::Loop
{
    StatementCommon common; 

    struct
    {
        Block* directions = nullptr;        ///< The Block which is executed when the condition is met.
        Expression* condition = nullptr;    ///< The Expression whose value decides whether the Block is executed. 
    } parse; 

    Loop(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_LOOP;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock;
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
    }

    ~Loop()
    {
        delete parse.directions;
    }
};

/** @brief A Statement which calls a Function but does not store the return value. 
 */
struct tabic::ProcedureCall
{
    StatementCommon common; 
    struct
    {
        Function* callee = nullptr;         ///< The Function which gets called. 
        std::vector<Expression*> args = {}; ///< The arguments which are passed to the Function.
    } parse; 

    ProcedureCall(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_PROCEDURE_CALL;
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction;
        common.parse.node = node; 
    }
}; 

/** @brief A Statement which inserts a row of values into a table. 
 */
struct tabic::TableInsert
{
    StatementCommon common; 
    struct
    {
        ValueRef* tableRef = nullptr;               ///< The table in which to insert. 
        std::vector<Expression*> elements = {};     ///< The elements to be inserted. 
        ValueRef* idRef = nullptr;                  ///< The variable in which to store the ID. 
    } parse; 

    TableInsert(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_TABLE_INSERT; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
        common.parse.node = node; 
    }
};

/** @brief A Statement which deletes a row by its ID.
 */
struct tabic::TableDelete
{
    StatementCommon common;
    struct
    {
        ValueRef* tableRef = nullptr;               ///< The table from which to delete.  
        Expression* id = nullptr;                   ///< The ID of the row to be deleted.  
    } parse;

    TableDelete(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_TABLE_DELETE;
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
        common.parse.node = node; 
    }
};

/** @brief A Statement which measures the number of used rows in a table. 
 */
struct tabic::TableMeasure
{
    StatementCommon common; 
    struct
    {
        ValueRef* tableRef = nullptr; 
        ValueRef* usedRef  = nullptr; 
    } parse;

    TableMeasure(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_TABLE_MEASURE;
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
        common.parse.node = node; 
    }
}; 

/** @brief A Statement which pushes all used rows to the top of the table. 
 */
struct tabic::TableCrunch
{
    StatementCommon common; 
    struct
    {
        ValueRef* tableRef = nullptr; 
        ValueRef* idRef = nullptr; 
    } parse;

    TableCrunch(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_TABLE_CRUNCH; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
        common.parse.node = node; 
    }
};

/** @brief A Statement which sets a subset of some vector's elements. 
 */
struct tabic::VectorSet
{
    StatementCommon common; 

    struct
    {
        ValueRef* vectorRef = nullptr;              ///< The vector whose elements we are setting. 
        Expression* from = nullptr;                 ///< The element from which we are setting. 
        std::vector<Expression*> elements = {};     ///< The expressions to set the vector elements to. 
    } parse; 

    VectorSet(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_VECTOR_SET; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction; 
        common.parse.node = node; 
    }
}; 

/** @brief A Statement which ties a fuzzy vector to an address. 
 */
struct tabic::Label
{
    StatementCommon common;

    struct
    {
        Expression* address = nullptr; 
        ValueRef* fuzzyRef = nullptr; 
    } parse; 

    Label(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_LABEL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction;
    }
}; 

/** @brief A Statement which deallocates a heaped variable. 
 */
struct tabic::Unheap
{
    StatementCommon common; 

    struct
    {
        Expression* address = nullptr;      ///< The address to deallocate. 
        Type* structure = nullptr;          ///< The Type according to which we deallocate.
    } parse; 

    Unheap(ASTNode node, Block* hostBlock)
    {
        common.statementClass = STATEMENT_UNHEAP; 
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        common.parse.hostFunction = hostBlock->common.parse.hostFunction;
    }
}; 

/** @brief Acts as a superstruct for all forms of Statement.
 */
union tabic::Statement
{
    StatementCommon common; 
    Block block; 
    Return ret;
    StackedDeclaration localDeclaration; 
    HeapedDeclaration heapedDeclaration; 
    Assignment assignment; 
    Conditional conditional;
    Branch branch; 
    Loop loop;
    ProcedureCall procedureCall; 
    VectorSet vectorSet;
    TableInsert tableInsert; 
    TableDelete tableDelete; 
    TableMeasure tableMeasure; 
    TableCrunch tableCrunch; 
    Label label; 
    Unheap unheap; 

    void destroy()
    {
        switch(common.statementClass)
        {
            case STATEMENT_BLOCK:
                std::cout << "block" << std::endl;
                delete (Block*) this; 
                break;
            case STATEMENT_RETURN:
                std::cout << "return" << std::endl;
                delete (Return*) this; 
                break;
            case STATEMENT_STACKED_DECLARATION:
                std::cout << "stacked" << std::endl;
                delete (StackedDeclaration*) this;
                break;
            case STATEMENT_HEAPED_DECLARATION:
                std::cout << "heaped" << std::endl;
                delete (HeapedDeclaration*) this; 
                break;
            case STATEMENT_ASSIGNMENT:
                std::cout << "assignment" << std::endl;
                delete (Assignment*) this; 
                break;
            case STATEMENT_CONDITIONAL:
                std::cout << "conditional" << std::endl;
                delete (Conditional*) this;
                break;
            case STATEMENT_PROCEDURE_CALL:
                std::cout << "procedure" << std::endl;
                delete (ProcedureCall*) this;
                break;
            case STATEMENT_BRANCH:
                std::cout << "branch" << std::endl;
                delete (Branch*) this;
                break;
            case STATEMENT_LOOP:
                std::cout << "loop" << std::endl;
                delete (Loop*) this; 
                break; 
            default:
                break;
        }
    }
};

namespace tabic
{
    /** @brief Acts as a destructor for Block.
     *
     * It is necessary to define this here, because Block takes ownership of Statament, which is
     * incomplete at the time of its definition. 
     */
    void cleanBlock(Block* block);
}

