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
#include"tabic/build.hpp"

#include"tabic/model/model.hpp"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/BasicBlock.h"
#include"llvm/IR/Type.h"
#include"llvm/IR/DerivedTypes.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/Value.h"
#include"llvm/IR/DataLayout.h"


#define WINDOWS 

/**
 * Elements such as calling convention are determined by the system. 
 */
#ifdef WINDOWS
static llvm::CallingConv::ID TABITHA_CALLING_CONVENTION = llvm::CallingConv::Win64;
#else
static llvm::CallingConv::ID TABITHA_CALLING_CONVENTION = llvm::CallingConv::X86_64_SysV; 
#endif

static llvm::LLVMContext llvmContext; 
static llvm::IRBuilder<> builder(llvmContext);

void tabic::buildBundle(Bundle* bundle)
{
    //NOTE: We do not build slab-wise, because e.g. some Type are needed by other Slab.
    //Build the primitive types
    if(sizeof(size_t) == 4) SupportedPrimitives::SIZE.common.build.llvmType = llvm::Type::getInt32Ty(llvmContext);
    else SupportedPrimitives::SIZE.common.build.llvmType = llvm::Type::getInt64Ty(llvmContext);
    SupportedPrimitives::INT.common.build.llvmType = llvm::Type::getInt32Ty(llvmContext);
    SupportedPrimitives::LONG.common.build.llvmType = llvm::Type::getInt64Ty(llvmContext);
    SupportedPrimitives::SHORT.common.build.llvmType = llvm::Type::getInt16Ty(llvmContext);
    SupportedPrimitives::FLOAT.common.build.llvmType = llvm::Type::getFloatTy(llvmContext);
    SupportedPrimitives::DOUBLE.common.build.llvmType = llvm::Type::getDoubleTy(llvmContext); 
    SupportedPrimitives::CHAR.common.build.llvmType = llvm::Type::getInt8Ty(llvmContext);
    SupportedPrimitives::TRUTH.common.build.llvmType = llvm::Type::getInt1Ty(llvmContext);
    SupportedPrimitives::NONE.common.build.llvmType = llvm::Type::getVoidTy(llvmContext);

    //Build the core functions.
    {
        std::vector<llvm::Type*> argTypes = {SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()->getPointerTo(), SupportedPrimitives::INT.common.build.llvmType}; 
        TabiCore::TABLE_INIT.build.functionType = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType, llvm::ArrayRef(argTypes), false); 
    }
    {
        std::vector<llvm::Type*> argTypes = {SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()->getPointerTo(), 
            SupportedPrimitives::INT.common.build.llvmType, SupportedPrimitives::INT.common.build.llvmType->getPointerTo()}; 
        TabiCore::TABLE_INSERT.build.functionType = llvm::FunctionType::get(SupportedPrimitives::INT.common.build.llvmType, llvm::ArrayRef(argTypes), false); 
    }
    {
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()->getPointerTo(), 
            SupportedPrimitives::INT.common.build.llvmType, SupportedPrimitives::INT.common.build.llvmType}; 
        TabiCore::TABLE_GET_BY_ID.build.functionType = llvm::FunctionType::get(SupportedPrimitives::INT.common.build.llvmType, llvm::ArrayRef(argTypes), false); 
    }
    {
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()->getPointerTo(),
            SupportedPrimitives::INT.common.build.llvmType,
            SupportedPrimitives::INT.common.build.llvmType
        };
        TabiCore::TABLE_DELETE_BY_ID.build.functionType = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType, llvm::ArrayRef(argTypes), false);
    }
    {
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()->getPointerTo(),
            SupportedPrimitives::INT.common.build.llvmType
        };
        TabiCore::TABLE_GET_NUM_USED.build.functionType = llvm::FunctionType::get(SupportedPrimitives::INT.common.build.llvmType, llvm::ArrayRef(argTypes), false);
    }
    {
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()->getPointerTo(),
            SupportedPrimitives::INT.common.build.llvmType,
            SupportedPrimitives::INT.common.build.llvmType,
            SupportedPrimitives::INT.common.build.llvmType->getPointerTo(),
            SupportedPrimitives::INT.common.build.llvmType->getPointerTo()
        };
        TabiCore::TABLE_CRUNCH.build.functionType = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType, llvm::ArrayRef(argTypes), false);
    }
    {
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::INT.common.build.llvmType
        };
        TabiCore::ALLOC.build.functionType = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType->getPointerTo(), llvm::ArrayRef(argTypes), false);
    }
    {
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()
        };
        TabiCore::DEALLOC.build.functionType = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType, llvm::ArrayRef(argTypes), false); 
    }
    {
        llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType; 
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()->getPointerTo()->getPointerTo(),
            int_type, int_type, int_type, int_type
        };
        TabiCore::SUBVECTOR_COPY.build.functionType = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType, llvm::ArrayRef(argTypes), false); 
    }
    /*
    {
        std::vector<llvm::Type*> argTypes = {
            SupportedPrimitives::CHAR.common.build.llvmType->getPointerTo(), SupportedPrimitives::CHAR.common.build.llvmType->getPointerTo(), SupportedPrimitives::INT.common.build.llvmType
        };
        TabiCore::MEMCPY.build.functionType = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType, llvm::ArrayRef(argTypes), false); 
    }
    */
    

    //Create all LLVM Modules and create all init and destroy functions
    for(auto pair : bundle->create.slabs)
    {
        Slab* slab = pair.second;
        slab->build.llvmModule = new llvm::Module(slab->create.name + ".ll", llvmContext);
    }
    
    //Build all top-level Types.
    for(auto pair : bundle->create.slabs)
    {
        Slab* slab = pair.second;
        for(auto pair : slab->create.types)
        {
            Type* type = pair.second; 
            buildType(type); 
        }
    }
    //set up init and destroy
    {
        llvm::FunctionType* ftype = llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType, false); 
        bundle->build.init = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, "_tabi_init", bundle->create.rootSlab->build.llvmModule); 
        bundle->build.init->setCallingConv(TABITHA_CALLING_CONVENTION); 
        bundle->build.initEntry = llvm::BasicBlock::Create(llvmContext, "entry", bundle->build.init); 
        bundle->build.initExit = bundle->build.initEntry;
        bundle->build.destroy = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, "_tabi_destroy", bundle->create.rootSlab->build.llvmModule);
        bundle->build.destroy->setCallingConv(TABITHA_CALLING_CONVENTION); 
        bundle->build.destroyEntry = llvm::BasicBlock::Create(llvmContext, "entry", bundle->build.destroy); 
    }
    //Build all Context and Dump
    for(auto pair : bundle->create.slabs)
    {
        Slab* slab = pair.second; 
        for(auto pair : slab->create.contexts)
        {
            Context* context = pair.second;
            buildContext(context);
        }
        for(auto pair : slab->create.dumps)
        {
            Dump* dump = pair.second; 
            buildDump(dump);
        }
    }
    //terminate init and destroy functions
    {
        builder.SetInsertPoint(bundle->build.initExit); 
        builder.CreateRetVoid();
        builder.SetInsertPoint(bundle->build.destroyEntry); 
        builder.CreateRetVoid();
    }
    //Register all Function.
    for(auto pair : bundle->create.slabs)
    {
        Slab* slab = pair.second;
        for(auto pair : slab->create.functions)
        {
            Function* function = pair.second;
            registerFunction(function);
        }
    }
    //Build all Function.
    for(auto pair : bundle->create.slabs)
    {
        Slab* slab = pair.second;
        for(auto pair : slab->create.functions)
        {
            Function* function = pair.second;
            if(function->common.functionClass == FUNCTION_TABITHA) buildTabithaFunction((TabithaFunction*) function);
            //NOTE: ExternalFunction is not built in the sense meant here. 
            //This is because by defnition they are given elsewhere. 
        }
    }
}

void tabic::buildCollectionType(CollectionType* type)
{
    //Build each of the member types and get a vector of their LLVM types. 
    std::vector<llvm::Type*> llvmMemberTypes(type->parse.members.size());
    for(auto pair : type->parse.members)
    {
        CollectionMember member = pair.second;
        buildType(member.type);
        llvmMemberTypes[member.index] = member.type->common.build.llvmType;
    }
    //The corresponding LLVM type for this CollectionType is a struct type.
    type->common.build.llvmType = llvm::StructType::get(llvmContext, llvm::ArrayRef(llvmMemberTypes));
}
void tabic::buildType(Type* type)
{
    //If llvmType is not `nullptr`, then we need not build.
    if(type->common.build.llvmType) return;
    TypeClass typeClass = type->common.typeClass;
    if(typeClass == TYPE_COLLECTION) buildCollectionType((CollectionType*) type);
    else if(typeClass == TYPE_ADDRESS)
    {
        buildType(type->address.parse.pointsTo);
        //The Correspoinding LLVM type for this AddressType is the pointer type to pointsTo
        type->common.build.llvmType = type->address.parse.pointsTo->common.build.llvmType->getPointerTo();
    }
    else if(typeClass == TYPE_VECTOR)
    {
        buildType(type->vector.parse.elemType);
        //if(type->vector.parse.numElem) buildExpression(type->vector.parse.numElem); //postpone until declaration
        type->common.build.llvmType = type->vector.parse.elemType->common.build.llvmType->getPointerTo();
    }
    else if(typeClass == TYPE_TABLE)
    {
        buildExpression(type->table.parse.numRows);
        std::vector<llvm::Type*> llvmFieldTypes = {}; 
        for(TableField &field : type->table.parse.fields)
        {
            buildType(field.type); 
            llvmFieldTypes.push_back(field.type->common.build.llvmType->getPointerTo());
        }
        type->common.build.llvmType = llvm::StructType::get(llvmContext, llvm::ArrayRef(llvmFieldTypes));
    }
    else if(typeClass == TYPE_ALIAS)
    {
        buildType(type->alias.parse.repType); 
        std::memcpy(type, type->alias.parse.repType, sizeof(Type));  
    }
}

void tabic::buildBlock(Block* block)
{
    for(Statement* statement : block->parse.statements)
    {
        StatementClass statementClass = statement->common.statementClass;
        if(statementClass == STATEMENT_RETURN)
        {

            TabithaFunction* function = block->common.parse.hostFunction;
            std::vector<llvm::Type*> argTypes = { SupportedPrimitives::NONE.common.build.llvmType->getPointerTo() } ;
            llvm::FunctionCallee stackRestore = function->create.hostSlab->build.llvmModule->getOrInsertFunction("llvm.stackrestore", 
                llvm::FunctionType::get(SupportedPrimitives::SupportedPrimitives::NONE.common.build.llvmType->getPointerTo(), llvm::ArrayRef(argTypes), false)); 
            std::vector<llvm::Value*> args = { function->build.stackState }; 
            builder.CreateCall(stackRestore, llvm::ArrayRef(args)); 
            buildExpression(statement->ret.parse.expression);
            builder.CreateRet(statement->ret.parse.expression->common.build.llvmValue);
        }
        else if(statementClass == STATEMENT_STACKED_DECLARATION)
        {
            if(statement->localDeclaration.parse.variable->common.parse.type->common.typeClass == TYPE_VECTOR 
                    && statement->localDeclaration.parse.variable->common.parse.type->vector.parse.numElem)
            {
                llvm::Value* arrayStore = allocateStackVectorElements(statement->localDeclaration.parse.variable->common.parse.type, block->common.parse.hostFunction); 
                builder.CreateStore(arrayStore, statement->localDeclaration.parse.variable->common.build.llvmStore); 
            }
            else if(statement->localDeclaration.parse.variable->common.parse.type->common.typeClass == TYPE_TABLE)
            {
                allocateStackTableFields(statement->localDeclaration.parse.variable->common.parse.type, block->common.parse.hostFunction, statement->localDeclaration.parse.variable->common.build.llvmStore); 
            }
            if(statement->localDeclaration.parse.initialiser)
            {
                buildExpression(statement->localDeclaration.parse.initialiser); 
                //The associated StackedVariable should already be allocated. 
                builder.CreateStore(
                        statement->localDeclaration.parse.initialiser->common.build.llvmValue,
                        statement->localDeclaration.parse.variable->common.build.llvmStore);
            }
        }
        else if(statementClass == STATEMENT_HEAPED_DECLARATION)
        {
            buildHeapedDeclaration((HeapedDeclaration*) statement); 
            if(statement->heapedDeclaration.parse.initialiser)
            {
                buildExpression(statement->heapedDeclaration.parse.initialiser); 
                llvm::Value* ptr = builder.CreateLoad(statement->heapedDeclaration.parse.variable->common.parse.type->common.build.llvmType->getPointerTo(), statement->heapedDeclaration.parse.variable->common.build.llvmStore);
                builder.CreateStore(
                        statement->heapedDeclaration.parse.initialiser->common.build.llvmValue,
                        ptr); 
            }
        }
        else if(statementClass == STATEMENT_ASSIGNMENT)
        {
            buildExpression(statement->assignment.parse.expression); 
            buildValueRef(statement->assignment.parse.ref, nullptr); 
            Type* type = statement->assignment.parse.ref->common.parse.type; 
            TypeClass typeClass = type->common.typeClass;
            if(typeClass == TYPE_PRIMITIVE | typeClass == TYPE_COLLECTION | typeClass == TYPE_ADDRESS)
            {
                builder.CreateStore(
                        statement->assignment.parse.expression->common.build.llvmValue, 
                        statement->assignment.parse.ref->common.build.llvmStore); 
            }
            else if(typeClass == TYPE_VECTOR)
            {
                if(type->vector.parse.numElem)
                {
                    llvm::Value* numBytes = statement->assignment.parse.ref->common.parse.type->vector.parse.numElem->common.build.llvmValue;
                    llvm::DataLayout dl = block->common.parse.hostFunction->create.hostSlab->build.llvmModule->getDataLayout();
                    llvm::Value* arrayStore = builder.CreateLoad(type->common.build.llvmType, statement->assignment.parse.ref->common.build.llvmStore); 
                    numBytes = builder.CreateMul(
                            numBytes,
                            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(type->vector.parse.elemType->common.build.llvmType))));
                    builder.CreateMemCpy(
                            arrayStore, llvm::MaybeAlign{}, 
                            statement->assignment.parse.expression->common.build.llvmValue, llvm::MaybeAlign{},
                            numBytes, false);
                }
                else 
                {
                    builder.CreateStore(
                            statement->assignment.parse.expression->common.build.llvmValue,
                            statement->assignment.parse.ref->common.build.llvmStore); 
                }
            }
        }
        else if(statementClass == STATEMENT_PROCEDURE_CALL)
        {
            //Build the arguments and get their LLVM values. 
            std::vector<llvm::Value*> llvmArgs = {};
            for(Expression* arg : statement->procedureCall.parse.args)
            {
                buildExpression(arg); 
                llvmArgs.push_back(arg->common.build.llvmValue); 
            }
            Slab* hostSlab = block->common.parse.hostFunction->create.hostSlab;
            llvm::FunctionCallee callee = hostSlab->build.llvmModule->getOrInsertFunction(
                    statement->procedureCall.parse.callee->common.build.fullName,
                    statement->procedureCall.parse.callee->common.build.llvmFunction->getFunctionType());
            builder.CreateCall(callee, llvm::ArrayRef(llvmArgs));
        }
        else if(statementClass == STATEMENT_CONDITIONAL)
        {
            buildConditional((Conditional*)statement);
        }
        else if(statementClass == STATEMENT_BRANCH)
        {
            buildBranch((Branch*) statement); 
        }
        else if(statementClass == STATEMENT_LOOP)
        {
            buildLoop((Loop*) statement); 
        }
        else if(statementClass == STATEMENT_VECTOR_SET)
        {
            buildVectorSet((VectorSet*) statement); 
        }
        else if(statementClass == STATEMENT_TABLE_INSERT)
        {
            buildTableInsert((TableInsert*) statement); 
        }
        else if(statementClass == STATEMENT_TABLE_DELETE)
        {
            buildTableDelete((TableDelete*) statement); 
        }
        else if(statementClass == STATEMENT_TABLE_MEASURE)
        {
            buildTableMeasure((TableMeasure*) statement); 
        }
        else if(statementClass == STATEMENT_TABLE_CRUNCH)
        {
            buildTableCrunch((TableCrunch*) statement); 
        }
        else if(statementClass == STATEMENT_LABEL)
        {
            buildLabel((Label*) statement);
        }
        else if(statementClass == STATEMENT_UNHEAP)
        {
            buildUnheap((Unheap*) statement); 
        }
        else if(statementClass == STATEMENT_BLOCK)
        {
            buildBlock((Block*) statement); 
        }
    }
}

void tabic::buildVectorSet(VectorSet* vectorSet)
{
    buildValueRef(vectorSet->parse.vectorRef, nullptr); 
    buildExpression(vectorSet->parse.from); 
    llvm::Value* index = vectorSet->parse.from->common.build.llvmValue; 
    llvm::Value* one   = llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 1)); 
    Type* vectorType = vectorSet->parse.vectorRef->common.parse.type; 
    Type* elemType = vectorType->vector.parse.elemType; 
    llvm::Value* vectorStore = builder.CreateLoad(vectorType->common.build.llvmType, vectorSet->parse.vectorRef->common.build.llvmStore); 
    for(Expression* element : vectorSet->parse.elements)
    {
        std::vector<llvm::Value*> offsets = { index }; 
        llvm::Value* elemStore = builder.CreateGEP(elemType->common.build.llvmType, vectorStore, llvm::ArrayRef(offsets));
        buildExpression(element); 
        builder.CreateStore(element->common.build.llvmValue, elemStore); 
        index = builder.CreateAdd(index, one); 
    }
}

void tabic::buildTableInsert(TableInsert* tableInsert)
{
    buildValueRef(tableInsert->parse.tableRef, nullptr);
    if(tableInsert->parse.idRef) buildValueRef(tableInsert->parse.idRef, nullptr);
    //First get the relevant row. 
    llvm::Value* row; 
    {
        std::vector<llvm::Value*> args;
        if(tableInsert->parse.idRef)
        {
            args = {
                tableInsert->parse.tableRef->common.build.llvmStore,
                tableInsert->parse.tableRef->common.parse.type->table.parse.numRows->common.build.llvmValue,
                tableInsert->parse.idRef->common.build.llvmStore
            };
        }
        else
        {   
            args = {
                tableInsert->parse.tableRef->common.build.llvmStore,
                tableInsert->parse.tableRef->common.parse.type->table.parse.numRows->common.build.llvmValue,
                llvm::Constant::getNullValue(SupportedPrimitives::INT.common.build.llvmType->getPointerTo())
            };
        }
        Slab* hostSlab = tableInsert->common.parse.hostFunction->create.hostSlab;
        llvm::FunctionCallee coreTableInsert = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::TABLE_INSERT.create.name, TabiCore::TABLE_INSERT.build.functionType); 
        row = builder.CreateCall(coreTableInsert, llvm::ArrayRef(args)); 
    }
    //Then update table elements. 
    ValueRef* tableRef = tableInsert->parse.tableRef; 
    TableType* tableType = (TableType*) tableInsert->parse.tableRef->common.parse.type; 
    for(int fieldIndex = 2; fieldIndex < tableType->parse.fields.size(); fieldIndex++)
    {
        TableField &field = tableType->parse.fields[fieldIndex]; 
        //Get the field store
        llvm::Value* fieldStore; 
        {
            std::vector<llvm::Value*> offsets = {
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, fieldIndex))
            };
            fieldStore = builder.CreateGEP(tableType->common.build.llvmType, tableRef->common.build.llvmStore, llvm::ArrayRef(offsets)); 
        }
        //Get the array store
        llvm::Value* arrayStore = builder.CreateLoad(tableType->parse.fields[fieldIndex].type->common.build.llvmType->getPointerTo(), fieldStore);
        //Get the element store
        llvm::Value* elemStore; 
        {
            std::vector<llvm::Value*> offsets = {
                row
            };
            elemStore = builder.CreateGEP(field.type->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets)); 
        }
        //Store the value if given. 
        //Otherwise store null
        if(tableInsert->parse.elements[fieldIndex-2])
        {
            buildExpression(tableInsert->parse.elements[fieldIndex-2]); 
            builder.CreateStore(tableInsert->parse.elements[fieldIndex-2]->common.build.llvmValue, elemStore);
        }
        else
        {
            builder.CreateStore(llvm::Constant::getNullValue(field.type->common.build.llvmType), elemStore);
        }
    }
}

void tabic::buildTabithaFunction(TabithaFunction* function)
{
    //Dedicate an LLVM BasicBlock to the allocation of stack variables. 
    //We allocate all of them at once to avoid stack overflows. 
    llvm::BasicBlock* stackAlloc = llvm::BasicBlock::Create(llvmContext, "stack_alloc", function->common.build.llvmFunction); 
    builder.SetInsertPoint(stackAlloc);
    llvm::FunctionCallee stackSave = function->create.hostSlab->build.llvmModule->getOrInsertFunction("llvm.stacksave", 
            llvm::FunctionType::get(SupportedPrimitives::SupportedPrimitives::NONE.common.build.llvmType->getPointerTo(), false)); 
    function->build.stackState = builder.CreateCall(stackSave); 
    //First we must allocate the arguments,
    //and set their initial values. 
    int argIndex = 0; 
    for(StackedVariable* arg : function->parse.args)
    {
        Type* type = arg->common.parse.type; 
        buildType(type);
        if(type->common.typeClass == TYPE_PRIMITIVE)
        {
            arg->common.build.llvmStore = builder.CreateAlloca(type->common.build.llvmType); 
            builder.CreateStore(
                function->common.build.llvmFunction->getArg(argIndex),
                arg->common.build.llvmStore);
        }
        else if(type->common.typeClass == TYPE_COLLECTION)
        {
            arg->common.build.llvmStore = builder.CreateAlloca(type->common.build.llvmType); 
            builder.CreateStore(
                function->common.build.llvmFunction->getArg(argIndex),
                arg->common.build.llvmStore);
            copySubvectors(type, arg->common.build.llvmStore, function);
        }
        else if(type->common.typeClass == TYPE_ADDRESS)
        {
            arg->common.build.llvmStore = builder.CreateAlloca(type->common.build.llvmType); 
            builder.CreateStore(
                function->common.build.llvmFunction->getArg(argIndex),
                arg->common.build.llvmStore);
        }
        else if(type->common.typeClass == TYPE_VECTOR)
        {
            llvm::DataLayout dl = function->create.hostSlab->build.llvmModule->getDataLayout();
            if(type->vector.parse.numElem)
            {
                arg->common.build.llvmStore = copyVector(type, function->common.build.llvmFunction->getArg(argIndex), function);  
            }
            else
            {
                arg->common.build.llvmStore = builder.CreateAlloca(type->common.build.llvmType); 
                builder.CreateStore(function->common.build.llvmFunction->getArg(argIndex), arg->common.build.llvmStore); 
            }
        }
        //TODO implement TYPE_TABLE
        argIndex++;
    }
    allocateStackVariables(function->parse.mainBlock);
    allocateHeapHandles(function->parse.mainBlock);     
    //Build the main block.
    llvm::BasicBlock* blockEntry = llvm::BasicBlock::Create(llvmContext, "main_block_entry", function->common.build.llvmFunction);
    builder.CreateBr(blockEntry); 
    builder.SetInsertPoint(blockEntry);
    buildBlock(function->parse.mainBlock);
    //If there is no terminator, then return a null value. 
    if(!builder.GetInsertBlock()->getTerminator())
    {

        std::vector<llvm::Type*> argTypes = { SupportedPrimitives::NONE.common.build.llvmType->getPointerTo() };
        llvm::FunctionCallee stackRestore = function->create.hostSlab->build.llvmModule->getOrInsertFunction("llvm.stackrestore", 
            llvm::FunctionType::get(SupportedPrimitives::SupportedPrimitives::NONE.common.build.llvmType->getPointerTo(), llvm::ArrayRef(argTypes), false)); 
        std::vector<llvm::Value*> args = { function->build.stackState };
        builder.CreateCall(stackRestore, llvm::ArrayRef(args)); 
        if(function->common.parse.returnType == (Type*) &SupportedPrimitives::NONE)
        {
            builder.CreateRetVoid(); 
        }
        else
        {
            builder.CreateRet(llvm::Constant::getNullValue(function->common.parse.returnType->common.build.llvmType));
        }
    }
}

llvm::Value* tabic::copyVector(Type* type, llvm::Value* arg, TabithaFunction* hostFunction)
{
    Slab* hostSlab = hostFunction->create.hostSlab;
    llvm::DataLayout dl = hostSlab->build.llvmModule->getDataLayout();
    llvm::Value* store = builder.CreateAlloca(type->common.build.llvmType);
    llvm::Value* arrayStore = builder.CreateAlloca(type->vector.parse.elemType->common.build.llvmType, type->vector.parse.numElem->common.build.llvmValue); 
    //vectors are passed to the function by their address in memory
    //and by default we create a copy of the vector to be used as a stacked variable
    llvm::Value* copySize = type->vector.parse.numElem->common.build.llvmValue; 
    copySize = builder.CreateMul(copySize, llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, 
                llvm::APInt(32, dl.getTypeAllocSize(type->vector.parse.elemType->common.build.llvmType))));  
    builder.CreateMemCpy(arrayStore, llvm::MaybeAlign{}, arg, llvm::MaybeAlign{}, copySize); 
    builder.CreateStore(arrayStore, store); 
    //TODO implement deeper copying
    llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType;
    if(type->vector.parse.elemType->common.typeClass == TYPE_VECTOR)
    {
        //loop over all elements, calling `copyVector`
        llvm::BasicBlock* subVecCopyStart = llvm::BasicBlock::Create(llvmContext, "subvec_copy_start", hostFunction->common.build.llvmFunction); 
        builder.CreateBr(subVecCopyStart);
        builder.SetInsertPoint(subVecCopyStart); 
        llvm::Value* indexStore = builder.CreateAlloca(int_type); 
        builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore);
        llvm::BasicBlock* subVecCopyCondition = llvm::BasicBlock::Create(llvmContext, "subvec_copy_condition", hostFunction->common.build.llvmFunction); 
        builder.CreateBr(subVecCopyCondition); 
        builder.SetInsertPoint(subVecCopyCondition); 
        llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
        llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
        llvm::BasicBlock* subVecCopyElem = llvm::BasicBlock::Create(llvmContext, "subvec_copy_elem", hostFunction->common.build.llvmFunction);
        builder.SetInsertPoint(subVecCopyElem);
        std::vector<llvm::Value*> offsets = { index }; 
        llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets)); 
        llvm::Value* argElemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arg, llvm::ArrayRef(offsets)); 
        llvm::Value* argElem = builder.CreateLoad(type->vector.parse.elemType->common.build.llvmType, argElemStore); 
        llvm::Value* copyStore = copyVector(type->vector.parse.elemType, argElem, hostFunction); 
        builder.CreateMemCpy(elemStore, llvm::MaybeAlign{}, copyStore, llvm::MaybeAlign{}, dl.getTypeAllocSize(type->vector.parse.elemType->common.build.llvmType));
        index = builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1)));
        builder.CreateStore(index, indexStore); 
        builder.CreateBr(subVecCopyCondition); 
        llvm::BasicBlock* subVecCopyEnd = llvm::BasicBlock::Create(llvmContext, "subvec_copy_end", hostFunction->common.build.llvmFunction); 
        builder.SetInsertPoint(subVecCopyCondition); 
        builder.CreateCondBr(truth, subVecCopyElem, subVecCopyEnd); 
        builder.SetInsertPoint(subVecCopyEnd); 
        /*
        llvm::FunctionCallee coreCopyVector = hostSlab->build.llvmModule->getOrInsertFunction(
                TabiCore::SUBVECTOR_COPY.create.name, 
                TabiCore::SUBVECTOR_COPY.build.functionType); 
        std::vector<llvm::Value*> args = {
            arrayStore, 
            llvm::ConstantInt::get(int_type, llvm::APInt(32, dl.getTypeAllocSize(type->vector.parse.elemType->common.build.llvmType))), 
            llvm::ConstantInt::get(int_type, llvm::APInt(32, dl.getTypeAllocSize(type->vector.parse.elemType->vector.parse.elemType->common.build.llvmType))),
            type->vector.parse.numElem->common.build.llvmValue, type->vector.parse.elemType->vector.parse.numElem->common.build.llvmValue
        }; builder.CreateCall(coreCopyVector, llvm::ArrayRef(args));  
        */ //Might work on this implementation of deep copying in the future, but it requires some thought. 
    }
    else if(type->vector.parse.elemType->common.typeClass == TYPE_COLLECTION)
    {
        llvm::BasicBlock* collectionCopyStart = llvm::BasicBlock::Create(llvmContext, "collection_copy_start", hostFunction->common.build.llvmFunction); 
        builder.CreateBr(collectionCopyStart);
        builder.SetInsertPoint(collectionCopyStart); 
        llvm::Value* indexStore = builder.CreateAlloca(int_type); 
        builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore);
        llvm::BasicBlock* collectionCopyCondition = llvm::BasicBlock::Create(llvmContext, "collection_copy_condition", hostFunction->common.build.llvmFunction); 
        builder.CreateBr(collectionCopyCondition); 
        builder.SetInsertPoint(collectionCopyCondition); 
        llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
        llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
        llvm::BasicBlock* collectionCopyElem = llvm::BasicBlock::Create(llvmContext, "collection_copy_elem", hostFunction->common.build.llvmFunction);
        builder.SetInsertPoint(collectionCopyElem);
        std::vector<llvm::Value*> offsets = { index }; 
        llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets)); 
        //llvm::Value* argElemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arg, llvm::ArrayRef(offsets)); 
        //llvm::Value* argElem = builder.CreateLoad(type->vector.parse.elemType->common.build.llvmType, argElemStore); 
        copySubvectors(type->vector.parse.elemType, elemStore, hostFunction); 
        index = builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1)));
        builder.CreateStore(index, indexStore); 
        builder.CreateBr(collectionCopyCondition); 
        llvm::BasicBlock* collectionCopyEnd = llvm::BasicBlock::Create(llvmContext, "subvec_copy_end", hostFunction->common.build.llvmFunction); 
        builder.SetInsertPoint(collectionCopyCondition); 
        builder.CreateCondBr(truth, collectionCopyElem, collectionCopyEnd); 
        builder.SetInsertPoint(collectionCopyEnd); 
    }

    return store;
}

void tabic::copySubvectors(Type* type, llvm::Value* store, TabithaFunction* hostFunction)
{
    Slab* hostSlab = hostFunction->create.hostSlab;
    llvm::DataLayout dl = hostSlab->build.llvmModule->getDataLayout();
    //for each member, check if it is a vector type or member type and insert code accordingly
    for(auto pair : type->collection.parse.members)
    {
        CollectionMember &member = pair.second; 
        Type* memberType = member.type;
        std::vector<llvm::Value*> offsets = {
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, member.index))
        };
        llvm::Value* memberStore = builder.CreateGEP(type->common.build.llvmType, store, llvm::ArrayRef(offsets)); 
        llvm::Value* memberVal = builder.CreateLoad(memberType->common.build.llvmType, memberStore);
        //llvm::Value* destMemberVal = builder.CreateLoad(memberType->common.build.llvmType, destStore);
        if(memberType->common.typeClass == TYPE_VECTOR)
        {
            llvm::Value* newVal = copyVector(memberType, memberVal, hostFunction); 
            builder.CreateMemCpy(memberStore, llvm::MaybeAlign{}, newVal, llvm::MaybeAlign{}, dl.getTypeAllocSize(memberType->common.build.llvmType));
        }
        else if(memberType->common.typeClass == TYPE_COLLECTION)
        {
            copySubvectors(memberType, memberStore, hostFunction);
        }
    }
}


llvm::Value* tabic::allocateStackType(Type* type, TabithaFunction* hostFunction, std::string name)
{
    buildType(type);
    llvm::Value* store = nullptr;
    if(type->common.typeClass == TYPE_PRIMITIVE)
    {
        if(name == "") store = builder.CreateAlloca(type->common.build.llvmType, nullptr);
        else store = builder.CreateAlloca(type->common.build.llvmType, nullptr, name); 
    }
    else if(type->common.typeClass == TYPE_COLLECTION)
    {
        if(name == "") store = builder.CreateAlloca(type->common.build.llvmType, nullptr); 
        else store = builder.CreateAlloca(type->common.build.llvmType, nullptr, name); 
        allocateStackSubvectors((CollectionType*) type, store, hostFunction);
    }
    else if(type->common.typeClass == TYPE_ADDRESS)
    {
        if(name == "") store = builder.CreateAlloca(type->common.build.llvmType, nullptr);
        else store = builder.CreateAlloca(type->common.build.llvmType, nullptr, name);
    }
    else if(type->common.typeClass == TYPE_VECTOR)
    {
        if(name == "") store = builder.CreateAlloca(type->common.build.llvmType, nullptr);
        else store = builder.CreateAlloca(type->common.build.llvmType, nullptr, name);
    }
    else if(type->common.typeClass == TYPE_TABLE)
    {
        if(name == "") store = builder.CreateAlloca(type->common.build.llvmType, nullptr, name); 
        else store = builder.CreateAlloca(type->common.build.llvmType, nullptr);
    }
    return store; 
}

llvm::Value* tabic::allocateStackVectorElements(Type* type, TabithaFunction* hostFunction)
{
    llvm::Value* arrayStore; 
    if(type->vector.parse.numElem)
    {
        buildExpression(type->vector.parse.numElem); 
        arrayStore = builder.CreateAlloca(
                type->vector.parse.elemType->common.build.llvmType, 
                type->vector.parse.numElem->common.build.llvmValue);
        //ensure that vector-like elements are allocated properly
        if(type->vector.parse.elemType->common.typeClass == TYPE_VECTOR)
        {
            llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType;
            llvm::BasicBlock* subVecAllocStart = llvm::BasicBlock::Create(
                   llvmContext, "subvec_alloc_start", hostFunction->common.build.llvmFunction);  

            builder.CreateBr(subVecAllocStart);
            builder.SetInsertPoint(subVecAllocStart);
            llvm::Value* indexStore = builder.CreateAlloca(int_type); 
            builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore); 
            llvm::BasicBlock* subVecCondition = llvm::BasicBlock::Create(
                    llvmContext, "subvec_alloc_condition", hostFunction->common.build.llvmFunction); 
            builder.CreateBr(subVecCondition);
            builder.SetInsertPoint(subVecCondition);
            llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
            llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
            llvm::BasicBlock* subVecAllocElem = llvm::BasicBlock::Create(
                    llvmContext, "subvec_alloc_elem", hostFunction->common.build.llvmFunction); 
            builder.SetInsertPoint(subVecAllocElem); 
            llvm::Value* elemAlloc = allocateStackType(type->vector.parse.elemType, hostFunction); 
            llvm::Value* elem = builder.CreateLoad(type->vector.parse.elemType->common.build.llvmType, elemAlloc); 
            llvm::Value* offsets = { index }; 
            llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets));
            builder.CreateStore(elem, elemStore);  
            builder.CreateStore(builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1))), indexStore); 
            builder.CreateBr(subVecCondition);
            llvm::BasicBlock* subVecAllocEnd = llvm::BasicBlock::Create(llvmContext, "subvec_alloc_end", hostFunction->common.build.llvmFunction); 
            builder.SetInsertPoint(subVecCondition);
            builder.CreateCondBr(truth, subVecAllocElem, subVecAllocEnd); 
            builder.SetInsertPoint(subVecAllocEnd);
        }  
        //ensure that the vectors in collection elements are allocated properly
        if(type->vector.parse.elemType->common.typeClass == TYPE_COLLECTION)
        {
            llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType;
            llvm::BasicBlock* subVecAllocStart = llvm::BasicBlock::Create(
                  llvmContext, "subvec_alloc_start", hostFunction->common.build.llvmFunction);  

            builder.CreateBr(subVecAllocStart);
            builder.SetInsertPoint(subVecAllocStart);
            llvm::Value* indexStore = builder.CreateAlloca(int_type); 
            builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore); 
            llvm::BasicBlock* subVecCondition = llvm::BasicBlock::Create(
                    llvmContext, "subvec_alloc_condition", hostFunction->common.build.llvmFunction); 
            builder.CreateBr(subVecCondition);
            builder.SetInsertPoint(subVecCondition);
            llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
            llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
            llvm::BasicBlock* subVecAllocElem = llvm::BasicBlock::Create(
                    llvmContext, "subvec_alloc_elem", hostFunction->common.build.llvmFunction); 
            builder.SetInsertPoint(subVecAllocElem); 
            llvm::Value* offsets = { index }; 
            llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets));
            allocateStackSubvectors((CollectionType*)type->vector.parse.elemType, elemStore, hostFunction);
            builder.CreateStore(builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1))), indexStore); 
            builder.CreateBr(subVecCondition);
            llvm::BasicBlock* subVecAllocEnd = llvm::BasicBlock::Create(llvmContext, "subvec_alloc_end", hostFunction->common.build.llvmFunction); 
            builder.SetInsertPoint(subVecCondition);
            builder.CreateCondBr(truth, subVecAllocElem, subVecAllocEnd); 
            builder.SetInsertPoint(subVecAllocEnd);
        }  
    }
    return arrayStore; 
} 

void tabic::allocateStackTableFields(Type* type, TabithaFunction* hostFunction, llvm::Value* store)
{
    int fieldIndex = 0; 
    for(TableField &field : type->table.parse.fields)
    {
        llvm::Value* fieldAlloc = builder.CreateAlloca(field.type->common.build.llvmType, type->table.parse.numRows->common.build.llvmValue); 
        llvm::Value* fieldStore;
        {
            std::vector<llvm::Value*> offsets = { 
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, fieldIndex))
            };
            fieldStore = builder.CreateGEP(
                    type->common.build.llvmType, 
                    store, 
                    llvm::ArrayRef(offsets)); 
        }
        builder.CreateStore(fieldAlloc, fieldStore); 
        fieldIndex++; 
    }
    //Call the core_table_init function
    {
        std::vector<llvm::Value*> args = {
            store, type->table.parse.numRows->common.build.llvmValue
        }; 
        Slab* hostSlab = hostFunction->create.hostSlab;
        llvm::FunctionCallee coreTableInit = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::TABLE_INIT.create.name, TabiCore::TABLE_INIT.build.functionType); 
        builder.CreateCall(coreTableInit, llvm::ArrayRef(args)); 
    }
}

void tabic::allocateStackSubvectors(CollectionType* collectionType, llvm::Value* store, TabithaFunction* hostFunction)
{
    for(auto pair : collectionType->parse.members)
    {
        CollectionMember &member = pair.second;
        Type* memberType = member.type;
        std::vector<llvm::Value*> offsets = {
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, member.index))
        };
        llvm::Value* memberStore = builder.CreateGEP(collectionType->common.build.llvmType, store, llvm::ArrayRef(offsets));
        if(memberType->common.typeClass == TYPE_VECTOR)
        {
            llvm::Value* vecStore = allocateStackType(memberType, hostFunction);  
            llvm::Value* vec      = builder.CreateLoad(memberType->common.build.llvmType, vecStore); 
            builder.CreateStore(vec, memberStore); 
        }
        else if(memberType->common.typeClass == TYPE_TABLE)
        {
            llvm::Value* tableStore = allocateStackType(memberType, hostFunction); 
            llvm::Value* table = builder.CreateLoad(memberType->common.build.llvmType, tableStore); 
            builder.CreateStore(table, memberStore); 
        }
        else if(memberType->common.typeClass == TYPE_COLLECTION)
        {
            allocateStackSubvectors((CollectionType*) memberType, memberStore, hostFunction);
        }
    }
}

void tabic::allocateStackVariables(Block* block)
{
    //First allocate all stack variables belonging explicitly to this block. 
    for(auto pair : block->parse.variables)
    {
        Variable* variable = pair.second;
        if(variable->common.variableClass == VARIABLE_STACKED)
        {
            Type* type = variable->common.parse.type; 
            variable->common.build.llvmStore = allocateStackType(type, block->common.parse.hostFunction, variable->common.parse.name); 
        }
    }
    //Then look for sub-blocks in statements. 
    for(Statement* statement : block->parse.statements)
    {
        StatementClass statementClass = statement->common.statementClass;

        if(statementClass == STATEMENT_BLOCK)
        {
            allocateStackVariables((Block*)statement);
        }
        else if(statementClass == STATEMENT_CONDITIONAL)
        {
            allocateStackVariables(statement->conditional.parse.pair.block);
        }
        else if(statementClass == STATEMENT_BRANCH)
        {
            for(ConditionBlockPair &twig : statement->branch.parse.twigs)
            {
                allocateStackVariables(twig.block);
            }
            if(statement->branch.parse.otherwiseBlock) allocateStackVariables(statement->branch.parse.otherwiseBlock);
        }
        else if(statementClass == STATEMENT_LOOP)
        {
            allocateStackVariables(statement->loop.parse.directions);
        }
    }
}

llvm::Value* tabic::allocateHeapType(Type* type, TabithaFunction* hostFunction, std::string name)
{
    Slab* hostSlab = hostFunction->create.hostSlab;
    llvm::FunctionCallee coreAlloc = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::ALLOC.create.name, TabiCore::ALLOC.build.functionType); 
    buildType(type); 
    TypeClass typeClass = type->common.typeClass;
    llvm::DataLayout dl = hostSlab->build.llvmModule->getDataLayout(); 
    llvm::Value* store;
    if(typeClass == TYPE_PRIMITIVE)
    {
        std::vector<llvm::Value*> args = { llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, 
                llvm::APInt(32, dl.getTypeAllocSize(type->common.build.llvmType))) }; 
        store = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
    }
    else if(typeClass == TYPE_COLLECTION)
    {
        std::vector<llvm::Value*> args = { llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(type->common.build.llvmType))) }; 
        store = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
        allocateHeapSubvectors((CollectionType*) type, store, hostFunction);
    }
    else if(typeClass == TYPE_ADDRESS)
    {
        std::vector<llvm::Value*> args = { llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(type->common.build.llvmType))) }; 
        store = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
    }
    else if(typeClass == TYPE_VECTOR)
    {
        Type* elemType = type->vector.parse.elemType; 
        //allocate the pointer storage 
        {
            std::vector<llvm::Value*> args = { llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(type->common.build.llvmType))) }; 
            store = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
        }
        //allocate element storage
        if(type->vector.parse.numElem)
        {
                buildExpression(type->vector.parse.numElem); 
                llvm::Value* elemSize = llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(elemType->common.build.llvmType)));
                llvm::Value* vecSize  = builder.CreateMul(elemSize, type->vector.parse.numElem->common.build.llvmValue); 
                std::vector<llvm::Value*> args = { vecSize };
                llvm::Value* arrayStore = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
                builder.CreateStore(arrayStore, store); 
                //ensure that vector-like elements are allocated properly
                if(type->vector.parse.elemType->common.typeClass == TYPE_VECTOR)
                {
                    llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType;
                    llvm::BasicBlock* subVecAllocStart = llvm::BasicBlock::Create(
                           llvmContext, "subvec_alloc_start", hostFunction->common.build.llvmFunction);  

                    builder.CreateBr(subVecAllocStart);
                    builder.SetInsertPoint(subVecAllocStart);
                    llvm::Value* indexStore = builder.CreateAlloca(int_type); 
                    builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore); 
                    llvm::BasicBlock* subVecCondition = llvm::BasicBlock::Create(
                            llvmContext, "subvec_alloc_condition", hostFunction->common.build.llvmFunction); 
                    builder.CreateBr(subVecCondition);
                    builder.SetInsertPoint(subVecCondition);
                    llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
                    llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
                    llvm::BasicBlock* subVecAllocElem = llvm::BasicBlock::Create(
                            llvmContext, "subvec_alloc_elem", hostFunction->common.build.llvmFunction); 
                    builder.SetInsertPoint(subVecAllocElem); 
                    llvm::Value* elemAlloc = allocateHeapType(type->vector.parse.elemType, hostFunction); 
                    llvm::Value* elem = builder.CreateLoad(type->vector.parse.elemType->common.build.llvmType, elemAlloc); 
                    llvm::Value* offsets = { index }; 
                    llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets));
                    builder.CreateStore(elem, elemStore);  
                    builder.CreateStore(builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1))), indexStore); 
                    builder.CreateBr(subVecCondition);
                    llvm::BasicBlock* subVecAllocEnd = llvm::BasicBlock::Create(llvmContext, "subvec_alloc_end", hostFunction->common.build.llvmFunction); 
                    builder.SetInsertPoint(subVecCondition);
                    builder.CreateCondBr(truth, subVecAllocElem, subVecAllocEnd); 
                    builder.SetInsertPoint(subVecAllocEnd);
                }
               //ensure that the vectors in collection elements are allocated properly
               if(type->vector.parse.elemType->common.typeClass == TYPE_COLLECTION)
               {
                   llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType;
                   llvm::BasicBlock* subVecAllocStart = llvm::BasicBlock::Create(
                         llvmContext, "subvec_alloc_start", hostFunction->common.build.llvmFunction);  

                   builder.CreateBr(subVecAllocStart);
                   builder.SetInsertPoint(subVecAllocStart);
                   llvm::Value* indexStore = builder.CreateAlloca(int_type); 
                   builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore); 
                   llvm::BasicBlock* subVecCondition = llvm::BasicBlock::Create(
                           llvmContext, "subvec_alloc_condition", hostFunction->common.build.llvmFunction); 
                   builder.CreateBr(subVecCondition);
                   builder.SetInsertPoint(subVecCondition);
                   llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
                   llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
                   llvm::BasicBlock* subVecAllocElem = llvm::BasicBlock::Create(
                           llvmContext, "subvec_alloc_elem", hostFunction->common.build.llvmFunction); 
                   builder.SetInsertPoint(subVecAllocElem); 
                   llvm::Value* offsets = { index }; 
                   llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets));
                   allocateHeapSubvectors((CollectionType*)type->vector.parse.elemType, elemStore, hostFunction);
                   builder.CreateStore(builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1))), indexStore); 
                   builder.CreateBr(subVecCondition);
                   llvm::BasicBlock* subVecAllocEnd = llvm::BasicBlock::Create(llvmContext, "subvec_alloc_end", hostFunction->common.build.llvmFunction); 
                   builder.SetInsertPoint(subVecCondition);
                   builder.CreateCondBr(truth, subVecAllocElem, subVecAllocEnd); 
                   builder.SetInsertPoint(subVecAllocEnd);
               }
        }
    }
    else if(typeClass == TYPE_TABLE)
    {
        std::vector<llvm::Value*> args = { llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(type->common.build.llvmType))) }; 
        store = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
        for(int fieldIndex = 0; fieldIndex < type->table.parse.fields.size(); fieldIndex++)
        {
            TableField &field = type->table.parse.fields[fieldIndex];
            llvm::Value* elemSize = llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(field.type->common.build.llvmType)));
            llvm::Value* fieldSize = builder.CreateMul(type->table.parse.numRows->common.build.llvmValue, elemSize);  
            std::vector<llvm::Value*> args = { fieldSize } ;
            llvm::Value* fieldAlloc = builder.CreateCall(coreAlloc, llvm::ArrayRef(args)); 
            std::vector<llvm::Value*> offsets = {
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, fieldIndex))
            };
            llvm::Value* fieldStore = builder.CreateGEP(type->common.build.llvmType, store, llvm::ArrayRef(offsets)); 
            builder.CreateStore(fieldAlloc, fieldStore); 
        }
    }
    return store;
}

void tabic::allocateContextType(Type* type, Slab* hostSlab, llvm::Value* contextStore, std::string name)
{
    Bundle* hostBundle = hostSlab->create.hostBundle;
    llvm::FunctionCallee coreAlloc = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::ALLOC.create.name, TabiCore::ALLOC.build.functionType); 
    buildType(type); 
    TypeClass typeClass = type->common.typeClass;
    llvm::DataLayout dl = hostSlab->build.llvmModule->getDataLayout(); 
    llvm::Value* store;
    if(typeClass == TYPE_COLLECTION)
    {
        allocateContextSubvectors((CollectionType*) type, contextStore, hostSlab);
    }
    else if(typeClass == TYPE_VECTOR)
    {
        Type* elemType = type->vector.parse.elemType; 
        //allocate the pointer storage 
        {
            std::vector<llvm::Value*> args = { llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(type->common.build.llvmType))) }; 
            store = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
        }
        //allocate element storage
        if(type->vector.parse.numElem)
        {
                buildExpression(type->vector.parse.numElem); 
                llvm::Value* elemSize = llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(elemType->common.build.llvmType)));
                llvm::Value* vecSize  = builder.CreateMul(elemSize, type->vector.parse.numElem->common.build.llvmValue); 
                std::vector<llvm::Value*> args = { vecSize };
                llvm::Value* arrayStore = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
                builder.CreateStore(arrayStore, contextStore); 
                //ensure that vector-like elements are allocated properly
                if(type->vector.parse.elemType->common.typeClass == TYPE_VECTOR)
                {
                    llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType;
                    llvm::BasicBlock* subVecAllocStart = llvm::BasicBlock::Create(
                           llvmContext, "subvec_alloc_start", hostBundle->build.init);  

                    builder.CreateBr(subVecAllocStart);
                    builder.SetInsertPoint(subVecAllocStart);
                    llvm::Value* indexStore = builder.CreateAlloca(int_type); 
                    builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore); 
                    llvm::BasicBlock* subVecCondition = llvm::BasicBlock::Create(
                            llvmContext, "subvec_alloc_condition", hostBundle->build.init);
                    builder.CreateBr(subVecCondition);
                    builder.SetInsertPoint(subVecCondition);
                    llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
                    llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
                    llvm::BasicBlock* subVecAllocElem = llvm::BasicBlock::Create(
                            llvmContext, "subvec_alloc_elem", hostBundle->build.init);
                    builder.SetInsertPoint(subVecAllocElem); 
                    llvm::Value* offsets = { index }; 
                    llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets));
                    allocateContextType(type->vector.parse.elemType, hostSlab, elemStore); 
                    builder.CreateStore(builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1))), indexStore); 
                    builder.CreateBr(subVecCondition);
                    llvm::BasicBlock* subVecAllocEnd = llvm::BasicBlock::Create(llvmContext, "subvec_alloc_end", hostBundle->build.init);
                    builder.SetInsertPoint(subVecCondition);
                    builder.CreateCondBr(truth, subVecAllocElem, subVecAllocEnd); 
                    builder.SetInsertPoint(subVecAllocEnd);
                }
               //ensure that the vectors in collection elements are allocated properly
               if(type->vector.parse.elemType->common.typeClass == TYPE_COLLECTION)
               {
                   llvm::Type* int_type = SupportedPrimitives::INT.common.build.llvmType;
                   llvm::BasicBlock* subVecAllocStart = llvm::BasicBlock::Create(
                         llvmContext, "subvec_alloc_start", hostBundle->build.init);

                   builder.CreateBr(subVecAllocStart);
                   builder.SetInsertPoint(subVecAllocStart);
                   llvm::Value* indexStore = builder.CreateAlloca(int_type); 
                   builder.CreateStore(llvm::ConstantInt::get(int_type, llvm::APInt(32, 0)), indexStore); 
                   llvm::BasicBlock* subVecCondition = llvm::BasicBlock::Create(
                           llvmContext, "subvec_alloc_condition", hostBundle->build.init);
                   builder.CreateBr(subVecCondition);
                   builder.SetInsertPoint(subVecCondition);
                   llvm::Value* index = builder.CreateLoad(int_type, indexStore); 
                   llvm::Value* truth = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
                   llvm::BasicBlock* subVecAllocElem = llvm::BasicBlock::Create(
                           llvmContext, "subvec_alloc_elem", hostBundle->build.init);
                   builder.SetInsertPoint(subVecAllocElem); 
                   llvm::Value* offsets = { index }; 
                   llvm::Value* elemStore = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets));
                   allocateContextSubvectors((CollectionType*)type->vector.parse.elemType, elemStore, hostSlab);
                   builder.CreateStore(builder.CreateAdd(index, llvm::ConstantInt::get(int_type, llvm::APInt(32, 1))), indexStore); 
                   builder.CreateBr(subVecCondition);
                   llvm::BasicBlock* subVecAllocEnd = llvm::BasicBlock::Create(llvmContext, "subvec_alloc_end", hostBundle->build.init);
                   builder.SetInsertPoint(subVecCondition);
                   builder.CreateCondBr(truth, subVecAllocElem, subVecAllocEnd); 
                   builder.SetInsertPoint(subVecAllocEnd);
               }
        }
    }
    else if(typeClass == TYPE_TABLE)
    {
        //std::vector<llvm::Value*> args = { llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(type->common.build.llvmType))) }; 
        //store = builder.CreateCall(coreAlloc, llvm::ArrayRef(args));  
        for(int fieldIndex = 0; fieldIndex < type->table.parse.fields.size(); fieldIndex++)
        {
            TableField &field = type->table.parse.fields[fieldIndex];
            llvm::Value* elemSize = llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, dl.getTypeAllocSize(field.type->common.build.llvmType)));
            llvm::Value* fieldSize = builder.CreateMul(type->table.parse.numRows->common.build.llvmValue, elemSize);  
            std::vector<llvm::Value*> args = { fieldSize } ;
            llvm::Value* fieldAlloc = builder.CreateCall(coreAlloc, llvm::ArrayRef(args)); 
            std::vector<llvm::Value*> offsets = {
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, fieldIndex))
            };
            llvm::Value* fieldStore = builder.CreateGEP(type->common.build.llvmType, contextStore, llvm::ArrayRef(offsets)); 
            builder.CreateStore(fieldAlloc, fieldStore); 
        }
    }
}

void tabic::allocateHeapSubvectors(CollectionType* collectionType, llvm::Value* store, TabithaFunction* hostFunction)
{
    for(auto pair : collectionType->parse.members)
    {
        CollectionMember &member = pair.second;
        Type* memberType = member.type;
        std::vector<llvm::Value*> offsets = {
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, member.index))
        };
        llvm::Value* memberStore = builder.CreateGEP(collectionType->common.build.llvmType, store, llvm::ArrayRef(offsets));
        if(memberType->common.typeClass == TYPE_VECTOR)
        {
            llvm::Value* vecStore = allocateHeapType(memberType, hostFunction);  
            llvm::Value* vec      = builder.CreateLoad(memberType->common.build.llvmType, vecStore); 
            builder.CreateStore(vec, memberStore); 
        }
        else if(memberType->common.typeClass == TYPE_TABLE)
        {
            llvm::Value* tableStore = allocateHeapType(memberType, hostFunction); 
            llvm::Value* table = builder.CreateLoad(memberType->common.build.llvmType, tableStore); 
            builder.CreateStore(table, memberStore); 
        }
        else if(memberType->common.typeClass == TYPE_COLLECTION)
        {
            allocateHeapSubvectors((CollectionType*) memberType, memberStore, hostFunction);
        }
    }
}


void tabic::allocateContextSubvectors(CollectionType* collectionType, llvm::Value* store, Slab* hostSlab)
{
    for(auto pair : collectionType->parse.members)
    {
        CollectionMember &member = pair.second;
        Type* memberType = member.type;
        std::vector<llvm::Value*> offsets = {
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, member.index))
        };
        llvm::Value* memberStore = builder.CreateGEP(collectionType->common.build.llvmType, store, llvm::ArrayRef(offsets));
        if(memberType->common.typeClass == TYPE_VECTOR)
        {
            allocateContextType(memberType, hostSlab, memberStore);  
        }
        else if(memberType->common.typeClass == TYPE_COLLECTION)
        {
            allocateContextSubvectors((CollectionType*) memberType, memberStore, hostSlab);
        }
    }
}

void tabic::buildHeapedDeclaration(HeapedDeclaration* heapedDeclaration)
{
    //Allocate the associated HeapedVariable. 
    HeapedVariable* variable = heapedDeclaration->parse.variable;
    Type* type = variable->common.parse.type;
    buildType(type);
    llvm::Value* ptr = allocateHeapType(type, heapedDeclaration->common.parse.hostFunction, variable->common.parse.name);    
    builder.CreateStore(ptr, variable->common.build.llvmStore);
}

void tabic::registerFunction(Function* function)
{
    //Build the return type.
    buildType(function->common.parse.returnType); 
    //Build the argument types.
    std::vector<llvm::Type*> llvmArgTypes = {}; 
    
    llvm::Module* hostModule = nullptr; 
    if(function->common.functionClass == FUNCTION_TABITHA)
    {
        for(StackedVariable* arg : function->tabitha.parse.args)
        {
            buildType(arg->common.parse.type); 
            llvmArgTypes.push_back(arg->common.parse.type->common.build.llvmType); 
        }
        Slab* hostSlab = function->tabitha.create.hostSlab;
        Bundle* hostBundle = hostSlab->create.hostBundle;
        if(function->tabitha.create.name == "main" && hostSlab == hostBundle->create.rootSlab) function->common.build.fullName = "_tabi_main";
        else function->common.build.fullName = function->tabitha.create.hostSlab->create.id + "::" + function->tabitha.create.name;
        hostModule = function->tabitha.create.hostSlab->build.llvmModule;
    }
    else if(function->common.functionClass == FUNCTION_EXTERNAL)
    {
        for(Type* arg : function->external.parse.args)
        {
            buildType(arg);
            llvmArgTypes.push_back(arg->common.build.llvmType); 
        }
        function->common.build.fullName = function->external.parse.externalName; 
        hostModule = function->external.create.hostSlab->build.llvmModule; 
    }
    llvm::FunctionType* functionType = llvm::FunctionType::get(
            function->common.parse.returnType->common.build.llvmType,
            llvm::ArrayRef(llvmArgTypes), false);
    function->common.build.llvmFunction = llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage, function->common.build.fullName, hostModule);
    function->common.build.llvmFunction->setCallingConv(TABITHA_CALLING_CONVENTION); 
}

void tabic::buildContext(Context* context)
{
    Bundle* hostBundle = context->create.hostSlab->create.hostBundle;
    //Create a global variable for each member
    for(auto pair : context->parse.members)
    {
        ContextVariable* variable = pair.second;
        buildType(variable->common.parse.type); 
        llvm::Value* llvmInit; 
        if(variable->parse.initialiser) 
        {
            buildExpression(variable->parse.initialiser); 
            llvmInit = variable->parse.initialiser->common.build.llvmValue; 
        }
        else
        {
            llvmInit = llvm::Constant::getNullValue(variable->common.parse.type->common.build.llvmType); 
        }
        variable->build.fullName = context->create.hostSlab->create.id + "_" + context->create.name + "_" + variable->common.parse.name;
        variable->common.build.llvmStore = new llvm::GlobalVariable(
                *context->create.hostSlab->build.llvmModule,
                variable->common.parse.type->common.build.llvmType,
                false, llvm::GlobalVariable::ExternalLinkage, (llvm::Constant*) llvmInit,
                variable->build.fullName, nullptr, llvm::GlobalVariable::NotThreadLocal);
        variable->build.declaredIn.push_back(context->create.hostSlab); 

        if((variable->common.parse.type->common.typeClass == TYPE_VECTOR && variable->common.parse.type->vector.parse.numElem) ||
                variable->common.parse.type->common.typeClass == TYPE_TABLE || variable->common.parse.type->common.typeClass == TYPE_COLLECTION)
        {
            builder.SetInsertPoint(hostBundle->build.initExit);
            allocateContextType(variable->common.parse.type, context->create.hostSlab, variable->common.build.llvmStore, variable->build.fullName + "_init");
            hostBundle->build.initExit = builder.GetInsertBlock(); 
        }
    }
}

void tabic::buildDump(Dump* dump)
{
    //Create a global variable for each member
    for(auto pair : dump->parse.members)
    {
        DumpVariable* variable = pair.second; 
        llvm::Value* llvmInit; 
        if(variable->parse.initialiser)
        {
            buildExpression(variable->parse.initialiser); 
            llvmInit = variable->parse.initialiser->common.build.llvmValue; 
        }
        else
        {
            llvmInit = llvm::Constant::getNullValue(variable->common.parse.type->common.build.llvmType); 
        }
        variable->build.fullName = dump->create.hostSlab->create.id + "_" + dump->create.name + "_" + variable->common.parse.name;
        variable->common.build.llvmStore = new llvm::GlobalVariable(
                *dump->create.hostSlab->build.llvmModule,
                variable->common.parse.type->common.build.llvmType,
                false, llvm::GlobalVariable::ExternalLinkage, (llvm::Constant*) llvmInit,
                variable->build.fullName, nullptr, llvm::GlobalVariable::NotThreadLocal);
        variable->build.declaredIn.push_back(dump->create.hostSlab);
    }
}

void tabic::buildExpression(Expression* expression)
{
    ExpressionClass expressionClass = expression->common.expressionClass; 
    //For each of the primitive literals, we are just creating constants. 
    if(expressionClass == EXPRESSION_NULL)
    {
        expression->common.build.llvmValue = llvm::Constant::getNullValue(SupportedPrimitives::NONE.common.build.llvmType->getPointerTo()); 
    }
    else if(expressionClass == EXPRESSION_INT_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantInt::get(
                SupportedPrimitives::INT.common.build.llvmType, 
                llvm::APInt(32, expression->intLiteral.parse.value));
    }
    else if(expressionClass == EXPRESSION_FLOAT_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantFP::get(
                SupportedPrimitives::FLOAT.common.build.llvmType,
                llvm::APFloat(expression->floatLiteral.parse.value));
    }
    else if(expressionClass == EXPRESSION_LONG_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantInt::get(
                SupportedPrimitives::LONG.common.build.llvmType,
                llvm::APInt(64, expression->longLiteral.parse.value)); 
    }
    else if(expressionClass == EXPRESSION_SHORT_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantInt::get(
                SupportedPrimitives::SHORT.common.build.llvmType,
                llvm::APInt(16, expression->shortLiteral.parse.value)); 
    }
    else if(expressionClass == EXPRESSION_DOUBLE_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantFP::get(
                SupportedPrimitives::DOUBLE.common.build.llvmType,
                llvm::APFloat(expression->doubleLiteral.parse.value)); 
    }
    else if(expressionClass == EXPRESSION_SIZE_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantInt::get(
                SupportedPrimitives::SIZE.common.build.llvmType,
                llvm::APInt(32, expression->sizeLiteral.parse.value)); 
    }
    else if(expressionClass == EXPRESSION_CHAR_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantInt::get(
                SupportedPrimitives::CHAR.common.build.llvmType,
                llvm::APInt(8, expression->charLiteral.parse.value));
    }
    else if(expressionClass == EXPRESSION_STRING_LITERAL)
    {
        expression->common.build.llvmValue = builder.CreateGlobalStringPtr(expression->stringLiteral.parse.value); 
    }
    else if(expressionClass == EXPRESSION_TRUTH_LITERAL)
    {
        expression->common.build.llvmValue = llvm::ConstantInt::get(
                SupportedPrimitives::TRUTH.common.build.llvmType,
                llvm::APInt(1, expression->truthLiteral.parse.value));
    }
    else if(expressionClass == EXPRESSION_VARIABLE_VALUE)
    {
        //Build the type.
        buildType(expression->common.parse.type);
        buildValueRef(expression->variableValue.parse.ref, nullptr); 
        expression->common.build.llvmValue = expression->variableValue.parse.ref->common.build.llvmStore;
        
        //If the locate operator `?` has not been applied, then we load the actual variable value.
        //Also if the variable ref is of type vector, we should not load. 
        if(!expression->variableValue.parse.locate)
        {
            expression->common.build.llvmValue = builder.CreateLoad(
            expression->common.parse.type->common.build.llvmType,
            expression->common.build.llvmValue);
        }
    }
    else if(expressionClass == EXPRESSION_FUNCTION_CALL)
    {
        //Build the type. 
        buildType(expression->common.parse.type); 
        llvm::FunctionCallee callee = expression->common.parse.hostSlab->build.llvmModule->getOrInsertFunction(
                expression->functionCall.parse.callee->common.build.fullName,
                expression->functionCall.parse.callee->common.build.llvmFunction->getFunctionType());
        //Build the arguments. 
        std::vector<llvm::Value*> llvmArgs = {};
        for(Expression* arg : expression->functionCall.parse.args)
        {
            buildExpression(arg); 
            llvmArgs.push_back(arg->common.build.llvmValue);
        }
        expression->common.build.llvmValue = builder.CreateCall(callee, llvm::ArrayRef(llvmArgs));
    }
    else if(expressionClass == EXPRESSION_BRACKETED)
    {
        //Build the type
        buildType(expression->common.parse.type); 
        //Build the contents
        buildExpression(expression->bracketed.parse.contents); 
        //Copy the value
        expression->common.build.llvmValue = expression->bracketed.parse.contents->common.build.llvmValue;
    }
    else if(expressionClass == EXPRESSION_BINARY)
    {
        //account for order of operations
        if(expression->binary.parse.rhs->common.expressionClass == EXPRESSION_BINARY)
        {
            Expression* a = expression->binary.parse.lhs; 
            Expression* b = expression->binary.parse.rhs->binary.parse.lhs; 
            Expression* c = expression->binary.parse.rhs->binary.parse.rhs; 
            BinaryOperator p = expression->binary.parse.op; 
            BinaryOperator q = expression->binary.parse.rhs->binary.parse.op; 
            if(p > q)
            {
                Expression* dummy = expression->binary.parse.lhs; 
                expression->binary.parse.lhs = expression->binary.parse.rhs; 
                expression->binary.parse.rhs = dummy;
                expression->binary.parse.lhs->binary.parse.lhs = a;
                expression->binary.parse.lhs->binary.parse.rhs = b;
                expression->binary.parse.rhs = c; 
                expression->binary.parse.op = q; 
                expression->binary.parse.lhs->binary.parse.op = p;
            }
        }
        Expression* lhs = expression->binary.parse.lhs;
        Expression* rhs = expression->binary.parse.rhs;
        BinaryOperator op = expression->binary.parse.op;
        //Build the type. 
        buildType(expression->common.parse.type); 
        //Build the LHS and RHS.
        buildExpression(lhs);
        buildExpression(rhs);
        llvm::Value* llvmLHS = lhs->common.build.llvmValue;
        llvm::Value* llvmRHS = rhs->common.build.llvmValue;
        Type* lhsType = lhs->common.parse.type;

        //Now process based on the operator and respective types. 
        if(op == BINARY_OP_PLUS)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateAdd(llvmLHS, llvmRHS);
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFAdd(llvmLHS, llvmRHS);
            }
        }
        else if(op == BINARY_OP_SUB)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateSub(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFSub(llvmLHS, llvmRHS); 
            } 
        }
        else if(op == BINARY_OP_MUL)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateMul(llvmLHS, llvmRHS);
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFMul(llvmLHS, llvmRHS); 
            }
        }
        else if(op == BINARY_OP_DIV)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateSDiv(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFDiv(llvmLHS, llvmRHS);
            }
        }
        else if(op == BINARY_OP_LT)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateICmpSLT(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFCmpOLT(llvmLHS, llvmRHS); 
            }
        }
        else if(op == BINARY_OP_GT)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateICmpSGT(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFCmpOGT(llvmLHS, llvmRHS); 
            }
        }
        else if(op == BINARY_OP_LTE)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateICmpSLE(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFCmpOLE(llvmLHS, llvmRHS); 
            }
        }
        else if(op == BINARY_OP_GTE)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateICmpSGE(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFCmpOGE(llvmLHS, llvmRHS); 
            }
        }
        else if(op == BINARY_OP_EQUALS)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateICmpEQ(llvmLHS, llvmRHS);
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFCmpOEQ(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_CHAR)
            {
                expression->common.build.llvmValue = builder.CreateICmpEQ(llvmLHS, llvmRHS); 
            }
        }
        else if(op == BINARY_OP_NOT_EQUAL)
        {
            if(expression->binary.parse.ep == EP_INT)
            {
                expression->common.build.llvmValue = builder.CreateICmpNE(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_FLOAT)
            {
                expression->common.build.llvmValue = builder.CreateFCmpONE(llvmLHS, llvmRHS); 
            }
            else if(expression->binary.parse.ep == EP_CHAR)
            {
                expression->common.build.llvmValue = builder.CreateICmpNE(llvmLHS, llvmRHS); 
            }
        }
    }
}


void tabic::buildConditional(Conditional* conditional)
{
    //Build the condition. 
    llvm::BasicBlock* conditionEval = llvm::BasicBlock::Create(llvmContext, "condition_evaluate", conditional->common.parse.hostFunction->common.build.llvmFunction); 
    builder.CreateBr(conditionEval);
    builder.SetInsertPoint(conditionEval);
    buildExpression(conditional->parse.pair.expression); 
    //Conditionally break to the consequence.   
    llvm::BasicBlock* consequenceStart = llvm::BasicBlock::Create(llvmContext, "consequence_start", conditional->common.parse.hostFunction->common.build.llvmFunction);
    builder.SetInsertPoint(consequenceStart); 
    //Build the consequence block
    buildBlock(conditional->parse.pair.block); 
    llvm::BasicBlock* consequenceEnd = llvm::BasicBlock::Create(llvmContext, "consequence_end", conditional->common.parse.hostFunction->common.build.llvmFunction);
    //If building the block has not introduced a terminator, then create one. 
    if(!builder.GetInsertBlock()->getTerminator()) builder.CreateBr(consequenceEnd); 
    builder.SetInsertPoint(conditionEval); 
    builder.CreateCondBr(conditional->parse.pair.expression->common.build.llvmValue, consequenceStart, consequenceEnd);  
    builder.SetInsertPoint(consequenceEnd); 
}

void tabic::buildBranch(Branch* branch)
{
    //Create all of the appropriate LLVM BasicBlock
    std::vector<llvm::BasicBlock*> twigStarts = {};  
    std::vector<llvm::BasicBlock*> twigEnds = {};  
    std::vector<llvm::BasicBlock*> twigDones = {};
    int twigIndex = 0; 
    for(ConditionBlockPair &pair : branch->parse.twigs)
    {
        twigStarts.push_back(llvm::BasicBlock::Create(
                    llvmContext, 
                    "twig_" + std::to_string(twigIndex) + + "_start", branch->common.parse.hostFunction->common.build.llvmFunction));  
        //Build condition. 
        buildExpression(pair.expression);
        llvm::BasicBlock* temp = builder.GetInsertBlock();
        //Build the twig block.
        builder.SetInsertPoint(twigStarts[twigIndex]); 
        buildBlock(pair.block); 
        twigDones.push_back(builder.GetInsertBlock()); 
        twigEnds.push_back(llvm::BasicBlock::Create(
                    llvmContext, 
                    "twig_" + std::to_string(twigIndex) + + "_end", branch->common.parse.hostFunction->common.build.llvmFunction));  
        builder.SetInsertPoint(twigEnds[twigIndex]); 
        //Make a conditional break. 
        builder.SetInsertPoint(twigStarts[twigIndex]);
        builder.SetInsertPoint(temp);
        builder.CreateCondBr(
                pair.expression->common.build.llvmValue, 
                twigStarts[twigIndex], twigEnds[twigIndex]); 
        builder.SetInsertPoint(twigEnds[twigIndex]);
        twigIndex++; 
    }
    llvm::BasicBlock* otherwiseStart = nullptr; 
    llvm::BasicBlock* otherwiseEnd = nullptr; 
    llvm::BasicBlock* otherwiseDone = nullptr;

    if(branch->parse.otherwiseBlock)
    {
        otherwiseStart = llvm::BasicBlock::Create(
                llvmContext, "otherwise_start", branch->common.parse.hostFunction->common.build.llvmFunction); 
        builder.CreateBr(otherwiseStart); 
        builder.SetInsertPoint(otherwiseStart); 
        buildBlock(branch->parse.otherwiseBlock); 
        otherwiseDone = builder.GetInsertBlock();
        otherwiseEnd = llvm::BasicBlock::Create(
                llvmContext, "otherwise_end", branch->common.parse.hostFunction->common.build.llvmFunction); 
        builder.SetInsertPoint(otherwiseEnd); 
    }
    llvm::BasicBlock* branchEnd = llvm::BasicBlock::Create(
            llvmContext, "branch_end", branch->common.parse.hostFunction->common.build.llvmFunction); 

    for(llvm::BasicBlock* done : twigDones)
    {
        //Conditinally insert a terminator to the end of the branch statement.
        if(!done->getTerminator()) 
        {
            builder.SetInsertPoint(done); 
            builder.CreateBr(branchEnd); 
        }
    }
    if(branch->parse.otherwiseBlock)
    {
        if(!otherwiseDone->getTerminator())
        {
            builder.SetInsertPoint(otherwiseDone);
            builder.CreateBr(branchEnd); 
        }
        builder.SetInsertPoint(otherwiseEnd); 
    }
    else if(twigEnds.size() > 0)
    {
        builder.SetInsertPoint(twigEnds[twigEnds.size()-1]); 
    }
    builder.CreateBr(branchEnd); 
    builder.SetInsertPoint(branchEnd); 
}

void tabic::buildLoop(Loop* loop)
{
    //Create LLVM BasicBlock 
    llvm::BasicBlock* condition = llvm::BasicBlock::Create(llvmContext, "loop_condition", loop->common.parse.hostFunction->common.build.llvmFunction);  
    builder.CreateBr(condition); 
    builder.SetInsertPoint(condition); 
    //Build the condition.
    buildExpression(loop->parse.condition); 
    llvm::BasicBlock* directionStart = llvm::BasicBlock::Create(llvmContext, "loop_direction_start", loop->common.parse.hostFunction->common.build.llvmFunction);  
    builder.SetInsertPoint(directionStart); 
    llvm::Value* stackState; 
    {
        Slab* hostSlab = loop->common.parse.hostFunction->create.hostSlab; 
        llvm::FunctionCallee stackSave = hostSlab->build.llvmModule->getOrInsertFunction("llvm.stacksave", 
                llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType->getPointerTo(), false)); 
        stackState = builder.CreateCall(stackSave); 
    }
    buildBlock(loop->parse.directions); 
    {
        Slab* hostSlab = loop->common.parse.hostFunction->create.hostSlab; 
        std::vector<llvm::Type*> argTypes = { SupportedPrimitives::NONE.common.build.llvmType->getPointerTo() }; 
        llvm::FunctionCallee stackRestore = hostSlab->build.llvmModule->getOrInsertFunction("llvm.stackrestore", 
                llvm::FunctionType::get(SupportedPrimitives::NONE.common.build.llvmType->getPointerTo(), llvm::ArrayRef(argTypes), false)); 
        std::vector<llvm::Value*> args = { stackState }; 
        builder.CreateCall(stackRestore, llvm::ArrayRef(args)); 
    }
    llvm::BasicBlock* directionEnd = llvm::BasicBlock::Create(llvmContext, "loop_direction_end", loop->common.parse.hostFunction->common.build.llvmFunction);  
    if(!builder.GetInsertBlock()->getTerminator()) builder.CreateBr(directionEnd); 
    builder.SetInsertPoint(directionEnd); 
    builder.CreateBr(condition); 
    llvm::BasicBlock* loopEnd = llvm::BasicBlock::Create(llvmContext, "loop_end", loop->common.parse.hostFunction->common.build.llvmFunction);
    builder.SetInsertPoint(condition);
    builder.CreateCondBr(loop->parse.condition->common.build.llvmValue, directionStart, loopEnd); 
    //Loop back. 
    //if(!builder.GetInsertBlock()->getTerminator()) builder.CreateBr(condition); 
    builder.SetInsertPoint(loopEnd); 
}

void tabic::buildValueRef(ValueRef* valueRef, ValueRef* fine)
{
    if(!fine) fine = valueRef;
    //First build the parent. 
    if(valueRef->common.parse.parent) buildValueRef(valueRef->common.parse.parent, fine); 
    //Then look at cases. 
    ValueRefClass valueRefClass = valueRef->common.valueRefClass; 
    if(valueRefClass == VALUE_REF_VARIABLE)
    {
        Variable* variable = valueRef->variable.parse.variable;
        VariableClass variableClass = valueRef->variable.parse.variable->common.variableClass; 
        llvm::Module* hostModule =  valueRef->variable.parse.hostSlab->build.llvmModule;
        if(variableClass == VARIABLE_CONTEXT) 
        {
            valueRef->common.build.llvmStore = hostModule->getOrInsertGlobal(variable->context.build.fullName, variable->common.parse.type->common.build.llvmType); 
        }
        else if(variableClass == VARIABLE_DUMP)
        {
            valueRef->common.build.llvmStore = hostModule->getOrInsertGlobal(variable->dump.build.fullName, variable->common.parse.type->common.build.llvmType); 
        }
        else
        {
            valueRef->common.build.llvmStore = variable->common.build.llvmStore; 
        }
        //extra load required for heaped varibles
        if(valueRef->variable.parse.variable->common.variableClass == VARIABLE_HEAPED)
        {
            valueRef->common.build.llvmStore = builder.CreateLoad(
                    valueRef->variable.parse.variable->common.parse.type->common.build.llvmType->getPointerTo(),
                    valueRef->common.build.llvmStore); 
        }
    }
    else if(valueRefClass == VALUE_REF_MEMBER)
    {
        std::vector<llvm::Value*> offsets = {
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, valueRef->member.parse.memberIndex)) 
        }; 
        //the parent might be Addr[Addr[...Addr[X]]] so need to be careful here
        Type* collectionType = valueRef->common.parse.parent->common.parse.type; 
        llvm::Value* collectionStore = valueRef->common.parse.parent->common.build.llvmStore; 
        while(collectionType->common.typeClass == TYPE_ADDRESS)
        {
            collectionStore = builder.CreateLoad(collectionType->common.build.llvmType, collectionStore); 
            collectionType = collectionType->address.parse.pointsTo; 
        }
        valueRef->common.build.llvmStore = builder.CreateGEP(
                collectionType->common.build.llvmType,
                collectionStore,
                llvm::ArrayRef(offsets)); 
    }
    else if(valueRefClass == VALUE_REF_ELEMENT)
    {
        buildExpression(valueRef->element.parse.index); 
        std::vector<llvm::Value*> offsets = {
            valueRef->element.parse.index->common.build.llvmValue
        };
        //the parent might be Addr[Addr[...Addr[Vec[X, n]]]] so need to be careful here
        Type* vectorType = valueRef->common.parse.parent->common.parse.type; 
        llvm::Value* vectorStore = valueRef->common.parse.parent->common.build.llvmStore; 

        //do an extra load to get the element store
        vectorStore = builder.CreateLoad(vectorType->common.build.llvmType, vectorStore); 

        while(vectorType->common.typeClass == TYPE_ADDRESS)
        {
            vectorStore = builder.CreateLoad(vectorType->common.build.llvmType, vectorStore);
            vectorType = vectorType->address.parse.pointsTo;
        }
        valueRef->common.build.llvmStore = builder.CreateGEP(
                vectorType->vector.parse.elemType->common.build.llvmType,
                vectorStore,
                llvm::ArrayRef(offsets));
    }
    else if(valueRefClass == VALUE_REF_ROW)
    {
        buildExpression(valueRef->row.parse.id); 
        //Get the associated row.
        llvm::Value* row; 
        {
            std::vector<llvm::Value*> args = {
                valueRef->common.parse.parent->common.build.llvmStore,
                valueRef->common.parse.parent->common.parse.type->table.parse.numRows->common.build.llvmValue,
                valueRef->row.parse.id->common.build.llvmValue
            }; 
            Slab* hostSlab = valueRef->common.parse.parent->variable.parse.hostSlab;
            llvm::FunctionCallee coreTableGetRow = hostSlab->build.llvmModule->getOrInsertFunction(
                    TabiCore::TABLE_GET_BY_ID.create.name, TabiCore::TABLE_GET_BY_ID.build.functionType
            );
            row = builder.CreateCall(coreTableGetRow, llvm::ArrayRef(args)); 
        }
        //Get the field store
        llvm::Value* fieldStore;
        {
            std::vector<llvm::Value*> offsets = {
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, valueRef->row.parse.fieldIndex)) 
            };
            fieldStore = builder.CreateGEP(
                    valueRef->common.parse.parent->common.parse.type->common.build.llvmType,
                    valueRef->common.parse.parent->common.build.llvmStore,
                    llvm::ArrayRef(offsets));
        }
        //now load the actual pointer to elements
        llvm::Value* arrayStore = builder.CreateLoad(valueRef->common.parse.type->common.build.llvmType->getPointerTo(), fieldStore);
        //Get the element store
        {
            std::vector<llvm::Value*> offsets = {
                row
            };
            valueRef->common.build.llvmStore = builder.CreateGEP(valueRef->common.parse.type->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets)); 
        }
    }
    //Now if there has been a query, we do an extra load. 
    if(valueRef == fine && valueRef->common.parse.query)
    {
        valueRef->common.build.llvmStore = builder.CreateLoad(
            valueRef->common.parse.type->common.build.llvmType->getPointerTo(), 
            valueRef->common.build.llvmStore);
    }
}

void tabic::buildTableDelete(TableDelete* tableDelete)
{
    buildValueRef(tableDelete->parse.tableRef, nullptr); 
    buildExpression(tableDelete->parse.id); 
    Slab* hostSlab = tableDelete->common.parse.hostFunction->create.hostSlab;
    llvm::FunctionCallee coreTableDelete = hostSlab->build.llvmModule->getOrInsertFunction( TabiCore::TABLE_DELETE_BY_ID.create.name, TabiCore::TABLE_DELETE_BY_ID.build.functionType); 
    std::vector<llvm::Value*> args = {
        tableDelete->parse.tableRef->common.build.llvmStore,
        tableDelete->parse.tableRef->common.parse.type->table.parse.numRows->common.build.llvmValue,
        tableDelete->parse.id->common.build.llvmValue
    };
    builder.CreateCall(coreTableDelete, llvm::ArrayRef(args)); 
}

void tabic::buildTableMeasure(TableMeasure* tableMeasure)
{
    buildValueRef(tableMeasure->parse.tableRef, nullptr); 
    buildValueRef(tableMeasure->parse.usedRef, nullptr);
    Slab* hostSlab = tableMeasure->common.parse.hostFunction->create.hostSlab;
    llvm::FunctionCallee coreTableGetNumUsed = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::TABLE_GET_NUM_USED.create.name, TabiCore::TABLE_GET_NUM_USED.build.functionType);
    std::vector<llvm::Value*> args = {
        tableMeasure->parse.tableRef->common.build.llvmStore,
        tableMeasure->parse.tableRef->common.parse.type->table.parse.numRows->common.build.llvmValue
    };
    llvm::Value* numUsed = builder.CreateCall(coreTableGetNumUsed, llvm::ArrayRef(args));
    builder.CreateStore(numUsed, tableMeasure->parse.usedRef->common.build.llvmStore);
}

void tabic::buildTableCrunch(TableCrunch* tableCrunch)
{
    buildValueRef(tableCrunch->parse.tableRef, nullptr); 
    if(tableCrunch->parse.idRef) buildValueRef(tableCrunch->parse.idRef, nullptr); 
    Slab* hostSlab = tableCrunch->common.parse.hostFunction->create.hostSlab;
    int numFields = tableCrunch->parse.tableRef->common.parse.type->table.parse.fields.size() - 2;
    llvm::FunctionCallee coreTableCrunch = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::TABLE_CRUNCH.create.name, TabiCore::TABLE_CRUNCH.build.functionType);
    llvm::Value* fieldSizes = builder.CreateAlloca(SupportedPrimitives::INT.common.build.llvmType, 
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, numFields));
    llvm::DataLayout dl = hostSlab->build.llvmModule->getDataLayout();
    for(int i = 0; i < numFields; i++)
    {
        std::vector<llvm::Value*> offsets = { 
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, i))
        };
        llvm::Value* elemPtr = builder.CreateGEP(SupportedPrimitives::INT.common.build.llvmType, fieldSizes, llvm::ArrayRef(offsets));
        builder.CreateStore(
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, 
                    llvm::APInt(32, dl.getTypeAllocSize(tableCrunch->parse.tableRef->common.parse.type->table.parse.fields[i].type->common.build.llvmType))),
                elemPtr);
    }
    std::vector<llvm::Value*> args;
    if(tableCrunch->parse.idRef) {
        args = {
            tableCrunch->parse.tableRef->common.build.llvmStore,
            tableCrunch->parse.tableRef->common.parse.type->table.parse.numRows->common.build.llvmValue,
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType,
                    llvm::APInt(32, numFields)),
            fieldSizes,
            tableCrunch->parse.idRef->common.build.llvmStore 
        };
    }
    else
    {
        args = {
            tableCrunch->parse.tableRef->common.build.llvmStore,
            tableCrunch->parse.tableRef->common.parse.type->table.parse.numRows->common.build.llvmValue,
            llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType,
                    llvm::APInt(32, numFields)),
            fieldSizes,
            llvm::Constant::getNullValue(SupportedPrimitives::INT.common.build.llvmType->getPointerTo())
        };
    }
    builder.CreateCall(coreTableCrunch, llvm::ArrayRef(args));
}

void tabic::buildLabel(Label* label)
{
    buildExpression(label->parse.address);
    buildValueRef(label->parse.fuzzyRef, nullptr); 
    builder.CreateStore(label->parse.address->common.build.llvmValue, label->parse.fuzzyRef->common.build.llvmStore);
}

void tabic::buildUnheap(Unheap* unheap)
{
    buildExpression(unheap->parse.address);
    if(unheap->parse.structure)
    {
        buildType(unheap->parse.structure); 
        deallocType(unheap->parse.structure, unheap->parse.address->common.build.llvmValue, (Function*) unheap->common.parse.hostFunction); 
    }
    else
    {
        deallocType(unheap->parse.address->common.parse.type->address.parse.pointsTo, unheap->parse.address->common.build.llvmValue, (Function*) unheap->common.parse.hostFunction); 
    }
}

void tabic::deallocType(Type* type, llvm::Value* store, Function* hostFunction, bool deallocBase)
{
    Slab* hostSlab = hostFunction->tabitha.create.hostSlab;
    if(type->common.typeClass == TYPE_PRIMITIVE)
    {
        //simply deallocate the memory according to the primitive's pointer
        std::vector<llvm::Value*> args = { store };
        llvm::FunctionCallee coreDealloc = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::DEALLOC.create.name, TabiCore::DEALLOC.build.functionType);
        if(deallocBase) builder.CreateCall(coreDealloc, llvm::ArrayRef(args)); 
    }
    else if(type->common.typeClass == TYPE_ADDRESS)
    {
        //simply deallocate the memory according the the pointer to the address
        std::vector<llvm::Value*> args = { store };
        llvm::FunctionCallee coreDealloc = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::DEALLOC.create.name, TabiCore::DEALLOC.build.functionType);
        if(deallocBase) builder.CreateCall(coreDealloc, llvm::ArrayRef(args)); 
    }
    else if(type->common.typeClass == TYPE_COLLECTION)
    {
        //must make sure we properly deallocate the members (which may include vectors etc. )before deallocating the 
        //memory for the CollectionType itself. 
        for(auto pair : type->collection.parse.members)
        {
            std::vector<llvm::Value*> offsets = { 
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, pair.second.index)),
            };
            llvm::Value* memberPtr = builder.CreateGEP(type->common.build.llvmType, store, llvm::ArrayRef(offsets));  
            if(pair.second.type->common.typeClass == TYPE_VECTOR)
            {
                deallocType(pair.second.type, memberPtr, hostFunction, false);  
            }
            else if(pair.second.type->common.typeClass == TYPE_COLLECTION)
            {
                deallocType(pair.second.type, memberPtr, hostFunction, false); 
            }
        }
        std::vector<llvm::Value*> args = { store };
        llvm::FunctionCallee coreDealloc = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::DEALLOC.create.name, TabiCore::DEALLOC.build.functionType);
        if(deallocBase) builder.CreateCall(coreDealloc, llvm::ArrayRef(args)); 
    }
    else if(type->common.typeClass == TYPE_VECTOR)
    {
        //if the element type is another vector, a table or collection, we must properly deallocated all of these first 
        llvm::Value* arrayStore = builder.CreateLoad(type->common.build.llvmType, store);
        if(type->vector.parse.elemType->common.typeClass == TYPE_VECTOR || type->vector.parse.elemType->common.typeClass == TYPE_COLLECTION)
        {
            buildExpression(type->vector.parse.numElem); 
            llvm::Value* indexStore = builder.CreateAlloca(SupportedPrimitives::INT.common.build.llvmType); 
            builder.CreateStore(llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)), indexStore); 
            llvm::BasicBlock* vecDeallocCondition = llvm::BasicBlock::Create(llvmContext, "vec_dealloc_condition", hostFunction->common.build.llvmFunction); 
            builder.CreateBr(vecDeallocCondition); 
            builder.SetInsertPoint(vecDeallocCondition); 
            llvm::Value* index = builder.CreateLoad(SupportedPrimitives::INT.common.build.llvmType, indexStore); 
            llvm::Value* condition = builder.CreateICmpSLT(index, type->vector.parse.numElem->common.build.llvmValue); 
            llvm::BasicBlock* vecDeallocBody = llvm::BasicBlock::Create(llvmContext, "vec_dealloc_body", hostFunction->common.build.llvmFunction); 
            builder.SetInsertPoint(vecDeallocBody); 
            std::vector<llvm::Value*> offsets = {
                index
            };
            llvm::Value* elementPtr = builder.CreateGEP(type->vector.parse.elemType->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets)); 
            if(type->vector.parse.elemType->common.typeClass == TYPE_VECTOR)
            {
                deallocType(type->vector.parse.elemType, elementPtr, hostFunction, false); 
            }
            else if(type->vector.parse.elemType->common.typeClass == TYPE_COLLECTION)
                deallocType(type->vector.parse.elemType, elementPtr, hostFunction, false); 
            llvm::Value* next = builder.CreateAdd(index, llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 1))); 
            builder.CreateStore(next, indexStore); 
            builder.CreateBr(vecDeallocCondition); 
            llvm::BasicBlock* vecDeallocEnd = llvm::BasicBlock::Create(llvmContext, "vec_dealloc_end", hostFunction->common.build.llvmFunction); 
            builder.SetInsertPoint(vecDeallocCondition); 
            builder.CreateCondBr(condition, vecDeallocBody, vecDeallocEnd); 
            builder.SetInsertPoint(vecDeallocEnd); 
        }
        llvm::FunctionCallee coreDealloc = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::DEALLOC.create.name, TabiCore::DEALLOC.build.functionType);
        std::vector<llvm::Value*> args = { arrayStore };
        builder.CreateCall(coreDealloc, llvm::ArrayRef(args)); 
        args[0] = store; 
        if(deallocBase) builder.CreateCall(coreDealloc, llvm::ArrayRef(args)); 
    }
    else if(type->common.typeClass == TYPE_TABLE)
    {
        llvm::FunctionCallee coreDealloc = hostSlab->build.llvmModule->getOrInsertFunction(TabiCore::DEALLOC.create.name, TabiCore::DEALLOC.build.functionType);
        for(int fieldIndex = 0; fieldIndex < type->table.parse.fields.size(); fieldIndex++)
        {
            TableField &field = type->table.parse.fields[fieldIndex]; 
            std::vector<llvm::Value*> offsets = {
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, fieldIndex)),
            };
            llvm::Value* fieldStore = builder.CreateGEP(type->common.build.llvmType, store, llvm::ArrayRef(offsets)); 
            llvm::Value* arrayStore = builder.CreateLoad(field.type->common.build.llvmType->getPointerTo(), fieldStore); 
            if(field.type->common.typeClass == TYPE_VECTOR || field.type->common.typeClass == TYPE_COLLECTION)
            {
                buildExpression(type->table.parse.numRows); 
                llvm::Value* indexStore = builder.CreateAlloca(SupportedPrimitives::INT.common.build.llvmType); 
                builder.CreateStore(llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)), indexStore); 
                llvm::BasicBlock* vecDeallocCondition = llvm::BasicBlock::Create(llvmContext, "field_dealloc_condition", hostFunction->common.build.llvmFunction); 
                builder.CreateBr(vecDeallocCondition); 
                builder.SetInsertPoint(vecDeallocCondition); 
                llvm::Value* index = builder.CreateLoad(SupportedPrimitives::INT.common.build.llvmType, indexStore); 
                llvm::Value* condition = builder.CreateICmpSLT(index, type->table.parse.numRows->common.build.llvmValue); 
                llvm::BasicBlock* vecDeallocBody = llvm::BasicBlock::Create(llvmContext, "field_dealloc_body", hostFunction->common.build.llvmFunction); 
                builder.SetInsertPoint(vecDeallocBody); 
                std::vector<llvm::Value*> offsets = {
                    index
                };
                llvm::Value* elementPtr = builder.CreateGEP(field.type->common.build.llvmType, arrayStore, llvm::ArrayRef(offsets)); 
                if(field.type->common.typeClass == TYPE_VECTOR)
                {
                    deallocType(field.type, elementPtr, hostFunction, false); 
                }
                else if(field.type->common.typeClass == TYPE_COLLECTION)
                    deallocType(field.type, elementPtr, hostFunction, false); 
                else if(field.type->common.typeClass == TYPE_TABLE)
                    deallocType(field.type, elementPtr, hostFunction, false); 
                    
                llvm::Value* next = builder.CreateAdd(index, llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 1))); 
                builder.CreateStore(next, indexStore); 
                builder.CreateBr(vecDeallocCondition); 
                llvm::BasicBlock* vecDeallocEnd = llvm::BasicBlock::Create(llvmContext, "field_dealloc_end", hostFunction->common.build.llvmFunction); 
                builder.SetInsertPoint(vecDeallocCondition); 
                builder.CreateCondBr(condition, vecDeallocBody, vecDeallocEnd); 
                builder.SetInsertPoint(vecDeallocEnd); 
            }
            std::vector<llvm::Value*> args = { arrayStore };
            builder.CreateCall(coreDealloc, llvm::ArrayRef(args)); 
        }
        std::vector<llvm::Value*> args = { store };
        if(deallocBase) builder.CreateCall(coreDealloc, llvm::ArrayRef(args)); 
    }
}

void tabic::deallocSubvectors(Type* type, llvm::Value* store, Function* hostFunction)
{
    if(type->common.typeClass == TYPE_COLLECTION)
    {
        for(auto pair : type->collection.parse.members)
        {
            if(pair.second.type->common.typeClass == TYPE_VECTOR)
            {
                std::vector<llvm::Value*> offsets = { 
                    llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                    llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, pair.second.index)),
                };
                llvm::Value* memberPtr = builder.CreateGEP(type->common.build.llvmType, store, llvm::ArrayRef(offsets));  
                deallocType(pair.second.type, memberPtr, hostFunction);  
            }
            else if(pair.second.type->common.typeClass == TYPE_COLLECTION)
            {
                std::vector<llvm::Value*> offsets = { 
                    llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, 0)),
                    llvm::ConstantInt::get(SupportedPrimitives::INT.common.build.llvmType, llvm::APInt(32, pair.second.index)),
                };
                llvm::Value* memberPtr = builder.CreateGEP(type->common.build.llvmType, store, llvm::ArrayRef(offsets));  
                deallocSubvectors(pair.second.type, memberPtr, hostFunction); 
            }
        }
    }
}

void tabic::allocateHeapHandles(Block* block)
{
    for(auto pair : block->parse.variables)
    {
        Variable* variable = pair.second;
        if(variable->common.variableClass == VARIABLE_HEAPED)
        {
            buildType(variable->common.parse.type); 
            variable->common.build.llvmStore = builder.CreateAlloca(variable->common.parse.type->common.build.llvmType->getPointerTo(), nullptr, variable->common.parse.name);
        }
    }
    for(Statement* statement : block->parse.statements)
    {
        StatementClass statementClass = statement->common.statementClass;

        if(statementClass == STATEMENT_BLOCK)
        {
            allocateHeapHandles((Block*)statement);
        }
        else if(statementClass == STATEMENT_CONDITIONAL)
        {
            allocateHeapHandles(statement->conditional.parse.pair.block);
        }
        else if(statementClass == STATEMENT_BRANCH)
        {
            for(ConditionBlockPair &twig : statement->branch.parse.twigs)
            {
                allocateHeapHandles(twig.block);
            }
            if(statement->branch.parse.otherwiseBlock) allocateHeapHandles(statement->branch.parse.otherwiseBlock);
        }
        else if(statementClass == STATEMENT_LOOP)
        {
            allocateHeapHandles(statement->loop.parse.directions);
        }
    }
}


