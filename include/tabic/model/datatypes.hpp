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

#include "cpp-peglib/peglib.h"

#include<memory> 

/** @brief The namespace containing datatypes and functions relating to the
 * operation of `tabic`. 
 */
namespace tabic
{
    typedef std::shared_ptr<peg::AstBase<peg::EmptyType>> ASTNode; 

    typedef enum SlabDomain
    {
        SLAB_DOM_NONE, SLAB_DOM_LOCAL, SLAB_DOM_EXTERNAL
    } SlabDomain; 
    typedef struct Bundle Bundle; 
    typedef struct Slab Slab; 

    /** @brief Indicator for the element of the union Function an instance happens to be. 
     */
    typedef enum FunctionClass 
    {
        FUNCTION_NONE,      ///< Default value. 
        FUNCTION_TABITHA,   ///< Corresponds to TabithaFunction
        FUNCTION_EXTERNAL,  ///< Corresponds to ExternalFunction
        FUNCTION_CORE       ///< Corresponds to CoreFunction
    } FunctionClass; 
    typedef struct FunctionCommon FunctionCommon; 
    typedef struct TabithaFunction TabithaFunction; 
    typedef struct ExternalFunction ExternalFunction; 
    typedef struct CoreFunction CoreFunction;  
    typedef union Function Function; 

    typedef struct TypeCommon TypeCommon; 
    /** @brief Indicator for the element of the union Type an instance happens to be. 
     */
    typedef enum TypeClass
    {
        TYPE_NONE,          ///< Default value. 
        TYPE_PRIMITIVE,     ///< Corresponds to PrimitiveType.
        TYPE_COLLECTION,    ///< Corresponds to CollectionType. 
        TYPE_ADDRESS,       ///< Corresponds to AddressType. 
        TYPE_VECTOR,        ///< Corresponds to VectorType. 
        TYPE_TABLE,         ///< Corresponds to TableType.
        TYPE_ALIAS          ///< Corresponds to AliasType.
    } TypeClass; 
    typedef struct PrimitiveType PrimitiveType; 
    typedef struct CollectionType CollectionType; 
    typedef struct CollectionMember CollectionMember; 
    typedef struct AddressType AddressType; 
    typedef struct VectorType VectorType; 
    typedef struct TableField TableField;
    typedef struct TableType TableType; 
    typedef struct AliasType AliasType;
    typedef union Type Type; 

    typedef struct StatementCommon StatementCommon;
    /** @brief Indicator for the element of the union Statement an instance happens to be. 
     */
    typedef enum StatementClass
    {
        STATEMENT_NONE,                     ///< Default value.
        STATEMENT_BLOCK,                    ///< Corresponds to Block.
        STATEMENT_RETURN,                   ///< Corresponds to Return.
        STATEMENT_STACKED_DECLARATION,      ///< Corresponds to LocalDeclaration.
        STATEMENT_HEAPED_DECLARATION,       ///< Corresponds to HeapedDeclaration.
        STATEMENT_ASSIGNMENT,               ///< Corresponds to Assignment.
        STATEMENT_CONDITIONAL,              ///< Corresponds to Conditional.
        STATEMENT_BRANCH,                   ///< Corresponds to Branch.
        STATEMENT_LOOP,                     ///< Corresponds to Loop.
        STATEMENT_PROCEDURE_CALL,           ///< Corresponds to ProcedureCall.
        STATEMENT_TABLE_INSERT,             ///< Corresponds to TableInsert. 
        STATEMENT_TABLE_DELETE,             ///< Corresponds to TableDelete.
        STATEMENT_TABLE_MEASURE,            ///< Corresponds to TableMeasure. 
        STATEMENT_TABLE_CRUNCH,             ///< Corresponds to TableCrunch. 
        STATEMENT_VECTOR_SET,               ///< Corresponds to VectorSet. 
        STATEMENT_LABEL,                    ///< Corresponds to Label.
        STATEMENT_UNHEAP                    ///< Corresponds to Unheap.
    } StatementClass; 
    typedef struct Block Block; 
    typedef struct Return Return; 
    typedef struct StackedDeclaration StackedDeclaration; 
    typedef struct HeapedDeclaration HeapedDeclaration; 
    typedef struct Assignment Assignment; 
    typedef struct ConditionBlockPair ConditionBlockPair;
    typedef struct Conditional Conditional; 
    typedef struct Branch Branch; 
    typedef struct Loop Loop; 
    typedef struct ProcedureCall ProcedureCall;
    typedef struct VectorSet VectorSet; 
    typedef struct TableInsert TableInsert; 
    typedef struct TableDelete TableDelete;
    typedef struct TableMeasure TableMeasure; 
    typedef struct TableCrunch TableCrunch; 
    typedef struct Label Label; 
    typedef struct Unheap Unheap;  
    typedef union Statement Statement;

    typedef struct ExpressionCommon ExpressionCommon; 
    /** @brief Indicator for the element of the union Expression an instance happens to be. 
     */
    typedef enum ExpressionClass {
        EXPRESSION_NONE,            ///< Default value.
        EXPRESSION_NULL,            ///< Corresponds to NullValue.  
        EXPRESSION_INT_LITERAL,     ///< Corresponds to IntLiteral.
        EXPRESSION_FLOAT_LITERAL,   ///< Corresponds to FloatLiteral.
        EXPRESSION_SHORT_LITERAL,   ///< Corresponds to ShortLiteral.
        EXPRESSION_LONG_LITERAL,    ///< Corresponds to LongLiteral.
        EXPRESSION_DOUBLE_LITERAL,  ///< Corresponds to DoubleLiteral.
        EXPRESSION_SIZE_LITERAL,    ///< Corresponds to SizeLiteral.
        EXPRESSION_CHAR_LITERAL,    ///< Corresponds to CharLiteral.
        EXPRESSION_STRING_LITERAL,  ///< Corresponds to StringLiteral. 
        EXPRESSION_TRUTH_LITERAL,   ///< Corresponds to TruthLiteral.
        EXPRESSION_VARIABLE_VALUE,  ///< Corresponds to VariableValue.
        EXPRESSION_FUNCTION_CALL,   ///< Corresponds to FunctionCall.   
        EXPRESSION_BRACKETED,       ///< Corresponds to BracketedExpression.
        EXPRESSION_BINARY           ///< Corresponds to BinaryExpression.
    } ExpressionStatement; 
    typedef struct NullValue NullValue; 
    typedef struct IntLiteral IntLiteral;
    typedef struct FloatLiteral FloatLiteral; 
    typedef struct ShortLiteral ShortLiteral; 
    typedef struct LongLiteral LongLiteral; 
    typedef struct DoubleLiteral DoubleLiteral; 
    typedef struct SizeLiteral SizeLiteral;
    typedef struct CharLiteral CharLiteral; 
    typedef struct StringLiteral StringLiteral;
    typedef enum TruthValue
    {
        TRUTH_FALSE, TRUTH_TRUE
    } TruthValue;
    typedef struct TruthLiteral TruthLiteral; 
    typedef struct VariableValue VariableValue; 
    typedef struct FunctionCall FunctionCall;
    typedef struct BracketedExpression BracketedExpression;
    //listed in order, from least to most precedence
    typedef enum BinaryOperator
    {
        BINARY_OP_NONE,     ///< Default value.
        BINARY_OP_SUB,      ///< Corresponds to `-`. 
        BINARY_OP_PLUS,     ///< Corresponds to `+`.
        BINARY_OP_MUL,      ///< Corresponds to `*`.
        BINARY_OP_DIV,      ///< Corresponds to `/`. 
        BINARY_OP_LT,       ///< Corresponds to `<`.
        BINARY_OP_GT,       ///< Corresponds to `>`.
        BINARY_OP_LTE,      ///< Corresponds to `<=`.   
        BINARY_OP_GTE,      ///< Corresponds to `>=`.
        BINARY_OP_EQUALS,   ///< Corresponds to `==`. 
        BINARY_OP_NOT_EQUAL ///< Corresponds to `!=`. 
    } BinaryOperator;
    typedef struct BinaryExpression BinaryExpression;
    typedef union Expression Expression;

    typedef struct VariableCommon VariableCommon; 
    typedef enum VariableClass
    {
        VARIABLE_NONE,              ///< Default value. 
        VARIABLE_STACKED,           ///< Corresponds to StackedVariable.
        VARIABLE_HEAPED,            ///< Corresponds to HeapedVariable. 
        VARIABLE_CONTEXT,           ///< Corresponds to ContextVariable.
        VARIABLE_DUMP               ///< Corresponds to DumpVariable. 
    } VariableClass; 
    typedef struct StackedVariable StackedVariable; 
    typedef struct HeapedVariable HeapedVariable; 
    typedef struct ContextVariable ContextVariable;
    typedef struct DumpVariable DumpVariable;
    typedef union Variable Variable; 

    typedef enum ValueRefClass
    {
        VALUE_REF_NONE,         ///< Default value. 
        VALUE_REF_VARIABLE,     ///< Corresponds to VariableRef.
        VALUE_REF_MEMBER,       ///< Corresponds to MemberRef.
        VALUE_REF_ELEMENT,      ///< Corresponds to ElementRef. 
        VALUE_REF_ROW           ///< COrresponds to IDRef. 
    } ValueRefClass;
    typedef struct ValueRefCommon VariableRefCommon;
    typedef struct VariableRef VariableRef; 
    typedef struct MemberRef MemberRef; 
    typedef struct ElementRef ElementRef; 
    typedef struct RowRef RowRef; 
    typedef union ValueRef ValueRef;

    typedef struct Context Context; 

    typedef struct Dump Dump; 
}



