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

#include "model/model.hpp"

namespace tabic
{
    typedef enum ParseStatus 
    {
        PARSE_STATUS_NONE, PARSE_STATUS_SUCCESS, PARSE_STATUS_FAIL
    } ParseStatus; 


    /** @brief The exception thrown when a function is called with the incorrect number of arguments. 
     */
    class CallIncorrectNumberOfArgs
    {
        public:
            int lineNum = 0; 
            int colNum = 0; 

            CallIncorrectNumberOfArgs(int lineNum, int colNum) : lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Function called with the incorrect number of arguments.";
            }
    }; 

    /** @brief The exception thrown when a queried value is neither an address, nor is it an alias for an address.
     */
    class QueryNotAddress
    {
        public:
            int lineNum = 0; 
            int colNum = 0; 

            QueryNotAddress(int lineNum, int colNum) : lineNum(lineNum), colNum(colNum)
                {}
            
            const char* what() const throw()
            {
                return "Value queried is not an address.";
            } 
    }; 

    /** @brief The exception thrown when the expression given in a function argument has a type which contradicts that of the function's definition.
     */
    class ArgTypeMismatch : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum = 0; 
        

            ArgTypeMismatch(int lineNum, int colNum) : lineNum(lineNum), colNum(colNum)
            { }

            const char* what() const throw()
            {
                return "Function argument expression mismatches the argument's type in the function definition.";
            }
    }; 

    /** @brief The exception thrown when the expression appearing in an Unheap is not an address. 
     */
    class UnheapExpressionNotAddress : std::exception
    {
        public:
            int lineNum = 0;
            int colNum = 0; 

            UnheapExpressionNotAddress(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum)
            {
            }

            const char* what() const throw()
            {
                return "Unheap expression is not of type Addr[...].";
            }
    };

    /** @brief The exception thrown when a row reference uses an unrecognised field.  
     */
    class FieldNotFound : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            std::string fieldName = "";
            Type* tableType = nullptr; 

            FieldNotFound(Type* tableType, std::string fieldName, int lineNum, int colNum)
                : tableType(tableType), fieldName(fieldName), lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Table field not found.";
            }
    }; 

    /** @brief The exception thrown when trying to reference the index of a non-vector type. 
     */
    class IndexNotOfVector : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            IndexNotOfVector(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) {}
            
            const char* what() const throw()
            {
                return "Cannot reference the index of a non-vector type.";
            }
    };

    /** @brief The exception thrown when the vector given for a VECTOR_SET is not a vector. 
     */
    class VectorRefNotVector : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            VectorRefNotVector(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "The given vector reference is not a vector.";
            }
    };

    /** @brief The exception thrown when the variable given to store a table's measure is not an integer. 
     */
    class MeasureNotInteger : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            MeasureNotInteger(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "The given measure is not an integer."; 
            }
    }; 

    /** @brief The exception thrown when the ID given for a TABLE_DELETE is not an integer. 
     */
    class IDNotInt : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            IDNotInt(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "The given ID is not an integer."; 
            }
    }; 

    /** @brief The exception thrown when a TABLE_REF is not of TableType. 
     */
    class TableRefNotTable : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            TableRefNotTable(int lineNum, int colNum) 
                : lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "The given table reference is not a table.";
            }
    };

    /** @brief The exception thrown when an ID_REF is not an Int. 
     */
    class IDRefNotInt : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            IDRefNotInt(int lineNum, int colNum) 
                : lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "The given ID reference is not an Int."; 
            }
    };

    /** @brief The exception thrown when the index of an element reference is not an integer. 
     */
    class IndexNotInteger : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            IndexNotInteger(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "Given index of element reference is not an integer.";
            }
    };
    /** @brief The exception thrown when an assignment expression is to a variable with mismatched types. 
     */
    class AssignmentTypeMismatch : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 
            
            Type* expressionType = nullptr; 
            Type* valueType = nullptr; 

            AssignmentTypeMismatch(Type* valueType, Type* expressionType, int lineNum, int colNum)
                : valueType(valueType), expressionType(expressionType), lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Expression type does not match value for assignment."; 
            }
    };

    /** @brief The exception thrown when a type reference names a type which 
     * is not in the nominated Slab. 
     */
    class TypeNotFound : public std::exception
    {
        public: 
            Slab* typeSlab = nullptr; 
            std::string typeAlias = "";
            TypeNotFound(Slab* typeSlab, std::string typeAlias)
                : typeSlab(typeSlab), typeAlias(typeAlias) { }

            const char* what() const throw()
            {
                return "Type not found in the slab."; 
            }
    }; 

    /** @brief The exception thrown when an Expression cannot be parsed for an unknown reason.
     */
    class ExpressionNotRecognised : public std::exception
    {
        public:
            int lineNum = 0;
            int colNum = 0; 
            ExpressionNotRecognised(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Failed to parse expression.";
            }
    }; 

    /** @brief The exception thrown when a variable reference fails. 
     */
    class VariableNotFound : public std::exception
    {
        public:
            int lineNum = 0; 
            int colNum = 0;
            std::string variableName = "";
            VariableNotFound(std::string variableName, int lineNum, int colNum)
                : variableName(variableName), lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Variable not found.";
            }
    }; 
    
    /** @brief The exception thrown when there is a reference to a Slab which has
     * not been attached (at least to the current Slab).
     */
    class SlabNotAttached : public std::exception
    {
        public:
            int lineNum = 0;
            int colNum  = 0;
            std::string slabName = "";
            Slab* hostSlab = nullptr; 
            SlabNotAttached(std::string slabName, Slab* hostSlab, int lineNum, int colNum)
                : slabName(slabName), hostSlab(hostSlab), lineNum(lineNum), colNum(colNum) {}
            const char* what() const throw()
            {
                return "Referencing a slab which is not attached (at least not to this slab).";
            }
    }; 

    /** @brief The exception thrown when there is a reference to a Context which cannot
     * be found within a Slab. 
     */
    class ContextNotFound : public std::exception
    {
        public:
            int lineNum = 0; 
            int colNum = 0; 
            std::string contextName = "";
            Slab* contextSlab = nullptr; 

            ContextNotFound(std::string contextName, Slab* contextSlab, int lineNum, int colNum)
                : contextName(contextName), contextSlab(contextSlab), lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "Context not found.";
            }
    }; 


    /** @brief The exception thrown when there is a reference to a Context which cannot
     * be found within a Slab. 
     */
    class DumpNotFound : public std::exception
    {
        public:
            int lineNum = 0; 
            int colNum = 0; 
            std::string dumpName = "";
            Slab* dumpSlab = nullptr; 

            DumpNotFound(std::string dumpName, Slab* dumpSlab, int lineNum, int colNum)
                : dumpName(dumpName), dumpSlab(dumpSlab), lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "Dump not found.";
            }
    }; 


    /** @brief The exception thrown when a referenced context is not in the capture list of a TabithaFunction. 
     */
    class ContextNotCaptured : public std::exception
    {
        public:
            int lineNum = 0; 
            int colNum = 0; 
            Context* context = nullptr; 
            TabithaFunction* function = nullptr; 

            ContextNotCaptured(Context* context, TabithaFunction* function, int lineNum, int colNum)
                : context(context), function(function), lineNum(lineNum), colNum(colNum) { }

            const char* what() const throw()
            {
                return "Context not captured by function which references it."; 
            }
    }; 

    /** @brief The exception thrown when a referenced Function is not in the nominated Slab. 
     */
    class FunctionNotFound : public std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 
            Slab* context = nullptr; 
            std::string functionName = ""; 

            FunctionNotFound(std::string functionName, Slab* context, int lineNum, int colNum)
                : functionName(functionName), context(context), lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Function not found."; 
            }
    };

    /** @brief The exception thrown when a condition does not have Truth type. 
     */
    class ConditionNotTruth : public std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 
            
            ConditionNotTruth(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Condtion does not have Truth type.";
            }
    };

    /** @brief The exception thrown when the arguments of a binary operator are poorly typed.
     */
    class OperatorTypeMismatch : public std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            BinaryExpression* expression; 

            OperatorTypeMismatch(BinaryExpression* expression, int lineNum, int colNum)
                : expression(expression), lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Mismatch in types under binary operator.";
            }
    };

    /** @brief The exception thrown when we attempt to reference a member of a non-CollectionType. 
     */
    class MemberNotOfCollection : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0;

            MemberNotOfCollection(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) {} 

            const char* what() const throw()
            {
                return "Attempting to reference the member of a non-collection type."; 
            }
    };

    /** @brief The exception thrown when a collection type's member is not found.
     */
    class MemberNotFound : std::exception
    {
        public:
            int lineNum = 0; 
            int colNum  = 0; 

            MemberNotFound(int lineNum, int colNum)
                : lineNum(lineNum), colNum(colNum) {}

            const char* what() const throw()
            {
                return "Member not found."; 
            }
    }; 

    /** @brief Parses the given Bundle and all of the Slab it owns.
     * 
     * To parse a Bundle, means to translate every element of the source code
     * into the in-memory model. 
     * After this function is called therefore, it should be possible to completely
     * discard the source code. 
     *
     * @param bundle The Bundle to be parsed, intented to have been generated by createBundle. 
     */
    ParseStatus parseBundle(Bundle* bundle); 

    /** @brief Parses the given Slab and all of the Functions and Types it owns.
     *
     * @param slab The Slab to be parsed, intended to have been generated by createSlab. 
     */
    void parseSlab(Slab* slab); 

    /** @brief Parses the given CollectionType and all of its members. 
     *
     * @param collectionType The CollectionType to be parsed. 
     */
    void parseCollectionType(CollectionType* collectionType); 

    /** @brief Parses the given AliasType. 
     *
     * @param aliasType The AliasType to be parsed. 
     */
    void parseAliasType(AliasType* aliasType); 

    /** @brief Parse the given Type, whatever it may be. 
     *
     * Although this function can handle any element of the union Type, 
     * it is expected to be either a CollectionType or AliasType. 
     *
     * @param The Type to be parsed.
     */
    void parseType(Type* type);

    /** @brief Returns the type referenced by \p node if it exists, or creates a type according to \p node.
     *
     * @param node The PEG AST node which either references or defines the Type. 
     */
    Type* getOrCreateType(ASTNode node, Block* hostBlock, Slab* hostSlab); 

    /** @brief Parse the given TabithaFunction.
     *
     * To parse a TabtithaFunction, is to:
     * - Establish the return type
     * - Establish the arguments (if any)
     * - Establish the captured contexts (if any)
     * - Parse the main block and its children
     *
     * @param tabithaFunction The TabithaFunction to be parsed. 
     */
    void parseTabithaFunction(TabithaFunction* tabithaFunction); 

    /** @brief Parse the given ExternalFunction.
     *
     * To parse an ExternalFunction, is to:
     * - Establish the return type
     * - Establish the arguments (if any)
     * - Establish the actual name of the function used for its external definition 
     */
    void parseExternalFunction(ExternalFunction* externalFunction);

    /** @brief Parse the given Function, whatever its common->functionClass.
     *
     * @param function The Function to be parsed. 
     */
    void parseFunction(Function* function); 

    /** @brief Parses and returns a Block as defined by \p node. 
     *
     * @param node The PEG AST node which defines the Block. 
     * @param parentBlock The Block which contains the Block. May be `nullptr` if this is a main block.  
     * @param hostFunction The Function which contains the Block. Ignored if a \p parentBlock is given. 
     *
     */
    Block* parseBlock(ASTNode block, Block* parentBlock, TabithaFunction* hostFunction); 

    /** @brief Parses and returns a Return as defined by \p node. 
     *
     * @param node The PEG AST node which defines the Return. 
     * @param hostBlock The Block which contains the Return. 
     */
    Return* parseReturn(ASTNode node, Block* hostBlock);

    /** @brief Parses and returns an Expression defined by \p node. 
     *
     * @param node The PEG AST node which defines the Expression. 
     * @param hostBlock The Block which contains the Expression. May be `nullptr`. 
     * @param hostSlab The Slab which contains the Expression. Ignored if \p hostBlock is given. 
     */
    Expression* parseExpression(ASTNode node, Block* hostBlock, Slab* hostSlab); 

    /** @brief Parses and returns an Expression defined by some SINGLETON_EXPRESSION \p node. 
     *
     * @param node The PEG AST node which defines the Expression.
     * @param hostBlock The Block which contains the Expression. May be `nullptr`.
     * @param hostSlab The Slab which contains the Expression. Ignored if \p hostBlock is given.
     */
    Expression* parseSingletonExpression(ASTNode node, Block* hostBlock, Slab* hostSlab);

    /** @brief Parses and returns an Expression defined by some BINARY_EXPRESSION \p node.
     *
     * @param node The PEG AST node which defines the Expression.
     * @param hostBlock The BLock which contains the Expression. May be `nullptr`. 
     * @param hostSlab The Slab which contains the Expression. Ignored if \p hostBlock is given.
     */
    BinaryExpression* parseBinaryExpression(ASTNode node, Block* hostBlock, Slab* hostSlab);

    /** @brief Parses and returns a StackedDeclaration defined by \p node. 
     *
     * @param node The PEG AST node which defines the StackedDeclaration.
     * @param hostBlock The Block which contains the StackedDeclaration.
     */
    StackedDeclaration* parseStackedDeclaration(ASTNode node, Block* hostBlock); 

    /** @brief Parses and returns a HeapedDeclaration defined by \ node. 
     *
     * @param node The ASTNode which defines the HeapedDeclaration.
     * @param hostBlock The Block which contains the HeapedDeclaration.
     */
    HeapedDeclaration* parseHeapedDeclaration(ASTNode node, Block* hostBlock); 

    /** @brief Parses and returns an Assignment defined by \p node. 
     *
     * @param node The PEG AST node which defines the Assignment. 
     * @param hostBlock the Block which contains the Assignment. 
     */
    Assignment* parseAssignment(ASTNode node, Block* hostBlock); 

    /** @brief Returns the ValueRef described by a VALUE_REF node. 
     *
     * @param node The PEG AST node which defines the VALUE_REF. 
     * @param hostBlock The Block in which the reference appears.
     */
    ValueRef* parseValueRef(ASTNode node, Block* hostBlock); 

    /** @brief Returns the ValueRef described the a VALUE_SUB_REF node. 
     *
     * @param node The PEG AST node which defines the VALUE_SUB_REF. 
     * @param hostBlock The Block in which the reference appears.
     */
    ValueRef* parseValueSubRef(ASTNode node, Block* hostBlock, ValueRef* parent); 

    /** @brief Parses the given Context, \p context. 
     *
     * @param context The Context to be parsed.
     */
    void parseContext(Context* context);

    /** @brief Returns the Context described by a CONTEXT_REF.
     *
     * @param node The ASTNode which defines the CONTEXT_REF.
     * @param hostSlab The Slab in which the reference appears. Ignored if hostFunction is given.
     * @param hostFunction The TabithaFunction in which the reference appears. Reference is checked against the Function captures. 
     */
    Context* parseContextRef(ASTNode node, Slab* hostSlab, TabithaFunction* hostFunction); 

    /** @brief Parses the given Dump, \p dump. 
     *
     * @param dump The Dump to be parsed.
     */
    void parseDump(Dump* dump); 

    /** @brief Returns the Dump described by a DUMP_REF.
     *
     * @param node The ASTNode which defines the DUMP_REF. 
     * @param hostSlab The Slab in which the reference appears.
     */
    Dump* parseDumpRef(ASTNode node, Slab* hostSlab); 

    /** @brief Returns the Function described by a FUNCTION_CALL or PROCEDURE_CALL.
     *
     * @param node The ASTNode which defines the reference.
     * @param hostSlab The Slab in which the reference appears.
     */
    Function* parseFunctionRef(ASTNode node, Slab* hostSlab);

    /** @brief Parses and returns the ProcedureCall defined by \p node. 
     *
     * @param node The ASTNode which defines the ProcedureCall.
     * @param hostBlock The Block in which the ProcedureCall appears.
     */
    ProcedureCall* parseProcedureCall(ASTNode node, Block* hostBlock);

    /** @brief Parses and returns the VectorSet defined by \p node. 
     * 
     * @param node The ASTNode which defines the VectorSet. 
     * @param hostBlock The Block in which the VectorSet appears. 
     */
    VectorSet* parseVectorSet(ASTNode node, Block* hostBlock); 

    /** @brief Parses and returns the TableInsert defined by \p node. 
     * 
     * @param node The ASTNode which defines the TableInsert. 
     * @param hostBlock The Block in which the TableInsert appears.
     */
    TableInsert* parseTableInsert(ASTNode node, Block* hostBlock);

    /** @brief Parses and returns the TableDelete defined by \p node.
     *
     * @param node The ASTNode which defines the TableDelete.
     * @param hostBlock the Block in which the TableDelete appears.
     */
    TableDelete* parseTableDelete(ASTNode node, Block* hostBlock); 

    /** @brief Parses and returns the TableMeasure defined by \p node. 
     * @param node The ASTNode which defines the TableMeasure. 
     * @param hostBlock the Block in which the TableMeasure appears. 
     */
    TableMeasure* parseTableMeasure(ASTNode node, Block* hostBlock); 

    /** @brief Parses and returns the TableCrunch defined  by \p node. 
     * @param node The ASTNode which define the TableCrunch. 
     * @param hostBlock The Block in which the TableCrunch appears. 
     */
    TableCrunch* parseTableCrunch(ASTNode node, Block* hostBlock);

    /** @brief Parses and returns the Conditional defined by \p node. 
     *
     * @param node The ASTNode which defines the Conditional.
     * @param hostBlock The Block in which the Conditonal appears. 
     */
    Conditional* parseConditional(ASTNode node, Block* hostBlock);
  
    /** @brief Parses and returns the Branch defined by \p node. 
     *
     * @param node The ASTNode which defines the Branch.
     * @param hostBlock The Block in which the Branch appears.
     */
    Branch* parseBranch(ASTNode node, Block* hostBlock);

    /** @brief Parses and returns the Loop defined by \p node. 
     *
     * @param node The ASTNode which defines the Loop.
     * @param hostBlock The Block in which the Loop appears.
     */
    Loop* parseLoop(ASTNode node, Block* hostBlock);

    /** @brief Parses and returns the Label defined by \p node 
     *
     * @param node The ASTNode which defines the Label. 
     * @param hostBlock The Block in which the Label occurs. 
     */
    Label* parseLabel(ASTNode node, Block* hostBlock);

    /** @brief Parses and returns the Unheap defined by \p node. 
     *
     * @param node The ASTNode which defines the Unheap. 
     * @param hostBlock The Block in which the Unheap occurs. 
     */
    Unheap* parseUnheap(ASTNode node, Block* hostBlock);

    /** @brief Decides whether the given Type are equivalent or not. 
     *
     * @param a The first Type. 
     * @param b The second Type.
     */
    bool typesMatch(Type* a, Type* b);
}

