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
/*
 * Implementations for functions defined in include/tabic/create.hpp
 *
 * See the header for function descriptions. 
 */
#include "tabic/create.hpp"
#include "tabic/model/model.hpp"
#include "tabic/util.hpp"
#include "tabic/pre.hpp"

#include<cpp-peglib/peglib.h>
#include<cstdlib>

#define WINDOWS

static tabic::CreateStatus _createStatus = tabic::CREATE_STATUS_NONE; 

#define CREATE_FAIL _createStatus = CREATE_STATUS_FAIL

static std::string TABI_LIB = std::getenv("TABI_LIB"); 
static std::string TABI_RES = std::getenv("TABI_RES");
static std::vector<std::string> _libPaths = {}; 

tabic::PrimitiveType tabic::SupportedPrimitives::SIZE; 
tabic::PrimitiveType tabic::SupportedPrimitives::INT;
tabic::PrimitiveType tabic::SupportedPrimitives::FLOAT;
tabic::PrimitiveType tabic::SupportedPrimitives::CHAR;
tabic::PrimitiveType tabic::SupportedPrimitives::TRUTH; 
tabic::PrimitiveType tabic::SupportedPrimitives::NONE; 
tabic::PrimitiveType tabic::SupportedPrimitives::LONG; 
tabic::PrimitiveType tabic::SupportedPrimitives::SHORT; 
tabic::PrimitiveType tabic::SupportedPrimitives::DOUBLE;

tabic::CoreFunction tabic::TabiCore::TABLE_INIT("core_table_init"); 
tabic::CoreFunction tabic::TabiCore::TABLE_INSERT("core_table_insertRow"); 
tabic::CoreFunction tabic::TabiCore::TABLE_GET_BY_ID("core_table_getRowByID"); 
tabic::CoreFunction tabic::TabiCore::TABLE_DELETE_BY_ID("core_table_deleteRowByID");
tabic::CoreFunction tabic::TabiCore::TABLE_GET_NUM_USED("core_table_getNumUsed"); 
tabic::CoreFunction tabic::TabiCore::TABLE_CRUNCH("core_table_crunch"); 
tabic::CoreFunction tabic::TabiCore::ALLOC("core_alloc");
tabic::CoreFunction tabic::TabiCore::DEALLOC("core_dealloc"); 
tabic::CoreFunction tabic::TabiCore::MEMCPY("core_memcpy");
tabic::CoreFunction tabic::TabiCore::SUBVECTOR_COPY("core_subvector_copy");

static std::string _cwd; 

tabic::CreateStatus tabic::createBundle(std::string rootSlabFilename, std::string cwd, Bundle** bundle)
{
    //get all _libPaths from TABI_LIB
    int k = 0;
    int k0 = 0; 
#ifdef WINDOWS
    while((k = TABI_LIB.find(";", k0)) != std::string::npos)
    {
        _libPaths.push_back(TABI_LIB.substr(k0, k - k0));
        k0 = k+1; 
    }     
#else
    while((k = TABI_LIB.find(":", k0)) != std::string::npos)
    {
        _libPaths.push_back(TABI_LIB.substr(k0, k - k0));
        k0 = k+1; 
    }     
#endif
    _libPaths.push_back(TABI_LIB.substr(k0, TABI_LIB.length() - k0)); 
    _cwd = cwd; 
    *bundle = new Bundle(rootSlabFilename); 
    //Create the PEG parser
    {
        try
        {
            std::string grammarSource = Util::readFile(TABI_RES + "/grammar/tabitha.peg"); 
            (*bundle)->create.pegParser = new peg::parser(grammarSource); 
            (*bundle)->create.pegParser->enable_ast(); 
            (*bundle)->create.pegParser->log = [bundle](size_t line, size_t col, const std::string& msg) {
                std::cerr << line << ":" << col << ": " << "Syntax error." << std::endl;
                CREATE_FAIL;
            };
        }
        catch(CannotReadFile ex)
        {
            std::cerr << ex.what() << std::endl;
            std::cerr << "Failed to read grammar file: " 
                << TABI_RES << "/grammar/tabitha.peg" 
                << std::endl; 
            CREATE_FAIL;
        }
    }
    //Create the root Slab.
    std::string rootSlabID = ""; 
    {
        int k = rootSlabFilename.find_last_of("."); 
        rootSlabID = rootSlabFilename.substr(0, k); 
    }
    (*bundle)->create.rootSlab = getOrCreateSlab(SLAB_DOM_LOCAL, rootSlabID, *bundle, nullptr); 
    if(_createStatus == CREATE_STATUS_NONE) _createStatus = CREATE_STATUS_SUCCESS;
    return _createStatus;
}

tabic::Slab* tabic::getOrCreateSlab(SlabDomain domain, std::string relativeID, Bundle* hostBundle, Slab* hostSlab)
{
    //Figure out the unique slabID.
    std::string slabID = "";
    {
        if(domain == SLAB_DOM_LOCAL)
        {
            //If this is an attachment, the slabID begins with that
            //of the hostSlab.
            if(hostSlab) 
            {
                int k1 = hostSlab->create.id.find_last_of('_'); 
                int k2 = hostSlab->create.id.find_last_of('/'); 
                slabID = hostSlab->create.id.substr(0, std::max(k1, k2) + 1); 
            }
            //Otherwise we just begin slabID with an indicator
            //that this is a LOCAL Slab.
            else
            {
                slabID = "LOCAL_";
            }
            //now add on the relative ID
            slabID += relativeID; 
        }
        else if(domain == SLAB_DOM_EXTERNAL)
        {
            slabID = "EXTERNAL_" + relativeID; 
        }
    }
    //With the slabID known, we check hostBundle for a Slab
    //with this ID. 
    for(auto pair : hostBundle->create.slabs)
    {
        if(pair.first == slabID) return pair.second;
    }
    //If we reach this point, then we must create the Slab
    return createSlab(slabID, hostBundle); 
}

tabic::Slab* tabic::createSlab(std::string id, Bundle* hostBundle)
{
    Slab* slab = new Slab(id, hostBundle); 
    hostBundle->create.slabs[slab->create.id] = slab;
    //Figure out the slab->create.filepath
    //and the basic name
    {
        int a = slab->create.id.find_first_of('_'); 
        int b = slab->create.id.find_last_of('/');
        slab->create.filepath = slab->create.id.substr(a + 1) + ".tabi";
        //For EXTERNAL Slabs, the filepath must include the TABI_LIB
        //environment variable (defined statically in this source)
        if(slab->create.id[0] == 'E') 
        {
            std::string candidatePath = _libPaths[0] + "/" + slab->create.filepath;
            int k = 1; 
            //cycle through libpaths until out file exists
            std::FILE* candidateFile = std::fopen(candidatePath.c_str(), "r"); 
            while(!candidateFile && k < _libPaths.size())
            {
                candidatePath = _libPaths[k] + "/" + slab->create.filepath; 
                candidateFile = std::fopen(candidatePath.c_str(), "r"); 
                k++; 
            }
            if(candidateFile) std::fclose(candidateFile); 
            slab->create.filepath = candidatePath;
        }
        //For LOCAL Slabs, the filepath must include the cwd
        else
        {
            slab->create.filepath = _cwd + "/" + slab->create.filepath;
        }
        slab->create.name = slab->create.id.substr(std::max(a, b) + 1); 
    }
    try
    {
        //Now read source code into slab->create.source
        slab->create.source = Util::readFile(slab->create.filepath); 
        slab->create.source = preprocess(slab->create.source);  
        //Create PEG AST for the slab
        if(!hostBundle->create.pegParser->parse(slab->create.source, slab->create.ast))
        {
            std::cerr << "Slab: " << slab->create.id << std::endl; 
        } 
        if(_createStatus == CREATE_STATUS_FAIL) return slab;  //return early if there were source parsing errors
        if(Util::flags["show-ast"])
        {
            std::cout << peg::ast_to_s(slab->create.ast) << std::endl; 
        }
        NODE_LOOP(slab->create.ast, slabSub)
        {
            //ensure that all attached slabs are created
            NODE_CHECK(slabSub, "ATTACH")
            {
                std::string relativeID = "";
                bool external = false; 
                std::string alias = ""; 
                NODE_OP(slabSub, relNode, "REL_ID")
                {
                    relativeID = relNode->token_to_string(); 
                }
                NODE_OP(slabSub, extNode, "ATTACH_EXTERNAL")
                {
                    external = true; 
                } 
                NODE_OP(slabSub, aliasNode, "ATTACH_ALIAS") {
                    alias = aliasNode->token_to_string(); 
                }
                if(alias == "") alias = relativeID;
                Slab* attachSlab;

                if(external) attachSlab = getOrCreateSlab(SLAB_DOM_EXTERNAL, relativeID, hostBundle, slab); 
                else attachSlab = getOrCreateSlab(SLAB_DOM_LOCAL, relativeID, hostBundle, slab); 

                if(slab->create.attachments[alias])
                {
                    throw DoubleAlias(alias); 
                }
                
                slab->create.attachments[alias] = attachSlab; 
            }
            //Create CollectionType.
            NODE_CHECK(slabSub, "COLLECTION_TYPE")
            {
                CollectionType* collectionType = createCollectionType(slabSub, slab);
                slab->create.types[collectionType->create.name] = (Type*) collectionType; 
            }
            //Create AliasType.
            NODE_CHECK(slabSub, "ALIAS_TYPE")
            {
                AliasType* aliasType = createAliasType(slabSub, slab); 
                //Type* container = new Type{};                         //ONLY works with clang 
                Type* container = (Type*) malloc(sizeof(Type));         //this seems to work with both gcc and clang
                std::memcpy(container, aliasType, sizeof(AliasType)); 
                slab->create.types[aliasType->create.name] = container; 
            }
            //Create Context.
            NODE_CHECK(slabSub, "CONTEXT")
            {
                Context* context = createContext(slabSub, slab); 
                slab->create.contexts[context->create.name] = context; 
            }
            //Create Dump.
            NODE_CHECK(slabSub, "DUMP")
            {
                Dump* dump = createDump(slabSub, slab); 
                slab->create.dumps[dump->create.name] = dump; 
            }
            //Create TabithaFunction.
            NODE_CHECK(slabSub, "TABITHA_FUNCTION")
            {
                TabithaFunction* tabithaFunction = createTabithaFunction(slabSub, slab);
                slab->create.functions[tabithaFunction->create.name] = (Function*) tabithaFunction; 
            }
            //Create ExternalFunction.
            NODE_CHECK(slabSub, "EXTERNAL_FUNCTION")
            {
                ExternalFunction* externalFunction = createExternalFunction(slabSub, slab);
                slab->create.functions[externalFunction->create.internalName] = (Function*) externalFunction; 
            }
            
        }
    }
    catch(CannotReadFile ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << "Could not read slab source file: " << slab->create.filepath << std::endl; 
        CREATE_FAIL;
    }
    catch(DoubleAlias ex)
    {
        std::cerr << ex.what() << std::endl; 
        std::cerr << "More than one slab goes by alias: " << ex.alias << std::endl;
        CREATE_FAIL;
    }
    return slab; 
}


tabic::TabithaFunction* tabic::createTabithaFunction(ASTNode node, Slab* hostSlab)
{
    TabithaFunction* tabithaFunction = new TabithaFunction(node, hostSlab);  
    //get the name of the function
    NODE_OP(node, nameNode, "FUNCTION_NAME")
    {
        tabithaFunction->create.name = nameNode->token_to_string(); 
    }
    return tabithaFunction;
}

tabic::CollectionType* tabic::createCollectionType(ASTNode node, Slab* hostSlab)
{
    CollectionType* collectionType = new CollectionType(node, hostSlab); 
    //get the name of collectionType
    NODE_OP(node, nameNode, "TYPE_NAME")
    {
        collectionType->create.name = nameNode->token_to_string(); 
    }
    collectionType->common.create.topLevel = true; 
    return collectionType; 
}

tabic::Context* tabic::createContext(ASTNode node, Slab* hostSlab)
{
    Context* context = new Context(node, hostSlab); 
    //Get the name of context.
    NODE_OP(node, nameNode, "CONTEXT_NAME")
    {
        context->create.name = nameNode->token_to_string(); 
    }
    return context;
}

tabic::Dump* tabic::createDump(ASTNode node, Slab* hostSlab)
{
    Dump* dump = new Dump(node, hostSlab); 
    //Get the name of dump. 
    NODE_OP(node, nameNode, "DUMP_NAME")
    {
        dump->create.name = nameNode->token_to_string(); 
    }
    return dump; 
}

tabic::ExternalFunction* tabic::createExternalFunction(ASTNode node, Slab* hostSlab)
{
    ExternalFunction* externalFunction = new ExternalFunction(node, hostSlab); 
    //Get the internal name. 
    NODE_OP(node, internalNameNode, "INTERNAL_NAME")
    {
        NODE_OP(internalNameNode, nameNode, "FUNCTION_NAME")
        {
            externalFunction->create.internalName = nameNode->token_to_string(); 
        }
    }
    return externalFunction; 
}

tabic::AliasType* tabic::createAliasType(ASTNode node, Slab* hostSlab)
{
    AliasType* aliasType = new AliasType(node, hostSlab); 
    NODE_OP(node, nameNode, "TYPE_NAME")
    {
        aliasType->create.name = nameNode->token_to_string(); 
    }
    aliasType->common.create.topLevel = true;
    return aliasType;
}



