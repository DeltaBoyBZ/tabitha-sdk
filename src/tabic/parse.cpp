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
#include "tabic/parse.hpp"
#include "tabic/model/model.hpp"

//a flag to indicate whether or not the parsing stage was successful
//if not successful, the building stage should not be started
static tabic::ParseStatus _parseStatus = tabic::PARSE_STATUS_NONE; 

#define PARSE_FAIL _parseStatus = tabic::PARSE_STATUS_FAIL

tabic::ParseStatus tabic::parseBundle(Bundle* bundle)
{
    //Parse each of the Slab.
    for(auto pair : bundle->create.slabs)
    {
        Slab* slab = pair.second; 
        //Parse each of slab->create.types.
        for(auto pair : slab->create.types)
        {
            parseType(pair.second); 
        }
        //Parse each of slab->create.contexts
        for(auto pair : slab->create.contexts)
        {
            parseContext(pair.second); 
        }
        //Parse each of slab->create.dumps
        for(auto pair : slab->create.dumps)
        {
            parseDump(pair.second);
        }
        //Parse each of slab->create.functions.
        for(auto pair : slab->create.functions)
        {
            //use the generic parseFunction because this function
            //may be either a TabithaFunction or an ExternalFunction
            parseFunction(pair.second);
        }
        //parseSlab(pair.second); 
    }
    if(_parseStatus == PARSE_STATUS_NONE) _parseStatus = PARSE_STATUS_SUCCESS;
    return _parseStatus;
}

void tabic::parseSlab(Slab* slab)
{
    //Parse each of slab->create.types.
    for(auto pair : slab->create.types)
    {
        parseType(pair.second); 
    }
    //Parse each of slab->create.contexts
    for(auto pair : slab->create.contexts)
    {
        parseContext(pair.second); 
    }
    //Parse each of slab->create.dumps
    for(auto pair : slab->create.dumps)
    {
        parseDump(pair.second);
    }
    //Parse each of slab->create.functions.
    for(auto pair : slab->create.functions)
    {
        //use the generic parseFunction because this function
        //may be either a TabithaFunction or an ExternalFunction
        parseFunction(pair.second);
    }
}

void tabic::parseCollectionType(CollectionType* collectionType)
{
    int memberCount = 0; 
    NODE_LOOP(collectionType->create.node, collectionSub)
    {
        //Parse a member.
        NODE_CHECK(collectionSub, "COLLECTION_MEMBER")
        {
            CollectionMember member; 
            //Parse the member's type.
            NODE_OP(collectionSub, typeRefNode, "TYPE_REF")
            {
                member.type = getOrCreateType(typeRefNode, nullptr, collectionType->create.hostSlab);  
            }
            std::string memberName = "";
            //Parse the member's name.
            NODE_OP(collectionSub, variableNameNode, "VARIABLE_NAME")
            {
                memberName = variableNameNode->token_to_string(); 
            }
            member.index = memberCount; 
            collectionType->parse.members[memberName] = member;
            memberCount++; 
        }
    }
}

void tabic::parseAliasType(AliasType* aliasType)
{
    Slab* hostSlab = aliasType->create.hostSlab;
    NODE_OP(aliasType->create.node, typeNode, "TYPE_REF")
    {
        aliasType->parse.repType = getOrCreateType(typeNode, nullptr, hostSlab); 
    }
    //std::memcpy(aliasType, repType, sizeof(Type)); // delaying this operation to account for the fact the repType still needs to be built
}

tabic::Type* tabic::getOrCreateType(ASTNode node, Block* hostBlock, Slab* hostSlab)
{
    if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
    //If the node describes a PrimitiveType, we can return it right away.
    NODE_OP(node, primitiveNode, "PRIMITIVE_TYPE")
    {
        NODE_OP(primitiveNode, sizeNode, "SIZE_TYPE")
        {
            return (Type*) &SupportedPrimitives::SIZE;
        }
        NODE_OP(primitiveNode, intNode, "INT_TYPE")
        {
            return (Type*) &SupportedPrimitives::INT; 
        }
        NODE_OP(primitiveNode, shortNode, "SHORT_TYPE")
        {
            return (Type*) &SupportedPrimitives::SHORT; 
        }
        NODE_OP(primitiveNode, longNode, "LONG_TYPE")
        {
            return (Type*) &SupportedPrimitives::LONG; 
        }
        NODE_OP(primitiveNode, floatNode, "FLOAT_TYPE")
        {
            return (Type*) &SupportedPrimitives::FLOAT; 
        }
        NODE_OP(primitiveNode, doubleNode, "DOUBLE_TYPE")
        {
            return (Type*) &SupportedPrimitives::DOUBLE;
        }
        NODE_OP(primitiveNode, sizeNode, "SIZE_TYPE")
        {
            return (Type*) &SupportedPrimitives::SIZE;
        }
        NODE_OP(primitiveNode, charNode, "CHAR_TYPE")
        {
            return (Type*) &SupportedPrimitives::CHAR;
        }
        NODE_OP(primitiveNode, truthNode, "TRUTH_TYPE")
        {
            return (Type*) &SupportedPrimitives::TRUTH;
        }
        NODE_OP(primitiveNode, noneNode, "NONE_TYPE")
        {
            return (Type*) &SupportedPrimitives::NONE; 
        }
    }
    //If we have a NAMED_TYPE, then we have to establish the host slab and the 
    //type alias
    NODE_OP(node, namedTypeNode, "NAMED_TYPE")
    {
        Slab* typeSlab = hostSlab; 
        std::string typeAlias = ""; 
        try
        {
            NODE_OP(namedTypeNode, slabNode, "SLAB_NAME")
            {
                std::string slabName = slabNode->token_to_string(); 
                typeSlab = nullptr; 
                for(auto pair : hostSlab->create.attachments)
                {
                    if(slabName == pair.first)
                    {
                        typeSlab = pair.second;
                        break;
                    }
                }
                if(!typeSlab) throw SlabNotAttached(slabName, hostSlab, node->line, node->column);
            }
            NODE_OP(namedTypeNode, nameNode, "TYPE_NAME")
            {
                typeAlias = nameNode->token_to_string(); 
            }
            if(!typeSlab->create.types[typeAlias]) throw TypeNotFound(typeSlab, typeAlias); 
            Type* type = typeSlab->create.types[typeAlias]; 
            if(type->common.typeClass == TYPE_ALIAS)
            {
                parseType(type); 
            }
            return typeSlab->create.types[typeAlias];
        }
        catch(TypeNotFound ex)
        {
            std::cerr << ex.what() << std::endl;
            std::cerr << "Slab: " << ex.typeSlab->create.id << std::endl;
            std::cerr << "Type: " << ex.typeAlias << std::endl; 
            PARSE_FAIL; 
        }
        catch(SlabNotAttached ex)
        {
            std::cerr << ex.what() << std::endl;
            std::cerr << "Referenced Slab: " << ex.slabName << "; Host Slab: " << ex.hostSlab->create.id << std::endl;
            PARSE_FAIL;
        }
    }
    //If we have an ADDRESS_TYPE, then this needs to be created. 
    NODE_OP(node, addressTypeNode, "ADDRESS_TYPE")
    {
        AddressType* addressType = new AddressType();
        NODE_OP(addressTypeNode, pointsNode, "TYPE_REF")
        {
            addressType->parse.pointsTo = getOrCreateType(pointsNode, hostBlock, hostSlab); 
        }
        return (Type*) addressType; 
    }
    //If we have a VECTOR_TYPE, then this needs to be created. 
    NODE_OP(node, vectorTypeNode, "VECTOR_TYPE")
    {
        VectorType* vectorType = new VectorType(); 
        NODE_OP(vectorTypeNode, elemTypeNode, "TYPE_REF")
        {
            vectorType->parse.elemType = getOrCreateType(elemTypeNode, hostBlock, hostSlab);
        }
        NODE_OP(vectorTypeNode, numElemNode, "EXPRESSION")
        {
            vectorType->parse.numElem = parseExpression(numElemNode, hostBlock, hostSlab);  
        }
        return (Type*) vectorType;
    }
    //If we have a TABLE_TYPE, then this needs to be created.
    NODE_OP(node, tableTypeNode, "TABLE_TYPE")
    {
        TableType* tableType = new TableType(); 
        tableType->parse.fields.push_back(TableField{(Type*)&SupportedPrimitives::INT, "id"}); 
        tableType->parse.fields.push_back(TableField{(Type*)&SupportedPrimitives::INT, "use"}); 
        NODE_LOOP(tableTypeNode, tableSub)
        {
            NODE_CHECK(tableSub, "TABLE_FIELD")
            {
                TableField field;
                NODE_OP(tableSub, typeNode, "TYPE_REF")
                {
                    field.type = getOrCreateType(typeNode, hostBlock, hostSlab);
                }
                NODE_OP(tableSub, nameNode, "VARIABLE_NAME")
                {
                    field.name = nameNode->token_to_string();
                }
                tableType->parse.fields.push_back(field); 
            }
            NODE_CHECK(tableSub, "EXPRESSION")
            {
                tableType->parse.numRows = parseExpression(tableSub, hostBlock, hostSlab);
            }

        }
        return (Type*) tableType;
    }
    return nullptr;  
}

void tabic::parseType(Type* type)
{
    switch(type->common.typeClass)
    {
        case TYPE_COLLECTION:
            parseCollectionType((CollectionType*) type); 
            break;
        case TYPE_ALIAS:
            parseAliasType((AliasType*) type); 
        default:
            break;
    }
}

void tabic::parseTabithaFunction(TabithaFunction* tabithaFunction)
{
    //Parse captured contexts (if any)
    NODE_OP(tabithaFunction->create.node, contextCaptureNode, "CONTEXT_CAPTURE_LIST")
    {
        NODE_LOOP(contextCaptureNode, contextRefNode)
        {
            Context* context = parseContextRef(contextRefNode, tabithaFunction->create.hostSlab, nullptr); 
            tabithaFunction->parse.captures.push_back(context); 
        }
    }
    //Parse the return type. 
    //First set the return type to `None` as default. 
    tabithaFunction->common.parse.returnType = (Type*) &SupportedPrimitives::NONE; 
    NODE_OP(tabithaFunction->create.node, typeRefNode, "TYPE_REF")
    {
        tabithaFunction->common.parse.returnType = getOrCreateType(typeRefNode, nullptr, tabithaFunction->create.hostSlab); 
    }
    //Parse args (if any).
    NODE_OP(tabithaFunction->create.node, argListNode, "FUNCTION_ARG_LIST")
    {
        NODE_LOOP(argListNode, argNode)
        {
            StackedVariable* arg = new StackedVariable(nullptr);
            NODE_OP(argNode, typeNode, "TYPE_REF")
            {
                arg->common.parse.type = getOrCreateType(typeNode, nullptr, tabithaFunction->create.hostSlab); 
            }
            NODE_OP(argNode, nameNode, "VARIABLE_NAME")
            {
                arg->common.parse.name = nameNode->token_to_string();
            }
            tabithaFunction->parse.args.push_back(arg);
        }
    }
    //Parse the main block. 
    NODE_OP(tabithaFunction->create.node, blockNode, "BLOCK")
    {
        tabithaFunction->parse.mainBlock = parseBlock(blockNode, nullptr, tabithaFunction); 
    }
}

void tabic::parseExternalFunction(ExternalFunction* externalFunction)
{
    //Get the external name. 
    NODE_OP(externalFunction->create.node, externalNameNode, "EXTERNAL_NAME")
    {
        NODE_OP(externalNameNode, nameNode, "FUNCTION_NAME")
        {
            externalFunction->parse.externalName = nameNode->token_to_string(); 
        }
    }
    //Parse the return type. 
    //First set the return type to `None` as default. 
    externalFunction->common.parse.returnType = (Type*) &SupportedPrimitives::NONE; 
    NODE_OP(externalFunction->create.node, typeRefNode, "TYPE_REF")
    {
        externalFunction->common.parse.returnType = getOrCreateType(typeRefNode, nullptr, externalFunction->create.hostSlab); 
    }
    //Parse args (if any).
    NODE_OP(externalFunction->create.node, argListNode, "EXTERNAL_ARG_LIST")
    {
        NODE_LOOP(argListNode, argNode)
        {
            if(argNode->name == "TYPE_REF")
            {
                Type* type = getOrCreateType(argNode, nullptr, externalFunction->create.hostSlab);
                externalFunction->parse.args.push_back(type); 
            }
        }
    }
}

void tabic::parseFunction(Function* function)
{
    if(function->common.parseStatus == FUNCTION_PARSE_COMPLETE) return; 
    switch(function->common.functionClass)
    {
        case FUNCTION_TABITHA:
            parseTabithaFunction((TabithaFunction*) function); 
            break;
        case FUNCTION_EXTERNAL:
            parseExternalFunction((ExternalFunction*) function); 
            break; 
        default:
            break;
    }
    function->common.parseStatus = FUNCTION_PARSE_COMPLETE; 
}

tabic::Block* tabic::parseBlock(ASTNode node, Block* parentBlock, TabithaFunction* hostFunction)
{
    if(parentBlock) hostFunction = parentBlock->common.parse.hostFunction; 
    Block* block = new Block(node, parentBlock, hostFunction); 
    NODE_LOOP(node, blockSub)
    {
        NODE_CHECK(blockSub, "STATEMENT")
        {
            Statement* statement = nullptr; 
            NODE_OP(blockSub, returnNode, "RETURN")
            {
                statement = (Statement*) parseReturn(returnNode, block); 
            }
            NODE_OP(blockSub, stackedNode, "STACKED_DECLARATION")
            {
                statement = (Statement*) parseStackedDeclaration(stackedNode, block);
            }
            NODE_OP(blockSub, heapedNode, "HEAPED_DECLARATION")
            {
                statement = (Statement*) parseHeapedDeclaration(heapedNode, block); 
            }
            NODE_OP(blockSub, assignmentNode, "ASSIGNMENT")
            {
                statement = (Statement*) parseAssignment(assignmentNode, block); 
            }
            NODE_OP(blockSub, procedureNode, "PROCEDURE_CALL")
            {
                statement = (Statement*) parseProcedureCall(procedureNode, block); 
            }
            NODE_OP(blockSub, conditionalNode, "CONDITIONAL")
            {
                statement = (Statement*)  parseConditional(conditionalNode, block); 
            }
            NODE_OP(blockSub, branchNode, "BRANCH")
            {
                statement = (Statement*) parseBranch(branchNode, block);
            }
            NODE_OP(blockSub, loopNode, "LOOP")
            {
                statement = (Statement*) parseLoop(loopNode, block);
            }
            NODE_OP(blockSub, vectorSetNode, "VECTOR_SET")
            {
                statement = (Statement*) parseVectorSet(vectorSetNode, block);
            }
            NODE_OP(blockSub, insertNode, "TABLE_INSERT")
            {
                statement = (Statement*) parseTableInsert(insertNode, block);
            }
            NODE_OP(blockSub, deleteNode, "TABLE_DELETE")
            {
                statement = (Statement*) parseTableDelete(deleteNode, block);
            }
            NODE_OP(blockSub, measureNode, "TABLE_MEASURE")
            {
                statement = (Statement*) parseTableMeasure(measureNode, block); 
            }
            NODE_OP(blockSub, crunchNode, "TABLE_CRUNCH")
            {
                statement = (Statement*) parseTableCrunch(crunchNode, block); 
            }
            NODE_OP(blockSub, labelNode, "LABEL")
            {
                statement = (Statement*) parseLabel(labelNode, block); 
            }
            NODE_OP(blockSub, unheapNode, "UNHEAP")
            {
                statement = (Statement*) parseUnheap(unheapNode, block); 
            }
            NODE_OP(blockSub, subBlockNode, "BLOCK")
            {
                statement = (Statement*) parseBlock(subBlockNode, block, hostFunction); 
            }
            if(!statement && blockSub->nodes[0]->name != "COMMENT") 
            {
                PARSE_FAIL;
                return nullptr; 
            }
            if(statement) block->parse.statements.push_back(statement); 
        }
    }
    return block; 
}

tabic::Return* tabic::parseReturn(ASTNode node, Block* hostBlock)
{
    Return* ret = new Return(node, hostBlock); 
    NODE_OP(node, expressionNode, "EXPRESSION")
    {
        ret->parse.expression = parseExpression(expressionNode, hostBlock, nullptr); 
    }
    return ret; 
}

tabic::Expression* tabic::parseSingletonExpression(ASTNode node, Block* hostBlock, Slab* hostSlab)
{
    if(hostBlock) hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
    NODE_OP(node, nullNode, "NULL")
    {
        NullValue* nullValue = new NullValue(nullNode, hostBlock, hostSlab); 
        return (Expression*) nullValue; 
    }
    NODE_OP(node, intNode, "INT_LITERAL")
    {
        IntLiteral* intLiteral = new IntLiteral(intNode, hostBlock, hostSlab); 
        NODE_OP(intNode, digitNode, "DIGITS")
        {
            intLiteral->parse.value = std::stoi(digitNode->token_to_string()); 
        }
        NODE_OP(intNode, negNode, "NEG")
        {
            intLiteral->parse.value *= -1; 
        }
        return (Expression*) intLiteral;
    }
    NODE_OP(node, floatNode, "FLOAT_LITERAL")
    {
        FloatLiteral* floatLiteral = new FloatLiteral(floatNode, hostBlock, hostSlab);
        float* pre = nullptr; 
        float* post = nullptr; 
        NODE_LOOP(floatNode, floatSub)
        {
            NODE_CHECK(floatSub, "DIGITS")
            {
                if(pre) post = new float(std::stof("0." + floatSub->token_to_string())); 
                else    pre  = new float(std::stof(floatSub->token_to_string())); 
            }
        }
        floatLiteral->parse.value = *pre + *post; 
        NODE_OP(floatNode, negNode, "NEG")
        {
            floatLiteral->parse.value *= -1.0f; 
        }
        delete post; 
        delete pre; 
        return (Expression*) floatLiteral;
    }
    NODE_OP(node, shortNode, "SHORT_LITERAL")
    {
        ShortLiteral* shortLiteral = new ShortLiteral(shortNode, hostBlock, hostSlab); 
        NODE_OP(shortNode, digitNode, "DIGITS")
        {
            shortLiteral->parse.value = (short) std::stoi(digitNode->token_to_string()); 
        }
        NODE_OP(shortNode, negNode, "NEG")
        {
            shortLiteral->parse.value *= -1; 
        }
        return (Expression*) shortLiteral;
    }
    NODE_OP(node, longNode, "LONG_LITERAL")
    {
        LongLiteral* longLiteral = new LongLiteral(longNode, hostBlock, hostSlab); 
        NODE_OP(longNode, digitNode, "DIGITS")
        {
            longLiteral->parse.value = (long) std::stoi(digitNode->token_to_string()); 
        }
        NODE_OP(longNode, negNode, "NEG")
        {
            longLiteral->parse.value *= -1; 
        }
        return (Expression*) longLiteral;
    }
    NODE_OP(node, doubleNode, "DOUBLE_LITERAL")
    {
        DoubleLiteral* doubleLiteral = new DoubleLiteral(doubleNode, hostBlock, hostSlab);
        float* pre = nullptr; 
        float* post = nullptr; 
        NODE_LOOP(doubleNode, floatSub)
        {
            NODE_CHECK(floatSub, "DIGITS")
            {
                if(pre) post = new float(std::stof("0." + floatSub->token_to_string())); 
                else    pre  = new float(std::stof(floatSub->token_to_string())); 
            }
        }
        doubleLiteral->parse.value = *pre + *post; 
        NODE_OP(doubleNode, negNode, "NEG")
        {
            doubleLiteral->parse.value *= -1.0f; 
        }
        delete post; 
        delete pre; 
        return (Expression*) doubleLiteral;
    }
    NODE_OP(node, sizeNode, "SIZE_LITERAL")
    {
        SizeLiteral* sizeLiteral = new SizeLiteral(sizeNode, hostBlock, hostSlab); 
        NODE_OP(sizeNode, digitNode, "DIGITS")
        {
            sizeLiteral->parse.value = (long) std::stoi(digitNode->token_to_string()); 
        }
        NODE_OP(sizeNode, negNode, "NEG")
        {
            sizeLiteral->parse.value *= -1; 
        }
        return (Expression*) sizeLiteral;
    }
    NODE_OP(node, charNode, "CHAR_LITERAL")
    {
        CharLiteral* charLiteral =  new CharLiteral(charNode, hostBlock, hostSlab);
        NODE_OP(charNode, charValNode, "CHAR_VAL")
        {
            charLiteral->parse.value = charValNode->token_to_string()[0];
        }
        return (Expression*) charLiteral; 
    }
    NODE_OP(node, stringNode, "STRING_LITERAL")
    {
        StringLiteral* stringLiteral = new StringLiteral(stringNode, hostBlock, hostSlab); 
        NODE_OP(stringNode, stringValNode, "STRING_CONTENTS")
        {
            stringLiteral->parse.value = stringValNode->token_to_string(); 
        }
        return (Expression*) stringLiteral; 
    }
    NODE_OP(node, truthNode, "TRUTH_LITERAL")
    {
        TruthLiteral* truthLiteral = new TruthLiteral(truthNode, hostBlock, hostSlab); 
        NODE_OP(truthNode, trueValNode, "TRUTH_TRUE")
        {
            truthLiteral->parse.value = TRUTH_TRUE;
        }
        NODE_OP(truthNode, falseValNode, "TRUTH_FALSE")
        {
            truthLiteral->parse.value = TRUTH_FALSE;
        }
        return (Expression*) truthLiteral;
    }
    NODE_OP(node, varValNode, "VARIABLE_VALUE")
    {
        VariableValue* value = new VariableValue(varValNode, hostBlock, hostSlab); 
        NODE_OP(varValNode, varRefNode, "VALUE_REF")
        {
            value->parse.ref = parseValueRef(varRefNode, hostBlock); 
        }
        if(!value->parse.ref) return nullptr; 
        value->common.parse.type = value->parse.ref->common.parse.type; 
        NODE_OP(varValNode, locateNode, "LOCATE")
        {
            value->parse.locate = true; 
        }
        if(value->parse.locate)
        {
            AddressType* ptrType = new AddressType(); 
            ptrType->parse.pointsTo = value->common.parse.type; 
            value->common.parse.type = (Type*) ptrType; 
        }
        return (Expression*) value; 
    }
    NODE_OP(node, functionNode, "FUNCTION_CALL")
    {
        FunctionCall* call = new FunctionCall(functionNode, hostBlock, hostSlab); 
        NODE_OP(functionNode, functionRefNode, "FUNCTION_REF")
        {
            call->parse.callee = parseFunctionRef(functionRefNode, hostSlab); 
        } 
        NODE_LOOP(functionNode, functionSub)
        {
            NODE_CHECK(functionSub, "EXPRESSION")
            {
                Expression* arg = parseExpression(functionSub, hostBlock, hostSlab);
                if(!arg) return nullptr; 
                call->parse.args.push_back(arg);
            }
        }
        if(!call->parse.callee) return nullptr; 
        parseFunction(call->parse.callee); //ensure the callee has been parsed already
        call->common.parse.type = call->parse.callee->common.parse.returnType;
        if(call->parse.callee->common.functionClass == FUNCTION_TABITHA)
        { 
            if(call->parse.callee->tabitha.parse.args.size() != call->parse.args.size())
            {
                std::cerr << call->parse.callee->tabitha.create.name << ", " << call->parse.callee->tabitha.parse.args.size()
                    << ", " << call->parse.args.size() << std::endl; 
                throw CallIncorrectNumberOfArgs(node->line, node->column); 
            }
            for(int i = 0; i < call->parse.callee->tabitha.parse.args.size(); i++)
            {
                if(call->parse.args[i]->common.parse.type != (Type*) &SupportedPrimitives::NONE && !typesMatch(call->parse.args[i]->common.parse.type, call->parse.callee->tabitha.parse.args[i]->common.parse.type))
                {
                    throw ArgTypeMismatch(call->parse.args[i]->common.parse.node->line, call->parse.args[i]->common.parse.node->column);
                } 
            }
        }
        else if(call->parse.callee->common.functionClass == FUNCTION_EXTERNAL)
        {
            if(call->parse.callee->external.parse.args.size() != call->parse.args.size())
            {
                throw CallIncorrectNumberOfArgs(node->line, node->column); 
            }
            for(int i = 0; i < call->parse.callee->external.parse.args.size(); i++)
            {
                if(call->parse.args[i]->common.parse.type != (Type*) &SupportedPrimitives::NONE && !typesMatch(call->parse.args[i]->common.parse.type, call->parse.callee->external.parse.args[i]))
                {
                    throw ArgTypeMismatch(call->parse.args[i]->common.parse.node->line, call->parse.args[i]->common.parse.node->column);
                } 
            }
        }
        return (Expression*) call;
    }
    NODE_OP(node, bracketedNode, "BRACKETED_EXPRESSION")
    {
        BracketedExpression* bracketed = new BracketedExpression(bracketedNode, hostBlock, hostSlab); 
        NODE_OP(bracketedNode, contentsNode, "EXPRESSION")
        {
            bracketed->parse.contents = parseExpression(contentsNode, hostBlock, hostSlab); 
        } 
        bracketed->common.parse.type = bracketed->parse.contents->common.parse.type;  
        return (Expression*) bracketed; 
    }
    throw ExpressionNotRecognised(node->line, node->column);
    return nullptr;

}

tabic::BinaryExpression* tabic::parseBinaryExpression(ASTNode node, Block* hostBlock, Slab* hostSlab)
{
    BinaryExpression* expression = new BinaryExpression(node, hostBlock, hostSlab);   
    NODE_LOOP(node, binarySub)
    {
        //The left-hand side of the binary expression must be a singleton.
        NODE_CHECK(binarySub, "SINGLETON_EXPRESSION")
        {
            expression->parse.lhs = parseSingletonExpression(binarySub, hostBlock, hostSlab);
        }
        //The right-hand side of the binary expression could be anything.
        NODE_CHECK(binarySub, "EXPRESSION")
        {
            expression->parse.rhs = parseExpression(binarySub, hostBlock, hostSlab);
        }
        //Get the operator acting between the LHS and RHS. 
        NODE_CHECK(binarySub, "BINARY_OPERATOR")
        {
            NODE_OP(binarySub, plusNode, "PLUS")
            {
                expression->parse.op = BINARY_OP_PLUS;
                expression->common.parse.type = expression->parse.lhs->common.parse.type;
            }
            NODE_OP(binarySub, subtractNode, "SUBTRACT")
            {
                expression->parse.op = BINARY_OP_SUB;
                expression->common.parse.type = expression->parse.lhs->common.parse.type;
            }
            NODE_OP(binarySub, mulNode, "MULTIPLY")
            {
                expression->parse.op = BINARY_OP_MUL;
                expression->common.parse.type = expression->parse.lhs->common.parse.type;
            }
            NODE_OP(binarySub, divNode, "DIVIDE")
            {
                expression->parse.op = BINARY_OP_DIV;
                expression->common.parse.type = expression->parse.lhs->common.parse.type;
            }
            NODE_OP(binarySub, ltNode, "LESS_THAN")
            {
                expression->parse.op = BINARY_OP_LT; 
                expression->common.parse.type = (Type*) &SupportedPrimitives::TRUTH;
            }
            NODE_OP(binarySub, gtNode, "GREATER_THAN")
            {
                expression->parse.op = BINARY_OP_GT; 
                expression->common.parse.type = (Type*) &SupportedPrimitives::TRUTH;
            }
            NODE_OP(binarySub, lteNode, "LESS_THAN_EQ")
            {
                expression->parse.op = BINARY_OP_LTE; 
                expression->common.parse.type = (Type*) &SupportedPrimitives::TRUTH;
            }
            NODE_OP(binarySub, gteNode, "GREATER_THAN_EQ") {
                expression->parse.op = BINARY_OP_GTE; 
                expression->common.parse.type = (Type*) &SupportedPrimitives::TRUTH;
            }
            NODE_OP(binarySub, eqNode, "EQUALS")
            {
                expression->parse.op = BINARY_OP_EQUALS;
                expression->common.parse.type = (Type*) &SupportedPrimitives::TRUTH;
            }
            NODE_OP(binarySub, neqNode, "NOT_EQUAL")
            {
                expression->parse.op = BINARY_OP_NOT_EQUAL;
                expression->common.parse.type = (Type*) &SupportedPrimitives::TRUTH; 
            }
        }
    }

    if(!expression->parse.lhs) return nullptr; 
    if(!expression->parse.rhs) return nullptr; 
    //Type checking.  
    if(!typesMatch(expression->parse.lhs->common.parse.type, expression->parse.rhs->common.parse.type))
    {
        throw OperatorTypeMismatch(expression, node->line, node->column);
    }
    if(typesMatch(expression->parse.lhs->common.parse.type, (Type*) &SupportedPrimitives::INT))
    {
        expression->parse.ep = EP_INT;
    }
    if(typesMatch(expression->parse.lhs->common.parse.type, (Type*) &SupportedPrimitives::FLOAT))
    {
        expression->parse.ep = EP_FLOAT;
    }
    if(typesMatch(expression->parse.lhs->common.parse.type, (Type*) &SupportedPrimitives::CHAR))
    {
        expression->parse.ep = EP_CHAR;
    }
    return expression;
}

tabic::Expression* tabic::parseExpression(ASTNode node, Block* hostBlock, Slab* hostSlab)
{
    try
    {
        NODE_OP(node, singletonNode, "SINGLETON_EXPRESSION")
        {
            return parseSingletonExpression(singletonNode, hostBlock, hostSlab);
        }
        NODE_OP(node, binaryNode, "BINARY_EXPRESSION")
        {
            return (Expression*) parseBinaryExpression(binaryNode, hostBlock, hostSlab);
        }
        throw ExpressionNotRecognised(node->line, node->column);
    }
    catch(ExpressionNotRecognised ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl; 
        PARSE_FAIL;
    }
    catch(OperatorTypeMismatch ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " <<  ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL; 
    }
    catch(ArgTypeMismatch ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL; 
    }
    catch(CallIncorrectNumberOfArgs ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(FunctionNotFound ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr;
}

tabic::StackedDeclaration* tabic::parseStackedDeclaration(ASTNode node, Block* hostBlock)
{
    StackedDeclaration* declaration = new StackedDeclaration(node, hostBlock); 
    try
    {
        //Need to know hostSlab because type references and Expression use Slab data. 
        Slab* hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        declaration->parse.variable = new StackedVariable(hostBlock); 
        NODE_OP(node, typeNode, "TYPE_REF")
        {
            declaration->parse.variable->common.parse.type = getOrCreateType(typeNode, hostBlock, hostSlab); 
        }
        NODE_OP(node, nameNode, "VARIABLE_NAME")
        {
            declaration->parse.variable->common.parse.name = nameNode->token_to_string(); 
        }
        hostBlock->parse.variables[declaration->parse.variable->common.parse.name] = (Variable*) declaration->parse.variable; 
        NODE_OP(node, initNode, "EXPRESSION")
        {
            declaration->parse.initialiser = parseExpression(initNode, hostBlock, hostSlab); 
            if(!typesMatch(declaration->parse.variable->common.parse.type, declaration->parse.initialiser->common.parse.type))
            {
                throw AssignmentTypeMismatch(
                        declaration->parse.variable->common.parse.type, 
                        declaration->parse.initialiser->common.parse.type, node->line, node->column);
            }
        }
        return declaration; 
    }
    catch(AssignmentTypeMismatch ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr;
}

tabic::HeapedDeclaration* tabic::parseHeapedDeclaration(ASTNode node, Block* hostBlock)
{
    HeapedDeclaration* declaration = new HeapedDeclaration(node, hostBlock); 
     try
    {
        //Need to know hostSlab because type references and Expression use Slab data. 
        Slab* hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        declaration->parse.variable = new HeapedVariable(hostBlock); 
        NODE_OP(node, typeNode, "TYPE_REF")
        {
            declaration->parse.variable->common.parse.type = getOrCreateType(typeNode, hostBlock, hostSlab); 
        }
        NODE_OP(node, nameNode, "VARIABLE_NAME")
        {
            declaration->parse.variable->common.parse.name = nameNode->token_to_string(); 
        }
        //Make the variable accessible to subsequent statements in the Block, as well as any subsequent child blocks. 
        hostBlock->parse.variables[declaration->parse.variable->common.parse.name] = (Variable*) declaration->parse.variable; 
        NODE_OP(node, initNode, "EXPRESSION")
        {
            declaration->parse.initialiser = parseExpression(initNode, hostBlock, hostSlab); 
            if(!typesMatch(declaration->parse.variable->common.parse.type, declaration->parse.initialiser->common.parse.type))
            {
                throw AssignmentTypeMismatch(
                        declaration->parse.variable->common.parse.type, 
                        declaration->parse.initialiser->common.parse.type, node->line, node->column);
            }
        }
        return declaration; 
    }
    catch(AssignmentTypeMismatch ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL; 
    }
    return declaration;
}


tabic::Assignment* tabic::parseAssignment(ASTNode node, Block* hostBlock)
{
    Assignment* assignment = new Assignment(hostBlock); 
    try
    {
        //Need to know hostSlab because Expression uses Slab data
        Slab* hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab; 
        NODE_OP(node, refNode, "VALUE_REF")
        {
            assignment->parse.ref = parseValueRef(refNode, hostBlock); 
            if(!assignment->parse.ref) return nullptr; 
        }
        NODE_OP(node, expressionNode, "EXPRESSION")
        {
            assignment->parse.expression = parseExpression(expressionNode, hostBlock, hostSlab);  
            if(!assignment->parse.expression) return nullptr; 
        }
        if(!typesMatch(assignment->parse.ref->common.parse.type, assignment->parse.expression->common.parse.type))
        {
            throw AssignmentTypeMismatch(assignment->parse.ref->common.parse.type, assignment->parse.expression->common.parse.type, node->line, node->column);
        }
        return assignment;
    }
    catch(AssignmentTypeMismatch ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr; 
}

tabic::ValueRef* tabic::parseValueRef(ASTNode node, Block* hostBlock)
{
    try
    {
        VariableRef* variableRef = new VariableRef(hostBlock->common.parse.hostFunction->create.hostSlab); 
        //Get the variable's Context, if any.
        Context* variableContext = nullptr; 
        NODE_OP(node, contextNode, "CONTEXT_REF")
        {
            variableContext = parseContextRef(contextNode, nullptr, hostBlock->common.parse.hostFunction);
        }
        //Get the variable's Dump, if any.
        Dump* variableDump = nullptr;
        NODE_OP(node, dumpNode, "DUMP_REF")
        {
            variableDump = parseDumpRef(dumpNode, hostBlock->common.parse.hostFunction->create.hostSlab);
        }
        //Get the variable name
        std::string variableName = "";
        NODE_OP(node, nameNode, "VARIABLE_NAME")
        {
            variableName = nameNode->token_to_string(); 
        }
        //If a Context has been given, then we search that Context for the Variable. 
        if(variableContext)
        {
            for(auto pair : variableContext->parse.members)
            {
                if(pair.first == variableName) 
                {
                    variableRef->parse.variable = (Variable*) pair.second;
                    break;
                }
            }
            if(!variableRef->parse.variable)
            {
                throw MemberNotFound(node->line, node->column); 
            }
        }
        //If a Dump is given, then we search that Dump for the Variable. 
        else if(variableDump)
        {
            for(auto pair : variableDump->parse.members)
            {
                if(pair.first == variableName) 
                {
                    variableRef->parse.variable = (Variable*) pair.second;
                    break;
                }
            }
            if(!variableRef->parse.variable)
            {
                throw MemberNotFound(node->line, node->column); 
            }
        }
        //Otherwise we search stacked variables. 
        else
        {
            Block* variableBlock = hostBlock;
            while(variableBlock && !variableRef->parse.variable)
            {
                for(auto pair : variableBlock->parse.variables)
                {
                    if(pair.first == variableName) variableRef->parse.variable = (Variable*) pair.second; 
                }
                variableBlock = variableBlock->common.parse.hostBlock; 
            }
            //If we reach this point without finding the Variable, we now need to search TabithaFunction args. 
            TabithaFunction* hostFunction = hostBlock->common.parse.hostFunction;
            if(!variableRef->parse.variable)
            {
                for(StackedVariable* arg : hostFunction->parse.args)
                {
                    if(arg->common.parse.name == variableName) variableRef->parse.variable = (Variable*) arg; 
                }
            }
            if(!variableRef->parse.variable) throw VariableNotFound(variableName, node->line, node->column); 
        }
        variableRef->common.parse.type = variableRef->parse.variable->common.parse.type; 
        //Now deal with subreferencing
        ValueRef* currentRef = (ValueRef*) variableRef; 
        NODE_LOOP(node, refSub)
        {
            NODE_CHECK(refSub, "VALUE_SUB_REF")
            {
                currentRef = parseValueSubRef(refSub, hostBlock, currentRef); 
            }
        }
        //Finally deal with querying. 
        NODE_OP(node, queryNode, "QUERY")
        {
            currentRef->common.parse.query = true; 
            if(currentRef->common.parse.type->common.typeClass == TYPE_ADDRESS) currentRef->common.parse.type = currentRef->common.parse.type->address.parse.pointsTo;
            else if(currentRef->common.parse.type->common.typeClass == TYPE_ALIAS
                    && currentRef->common.parse.type->alias.parse.repType->common.typeClass == TYPE_ADDRESS) 
            {
                currentRef->common.parse.type = currentRef->common.parse.type->alias.parse.repType->address.parse.pointsTo; 
            }
            else
            {
                throw QueryNotAddress(queryNode->line, queryNode->column); 
            }
        }
        return currentRef;
    }
    catch(VariableNotFound ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(MemberNotOfCollection ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(MemberNotFound ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(IndexNotOfVector ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(IndexNotInteger ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(IDNotInt ex)
    {
        std::cerr << ex.what() << std::endl;;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(FieldNotFound ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr; 
}

tabic::ValueRef* tabic::parseValueSubRef(ASTNode node, Block* block, ValueRef* parent)
{
    NODE_OP(node, memberNode,  "MEMBER_REF")
    {
        MemberRef* memberRef = new MemberRef(parent); 
        NODE_OP(memberNode, nameNode, "VARIABLE_NAME")
        {
            memberRef->parse.memberName = nameNode->token_to_string();
        }

        //determine whether the parent is collection type or at least points to a collection type
        Type* collectionType = parent->common.parse.type;
        if(parent->common.parse.type->common.typeClass != TYPE_COLLECTION)
        {
            while(collectionType->common.typeClass == TYPE_ADDRESS)
            {
                collectionType = collectionType->address.parse.pointsTo; 
            }
            if(collectionType->common.typeClass != TYPE_COLLECTION) throw MemberNotOfCollection(node->line, node->column); 
        }
        for(auto pair : collectionType->collection.parse.members)
        {
            if(pair.first == memberRef->parse.memberName)
            {
                memberRef->parse.memberIndex = pair.second.index; 
                memberRef->common.parse.type = pair.second.type; 
            }
        }
        if(memberRef->parse.memberIndex == -1)
        {
            throw MemberNotFound(node->line, node->column); 
        }
        return (ValueRef*) memberRef; 
    }
    NODE_OP(node, elemNode, "ELEMENT_REF")
    {
        ElementRef* elemRef = new ElementRef(parent); 
        NODE_OP(elemNode, indexNode, "EXPRESSION")
        {
            elemRef->parse.index = parseExpression(indexNode, block, nullptr); 

            
            //determine whether the index is valid
            if(!typesMatch(elemRef->parse.index->common.parse.type, (Type*) &SupportedPrimitives::INT))
            {
                throw IndexNotInteger(indexNode->line, indexNode->column);
            }
        }
        //determine whether the parent ref is a vector 
        //or at least points to a vector
        Type* vectorType = parent->common.parse.type; 
        while(vectorType->common.typeClass == TYPE_ADDRESS)
        {
            vectorType = vectorType->address.parse.pointsTo; 
        }
        if(vectorType->common.typeClass != TYPE_VECTOR)
        {
            throw IndexNotOfVector(node->line, node->column); 
        }
        elemRef->common.parse.type = vectorType->vector.parse.elemType; 
        return (ValueRef*) elemRef; 
    }
    NODE_OP(node, rowNode, "ROW_REF")
    {
        RowRef* rowRef = new RowRef(parent); 
        NODE_OP(rowNode, fieldNode, "VARIABLE_NAME")
        {
            rowRef->parse.fieldName = fieldNode->token_to_string(); 
        }
        NODE_OP(rowNode, expressionNode, "EXPRESSION")
        {
            rowRef->parse.id = parseExpression(expressionNode, block, nullptr); 
        }
        rowRef->parse.fieldIndex = -1; 
        for(TableField &field : parent->common.parse.type->table.parse.fields)
        {
            rowRef->parse.fieldIndex++; 
            if(field.name == rowRef->parse.fieldName) 
            {
                rowRef->common.parse.type = field.type; 
                break; 
            }
        }
        if(!rowRef->common.parse.type) throw FieldNotFound(parent->common.parse.type, rowRef->parse.fieldName, node->line, node->column); 
        if(!typesMatch(rowRef->parse.id->common.parse.type, (Type*) &SupportedPrimitives::INT))
        {
            throw IDNotInt(node->line, node->column);
        }
        return (ValueRef*) rowRef; 
    }
    return nullptr; 
}

void tabic::cleanBlock(Block* block)
{
    for(Statement* statement : block->parse.statements) if(statement) statement->destroy();
    delete block; 
}

void tabic::parseContext(Context* context)
{
    //Create all members
    NODE_LOOP(context->create.node, contextSub)
    {
        NODE_CHECK(contextSub, "CONTEXT_MEMBER")
        {
            ContextVariable* member = new ContextVariable(context); 
            NODE_OP(contextSub, typeNode, "TYPE_REF")
            {
                member->common.parse.type = getOrCreateType(typeNode, nullptr, context->create.hostSlab); 
            }
            NODE_OP(contextSub, nameNode, "VARIABLE_NAME")
            {
                member->common.parse.name = nameNode->token_to_string();     
            }
            NODE_OP(contextSub, initNode, "EXPRESSION")
            {
                member->parse.initialiser = parseExpression(initNode, nullptr, context->create.hostSlab); 
            }
            context->parse.members[member->common.parse.name] = member; 
        }
    }
}

tabic::Context* tabic::parseContextRef(ASTNode node, Slab* hostSlab, TabithaFunction* hostFunction)
{
    try
    {
        if(hostFunction) hostSlab = hostFunction->create.hostSlab;
        //Find out which Slab the context is in.
        std::string slabName = "";
        NODE_OP(node, slabNode, "SLAB_NAME")
        {
            slabName = slabNode->token_to_string();  
        }
        Slab* contextSlab = hostSlab;
        if(slabName != "")
        {
            contextSlab = nullptr; 
            for(auto pair : hostSlab->create.attachments)
            {
                if(slabName == pair.first)
                {
                    contextSlab = pair.second;
                    break;
                }
            }
            if(!contextSlab) throw SlabNotAttached(slabName, hostSlab, node->line, node->column);
        }
        //Find out the actual Context.
        std::string contextName = "";
        NODE_OP(node, nameNode, "CONTEXT_NAME")
        {
            contextName = nameNode->token_to_string(); 
        }
        Context* context = nullptr; 
        for(auto pair : contextSlab->create.contexts)
        {
            if(pair.first == contextName)
            {
                context = pair.second; 
                break;
            }
        }

        if(!context) throw ContextNotFound(contextName, contextSlab, node->line, node->column); 
        
        //If hostFunction is given, we check against the captures. 
        if(hostFunction)
        {
            for(Context* capture : hostFunction->parse.captures)
            {
                if(context == capture) return context;
            }
            throw ContextNotCaptured(context, hostFunction, node->line, node->column); 
        }
        return context; 
    }
    catch(SlabNotAttached ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Referenced slab: " << ex.slabName << "; Master slab: " << ex.hostSlab->create.id << std::endl; 
        PARSE_FAIL;
    }
    catch(ContextNotFound ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Context name: " << ex.contextName << "; Host slab: " << ex.contextSlab->create.id << std::endl; 
        PARSE_FAIL;
    }
    catch(ContextNotCaptured ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Context name: " << ex.context->create.name << "; Host Function: " << ex.function->create.name << std::endl; 
        PARSE_FAIL;
    }
    return nullptr; 
}

void tabic::parseDump(Dump* dump)
{
    //Create all members.
    NODE_LOOP(dump->create.node, contextSub)
    {
        NODE_CHECK(contextSub, "DUMP_MEMBER")
        {
            DumpVariable* member = new DumpVariable(dump); 
            NODE_OP(contextSub, typeNode, "TYPE_REF")
            {
                member->common.parse.type = getOrCreateType(typeNode, nullptr, dump->create.hostSlab); 
            }
            NODE_OP(contextSub, nameNode, "VARIABLE_NAME")
            {
                member->common.parse.name = nameNode->token_to_string();     
            }
            NODE_OP(contextSub, initNode, "EXPRESSION")
            {
                member->parse.initialiser = parseExpression(initNode, nullptr, dump->create.hostSlab); 
            }
            dump->parse.members[member->common.parse.name] = member; 
        }
    }
}

tabic::Dump* tabic::parseDumpRef(ASTNode node, Slab* hostSlab)
{
    try
    {
        //Find out which Slab the context is in.
        std::string slabName = "";
        NODE_OP(node, slabNode, "SLAB_NAME")
        {
            slabName = slabNode->token_to_string();  
        }
        Slab* dumpSlab = hostSlab;
        if(slabName != "")
        {
            dumpSlab = nullptr; 
            for(auto pair : hostSlab->create.attachments)
            {
                if(slabName == pair.first)
                {
                    dumpSlab = pair.second;
                    break;
                }
            }
            if(!dumpSlab) throw SlabNotAttached(slabName, hostSlab, node->line, node->column);
        }
        //Find out the actual Context.
        std::string dumpName = "";
        NODE_OP(node, nameNode, "DUMP_NAME")
        {
            dumpName = nameNode->token_to_string(); 
        }
        Dump* dump = nullptr;
        for(auto pair : dumpSlab->create.dumps)
        {
            if(pair.first == dumpName)
            {
                dump = pair.second; 
                break;
            }
        }

        if(!dump) throw DumpNotFound(dumpName, dumpSlab, node->line, node->column); 
        return dump; 
    }
    catch(SlabNotAttached ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Referenced slab: " << ex.slabName << "; Master slab: " << ex.hostSlab->create.id << std::endl; 
        PARSE_FAIL;
    }
    catch(DumpNotFound ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Dump name: " << ex.dumpName << "; Host slab: " << ex.dumpSlab->create.id << std::endl; 
        PARSE_FAIL;
    }
    return nullptr; 
}

tabic::Function* tabic::parseFunctionRef(ASTNode node, Slab* hostSlab)
{
    //Get the functionSlab. 
    Slab* functionSlab = hostSlab; 
    std::string slabName = ""; 
    NODE_OP(node, slabNode, "SLAB_NAME")
    {
        slabName = slabNode->token_to_string(); 
    }
    if(slabName != "")
    {
        functionSlab = nullptr; 
        for(auto pair : hostSlab->create.attachments)
        {
            if(slabName == pair.first)
            {
                functionSlab = pair.second; 
            }
        }
    }
    if(!functionSlab) throw SlabNotAttached(slabName, hostSlab, node->line, node->column); 
    //Get the function. 
    std::string functionName = "";
    NODE_OP(node, functionNameNode, "FUNCTION_NAME")
    {
        functionName = functionNameNode->token_to_string();
    }
    for(auto pair : functionSlab->create.functions)
    {
        if(functionName == pair.first)
        {
            return pair.second;
        }
    }
    throw FunctionNotFound(functionName, functionSlab, node->line, node->column); 
}

tabic::ProcedureCall* tabic::parseProcedureCall(ASTNode node, Block* hostBlock)
{
    try {
        Slab* hostSlab = hostBlock->common.parse.hostFunction->create.hostSlab;
        ProcedureCall* call = new ProcedureCall(node, hostBlock); 
        //Get the Function which is called. 
        NODE_OP(node, functionRefNode, "FUNCTION_REF")
        {
            call->parse.callee = parseFunctionRef(functionRefNode, hostSlab);
        }
        //Get the arguments. 
        NODE_LOOP(node, functionSub)
        {
            NODE_CHECK(functionSub, "EXPRESSION")
            {
                Expression* arg = parseExpression(functionSub, hostBlock, nullptr);
                if(!arg) return nullptr; 
                call->parse.args.push_back(arg); 
            }
        }
        if(!call->parse.callee) return nullptr; 
        parseFunction(call->parse.callee); //ensure the callee has been parsed already
        if(call->parse.callee->common.functionClass == FUNCTION_TABITHA)
        { 
            if(call->parse.callee->tabitha.parse.args.size() != call->parse.args.size())
            {
                throw CallIncorrectNumberOfArgs(node->line, node->column); 
            }
            for(int i = 0; i < call->parse.callee->tabitha.parse.args.size(); i++)
            {
                if(call->parse.args[i]->common.parse.type != (Type*) &SupportedPrimitives::NONE && !typesMatch(call->parse.args[i]->common.parse.type, call->parse.callee->tabitha.parse.args[i]->common.parse.type))
                {
                    throw ArgTypeMismatch(call->parse.args[i]->common.parse.node->line, call->parse.args[i]->common.parse.node->column);
                } 
            }
        }
        else if(call->parse.callee->common.functionClass == FUNCTION_EXTERNAL)
        {
            if(call->parse.callee->external.parse.args.size() != call->parse.args.size())
            {
                throw CallIncorrectNumberOfArgs(node->line, node->column); 
            }
            for(int i = 0; i < call->parse.callee->external.parse.args.size(); i++)
            {
                if(call->parse.args[i]->common.parse.type != (Type*) &SupportedPrimitives::NONE && !typesMatch(call->parse.args[i]->common.parse.type, call->parse.callee->external.parse.args[i]))
                {
                    throw ArgTypeMismatch(call->parse.args[i]->common.parse.node->line, call->parse.args[i]->common.parse.node->column);
                } 
            }
        }
        return call;
    }
    catch(ArgTypeMismatch ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(CallIncorrectNumberOfArgs ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(FunctionNotFound ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr; 
}

tabic::Conditional* tabic::parseConditional(ASTNode node, Block* hostBlock)
{
    Conditional* conditional = new Conditional(node, hostBlock);
    try
    {
        NODE_OP(node, expressionNode, "EXPRESSION")
        {
            conditional->parse.pair.expression = parseExpression(expressionNode, hostBlock, nullptr); 
        }
        NODE_OP(node, blockNode, "BLOCK")
        {
            conditional->parse.pair.block = parseBlock(blockNode, hostBlock, nullptr); 
        }
        if(conditional->parse.pair.expression->common.parse.type != (Type*) &SupportedPrimitives::TRUTH)
        {
            throw ConditionNotTruth(node->line, node->column);
        }
    }
    catch(ConditionNotTruth ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return conditional;
}

tabic::Branch* tabic::parseBranch(ASTNode node, Block* hostBlock)
{
    Branch* branch = new Branch(node, hostBlock);
    try
    {
        ConditionBlockPair pair; 
        NODE_LOOP(node, branchSub)
        {
            NODE_CHECK(branchSub, "EXPRESSION")
            {
                pair.expression = parseExpression(branchSub, hostBlock, nullptr); 
                if(pair.expression->common.parse.type != (Type*) &SupportedPrimitives::TRUTH)
                {
                    throw ConditionNotTruth(node->line, node->column);
                }
            }
            NODE_CHECK(branchSub, "BLOCK")
            {
                pair.block = parseBlock(branchSub, hostBlock, nullptr); 
                if(pair.expression)
                {
                    branch->parse.twigs.push_back(pair); 
                    pair.expression = nullptr; 
                }
                else
                {
                    branch->parse.otherwiseBlock = pair.block;
                }
            }
        }
        return branch;
    }
    catch(ConditionNotTruth ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr;
}

tabic::Loop* tabic::parseLoop(ASTNode node, Block* hostBlock)
{
    Loop* loop = new Loop(node, hostBlock); 
    NODE_OP(node, conditionNode, "EXPRESSION")
    {
        loop->parse.condition= parseExpression(conditionNode, hostBlock, nullptr); 
    }
    NODE_OP(node, directionsNode, "BLOCK")
    {
        loop->parse.directions = parseBlock(directionsNode, hostBlock, nullptr);
    }
    return loop;
}

tabic::Label* tabic::parseLabel(ASTNode node, Block* hostBlock)
{
    Label* label = new Label(node, hostBlock); 
    NODE_OP(node, addrNode, "EXPRESSION")
    {
        label->parse.address = parseExpression(addrNode, hostBlock, nullptr);
    }
    NODE_OP(node, vecNode, "VALUE_REF")
    {
        label->parse.fuzzyRef = parseValueRef(vecNode, hostBlock);
    }
    return label;
}

tabic::VectorSet* tabic::parseVectorSet(ASTNode node, Block* hostBlock)
{
    VectorSet* vectorSet = new VectorSet(node, hostBlock); 
    try
    {
        //First find out the vector we are setting.
        NODE_OP(node, vecRefNode, "VALUE_REF")
        {
            vectorSet->parse.vectorRef = parseValueRef(vecRefNode, hostBlock);
            if(!vectorSet->parse.vectorRef) return nullptr; 
            if(vectorSet->parse.vectorRef->common.parse.type->common.typeClass != TYPE_VECTOR)
            {
                throw VectorRefNotVector(node->line, node->column); 
            }
        }
        //Then the entry point of our setting operation.
        NODE_OP(node, fromNode, "FROM_INDEX")
        {
            NODE_OP(fromNode, expressionNode, "EXPRESSION")
            {
                vectorSet->parse.from = parseExpression(expressionNode, hostBlock, nullptr); 
                if(!typesMatch(vectorSet->parse.from->common.parse.type, (Type*)&SupportedPrimitives::INT))
                {
                    throw IndexNotInteger(node->line, node->column); 
                }
            }
        }
        //Finally parse the expressions we are setting. 
        NODE_LOOP(node, subNode)
        {
            NODE_CHECK(subNode, "EXPRESSION")
            {
                vectorSet->parse.elements.push_back(parseExpression(subNode, hostBlock, nullptr));
            }
        }
        return vectorSet;
    }
    catch(VectorRefNotVector ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(IndexNotInteger ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr;
}

tabic::TableInsert* tabic::parseTableInsert(ASTNode node, Block* hostBlock)
{
    TableInsert* tableInsert = new TableInsert(node, hostBlock); 
    try
    {
        NODE_OP(node, tableNode, "TABLE_REF")
        {
            NODE_OP(tableNode, valueNode, "VALUE_REF")
            {
                tableInsert->parse.tableRef = parseValueRef(valueNode, hostBlock);
                if(!tableInsert->parse.tableRef) return nullptr; 
                if(tableInsert->parse.tableRef->common.parse.type->common.typeClass != TYPE_TABLE)
                {
                    throw TableRefNotTable(node->line, node->column); 
                }
            }
        }
        NODE_OP(node, tableNode, "ID_REF")
        {
            NODE_OP(tableNode, valueNode, "VALUE_REF")
            {
                tableInsert->parse.idRef = parseValueRef(valueNode, hostBlock);
                if(!tableInsert->parse.idRef) return nullptr; 
                if(!typesMatch(tableInsert->parse.idRef->common.parse.type, (Type*) &SupportedPrimitives::INT))
                {
                    throw IDRefNotInt(node->line, node->column); 
                }
            }
        }
        NODE_LOOP(node, insertSub)
        {
            NODE_CHECK(insertSub, "EXPRESSION")
            {
                Expression* element = parseExpression(insertSub, hostBlock, nullptr);
                if(!element) return nullptr; 
                tableInsert->parse.elements.push_back(element); 
            }
            NODE_CHECK(insertSub, "NULL")
            {
                tableInsert->parse.elements.push_back((Expression*) new NullValue(insertSub, hostBlock, nullptr)); 
            }
        }
        return tableInsert; 
    }
    catch(TableRefNotTable ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(IDRefNotInt ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr; 
}

tabic::TableDelete* tabic::parseTableDelete(ASTNode node, Block* hostBlock)
{
    TableDelete* tableDelete = new TableDelete(node, hostBlock); 
    try
    {
        NODE_OP(node, tableRefNode, "VALUE_REF")
        {
            tableDelete->parse.tableRef = parseValueRef(tableRefNode, hostBlock);
            if(!tableDelete->parse.tableRef) return nullptr; 
            if(tableDelete->parse.tableRef->common.parse.type->common.typeClass != TYPE_TABLE) throw TableRefNotTable(node->line, node->column); 
        }
        NODE_OP(node, idNode, "EXPRESSION")
        {
            tableDelete->parse.id = parseExpression(idNode, hostBlock, nullptr); 
            if(!tableDelete->parse.id) return nullptr; 
            if(!typesMatch(tableDelete->parse.id->common.parse.type, (Type*) &SupportedPrimitives::INT)) throw IDNotInt(node->line, node->column);  
        }
        return tableDelete; 
    }
    catch(TableRefNotTable ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(IDNotInt ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr;
}

tabic::TableMeasure* tabic::parseTableMeasure(ASTNode node, Block* hostBlock)
{
    TableMeasure* tableMeasure = new TableMeasure(node, hostBlock);
    try
    {
        NODE_OP(node, tableRefNode, "TABLE_REF")
        {
            tableMeasure->parse.tableRef = parseValueRef(tableRefNode->nodes[0], hostBlock); 
            if(!tableMeasure->parse.tableRef) return nullptr; 
            if(tableMeasure->parse.tableRef->common.parse.type->common.typeClass != TYPE_TABLE) throw TableRefNotTable(tableRefNode->line, tableRefNode->column); 
        }
        NODE_OP(node, measureNode, "VALUE_REF")
        {
            tableMeasure->parse.usedRef = parseValueRef(measureNode, hostBlock);
            if(!tableMeasure->parse.usedRef) return nullptr; 
            if(!typesMatch(tableMeasure->parse.usedRef->common.parse.type, (Type*) &SupportedPrimitives::INT)) throw MeasureNotInteger(measureNode->line, measureNode->column); 
        }
        return tableMeasure; 
    }
    catch(TableRefNotTable ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(MeasureNotInteger ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr;
}

tabic::TableCrunch* tabic::parseTableCrunch(ASTNode node, Block* hostBlock)
{
    TableCrunch* tableCrunch = new TableCrunch(node, hostBlock);
    try
    {
        NODE_OP(node, tableRefNode, "TABLE_REF")
        {
            tableCrunch->parse.tableRef = parseValueRef(tableRefNode->nodes[0], hostBlock); 
            if(!tableCrunch->parse.tableRef) return nullptr; 
            if(tableCrunch->parse.tableRef->common.parse.type->common.typeClass != TYPE_TABLE) throw TableRefNotTable(tableRefNode->line, tableRefNode->column); 
        }
        NODE_OP(node, valueRefNode, "VALUE_REF")
        {
            tableCrunch->parse.idRef = parseValueRef(valueRefNode, hostBlock); 
            if(!tableCrunch->parse.idRef) return nullptr; 
            if(!typesMatch(tableCrunch->parse.idRef->common.parse.type, (Type*) &SupportedPrimitives::INT))
            {
                throw IDRefNotInt(valueRefNode->line, valueRefNode->column);
            }
        }
        return tableCrunch;
    }
    catch(TableRefNotTable ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    catch(IDRefNotInt ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr;
}

tabic::Unheap* tabic::parseUnheap(ASTNode node, Block* hostBlock)
{
    try
    {
        Unheap* unheap = new Unheap(node, hostBlock); 
        NODE_OP(node, addressNode, "EXPRESSION")
        {
            unheap->parse.address = parseExpression(addressNode, hostBlock, nullptr); 
            if(unheap->parse.address->common.parse.type->common.typeClass != TYPE_ADDRESS)
            {
                throw UnheapExpressionNotAddress(node->line, node->column); 
            }
        }
        NODE_OP(node, typeNode, "TYPE_REF")
        {
            unheap->parse.structure = getOrCreateType(typeNode, hostBlock, nullptr); 
            if(!unheap->parse.structure) return nullptr; 
        }
        return unheap;
    }
    catch (UnheapExpressionNotAddress ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Line: " << ex.lineNum << "; Col: " << ex.colNum << std::endl;
        PARSE_FAIL;
    }
    return nullptr; 
}

bool tabic::typesMatch(Type* a, Type* b)
{
    while(a->common.typeClass == TYPE_ALIAS) a = a->alias.parse.repType;
    while(b->common.typeClass == TYPE_ALIAS) b = b->alias.parse.repType;
    //Firstly, if the TypeClass are different, then the Type cannot possibly be equivalent. 
    if(a->common.typeClass != b->common.typeClass) return false; 
    TypeClass typeClass = a->common.typeClass;
    if(typeClass == TYPE_PRIMITIVE)
    {
        if(a != b) return false; 
    }
    else if(typeClass == TYPE_COLLECTION)
    {
        if(a != b) return false; 
    }
    else if(typeClass == TYPE_ADDRESS)
    {
        if(!typesMatch(a->address.parse.pointsTo, b->address.parse.pointsTo)) return false; 
    }
    return true; 
}

