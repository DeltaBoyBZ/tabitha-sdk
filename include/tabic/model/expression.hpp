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
#include "type.hpp"
#include "function.hpp"
#include "statement.hpp"

#include "llvm/IR/Value.h"

/** @brief Data common to all elements of the union Expression. 
 *
 * All Expression have at least:
 * - A PEG AST node which defines it, refered to as parse.node
 * - A corresponding llvm::Value, referred to as build.llvmValue.
 * - Either a parse.hostSlab or parse.hostBlock, depending on where this expression is
 * - A Type, which the evaluated Expression takes on. 
 */
struct tabic::ExpressionCommon
{
    ExpressionClass expressionClass;

    struct
    {
        ASTNode node = nullptr;         ///< The ASTNode which defines the Expression.  
        Block* hostBlock = nullptr;     ///< The Block in which this Expression appears. May be `nullptr`. 
        Slab* hostSlab = nullptr;       ///< The Slab in which this Expression appears. 
        Type* type = nullptr;           ///< The Type to which the evaluated Expression belongs. 
    } parse; 

    struct
    {
        llvm::Value* llvmValue = nullptr;   ///< The value this Expression corresponds to within the LLVM framework. 
    } build;
};

/** @brief Represents an integer literal, i.e. an integer stated on its own. 
 *
 * e.g. `3`, `69`
 */
struct tabic::IntLiteral
{
    ExpressionCommon common; 

    struct
    {
        int value = 0;  ///< The value the IntLiteral represents. 
    } parse;

    IntLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_INT_LITERAL; 
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::INT;
    }

    ~IntLiteral() {}
};

/** @brief Represents a float literal, i.e. a floating point number on its own.
 *
 * e.g. `3.14`
 */
struct tabic::FloatLiteral
{
    ExpressionCommon common; 

    struct
    {
        float value = 0.0f;     ///< The value the FloatLiteral represents. 
    } parse; 

    FloatLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_FLOAT_LITERAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::FLOAT;
    }

    ~FloatLiteral() {}
};

struct tabic::ShortLiteral
{
    ExpressionCommon common; 

    struct
    {
        short value = 0; 
    } parse;

    ShortLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_SHORT_LITERAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::SHORT;
    }
};

struct tabic::LongLiteral
{
    ExpressionCommon common;

    struct 
    {
        long value = 0; 
    } parse;

    LongLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_LONG_LITERAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::LONG;
    }
};

struct tabic::DoubleLiteral
{
    ExpressionCommon common;

    struct
    {
        double value = 0.0;
    } parse;

    DoubleLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_DOUBLE_LITERAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock;
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        common.parse.hostSlab = hostSlab;
        common.parse.type = (Type*) &SupportedPrimitives::DOUBLE;
    }
};

struct tabic::SizeLiteral
{
    ExpressionCommon common;

    struct
    {
        size_t value = 0; 
    } parse;

    SizeLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_SIZE_LITERAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock;
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::SIZE; 
    }
};

/** @brief Represents character literal, i.e. a character on its own
 *
 * e.g. `'a'`, `'?'`
 */
struct tabic::CharLiteral
{
    ExpressionCommon common; 

    struct
    {
        char value = '\0';      ///< The value the CharLiteral represents. 
    } parse; 

    CharLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_CHAR_LITERAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        common.parse.hostSlab  = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::CHAR;
    }

    ~CharLiteral(){}
};

/** @brief Represents string literal, i.e. a string of characters on their own. 
 *
 * e.g. "Hello there!"
 */
struct tabic::StringLiteral
{
    ExpressionCommon common; 

    struct
    {
        std::string value = ""; 
    } parse; 
    
    StringLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_STRING_LITERAL; 
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) new AddressType();  
        common.parse.type->address.parse.pointsTo = (Type*) &SupportedPrimitives::CHAR;
    }
};

/** @brief Represents a truth literal, either `true` or `false`. 
 */
struct tabic::TruthLiteral
{
    ExpressionCommon common; 

    struct
    {
        TruthValue value = TRUTH_TRUE;  ///< The value the TruthLiteral represents. 
    } parse; 

    TruthLiteral(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_TRUTH_LITERAL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::TRUTH;
    }

    ~TruthLiteral(){}
}; 

/** @brief Represents the value of a variable via a reference to that variable. 
 *
 * Could also be the variable's memory address, accessed by the location operator `?`.
 */
struct tabic::VariableValue
{
    ExpressionCommon common; 

    struct
    {
        bool locate = false; 
        ValueRef* ref = nullptr; 
    } parse; 

    VariableValue(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_VARIABLE_VALUE; 
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        common.parse.hostSlab = hostSlab; 
    }

    ~VariableValue(){}
}; 

/** @brief An Expression obtained from calling a Function. 
 */
struct tabic::FunctionCall
{
    ExpressionCommon common; 

    struct
    {
        Function* callee = nullptr;             ///< The Function to be called.  
        std::vector<Expression*> args = {};     ///< The arguments passed to the Function.  
    } parse; 

    FunctionCall(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_FUNCTION_CALL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        common.parse.hostSlab  = hostSlab; 
    }

    ~FunctionCall(){}
}; 

struct tabic::BracketedExpression
{
    ExpressionCommon common; 

    struct
    {
        Expression* contents = nullptr; 
    } parse;

    BracketedExpression(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_BRACKETED;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        common.parse.hostSlab = hostSlab; 
    }
};

typedef enum  EquivalentPrimitive
{
    EP_NONE, EP_INT, EP_FLOAT, EP_CHAR
} EquivalentPrimitive; 

/** @brief An Expression consisting of two Expression combined with an operator.
 */
struct tabic::BinaryExpression
{
    ExpressionCommon common;

    struct
    {
        Expression* lhs = nullptr;
        Expression* rhs = nullptr;
        BinaryOperator op = BINARY_OP_NONE; 
        EquivalentPrimitive ep = EP_NONE; 
    } parse;
    
    BinaryExpression(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_BINARY;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock;
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        common.parse.hostSlab = hostSlab;
    }

    ~BinaryExpression(){}
};

struct tabic::NullValue
{
    ExpressionCommon common;

    NullValue(ASTNode node, Block* hostBlock, Slab* hostSlab)
    {
        common.expressionClass = EXPRESSION_NULL;
        common.parse.node = node; 
        common.parse.hostBlock = hostBlock; 
        if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        common.parse.hostSlab = hostSlab; 
        common.parse.type = (Type*) &SupportedPrimitives::NONE; 
    }

    ~NullValue(){}
};

/** @brief Acts as a superstruct for all forms of Expression
 * 
 * An Expression is anything that can be assigned to a variable, passed to a function,
 * or be evaluated for some other purpose such as deciding whether to execute
 * a Conditional.
 *
 * Expressions are evaluated in a tree-like fashion, as they may be combined via operators. 
 * These combinations are treated as being Expression themselves.
 */
union tabic::Expression
{
    ExpressionCommon common; 
    NullValue nullValue; 
    IntLiteral intLiteral; 
    FloatLiteral floatLiteral;
    ShortLiteral shortLiteral;
    LongLiteral longLiteral;
    DoubleLiteral doubleLiteral;
    SizeLiteral sizeLiteral;
    CharLiteral charLiteral;
    StringLiteral stringLiteral;
    TruthLiteral truthLiteral; 
    VariableValue variableValue; 
    FunctionCall functionCall;
    BracketedExpression bracketed;
    BinaryExpression binary;

    void destroy()
    {
        switch(common.expressionClass)
        {
            case EXPRESSION_INT_LITERAL:
                delete (IntLiteral*) this;
                break;
            case EXPRESSION_FLOAT_LITERAL:
                delete (FloatLiteral*) this;
                break;
            case EXPRESSION_CHAR_LITERAL:
                delete (CharLiteral*) this;
                break;
            case EXPRESSION_TRUTH_LITERAL:
                delete (TruthLiteral*) this;
                break;
            case EXPRESSION_VARIABLE_VALUE:
                delete (VariableValue*) this;
                break;
            case EXPRESSION_FUNCTION_CALL:
                delete (FunctionCall*) this;
                break;
            case EXPRESSION_BINARY:
                delete (BinaryExpression*) this;
                break;
            default:
                break;
        }
    }
}; 
