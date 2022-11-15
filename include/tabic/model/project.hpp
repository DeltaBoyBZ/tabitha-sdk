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

#include"tabic/model/datatypes.hpp"
#include"tabic/model/function.hpp"
#include"tabic/model/context.hpp"
#include"tabic/model/dump.hpp"
#include"tabic/model/type.hpp"

#include<iostream>
#include<map>

#include<cpp-peglib/peglib.h>

namespace tabic
{
    /** @brief This corresponds to a single Tabitha source file. 
     */
    struct Slab
    {
        struct
        {
            std::string name        = "";   ///< The shortened name of the Slab.
            std::string id          = "";   ///< This is unique to every Slab.
            std::string source      = "";   ///< The source code, i.e. the contents of the `.tabi` file.
            std::string filepath    = "";   ///< The file path of the `.tabi` file.
            std::shared_ptr<peg::Ast> ast;  ///< The PEG AST as defined by the grammar file `tabitha.peg`
            std::map<std::string, Slab*> attachments;   ///< Gives the *attached* Slab corresponding to its alias. 
            std::map<std::string, Function*> functions; ///< Gives the Function (either TabithaFunction or ExternalFunction) corresponding to its name or alias. 
            std::map<std::string, Type*>     types;     ///< Gives the Type corresponding to its name or alias. 
            std::map<std::string, Context*>  contexts;  ///< Gives the Context corresonding to its name.
            std::map<std::string, Dump*>     dumps;     ///< Gives the Dump corresponding to its name. 
            Bundle* hostBundle = nullptr;   ///< The Bundle to which this Slab belongs. 
        } create;

        struct
        {
            llvm::Module* llvmModule = nullptr;     ///< The LLVM Module this Slab represents.
        } build;

        Slab(std::string id, Bundle* hostBundle)
        {
            create.id = id;
            create.hostBundle = hostBundle; 
        }

        ~Slab()
        {
            //delete the functions
            for(auto pair : create.functions)   if(pair.second) pair.second->destroy();
            //delete the contents
            for(auto pair : create.contexts)    if(pair.second) delete pair.second; 
            //delete the dumps
            for(auto pair : create.dumps)       if(pair.second) delete pair.second; 
            //delete the types
            for(auto pair : create.types)       if(pair.second) pair.second->destroy();
            //NOTE: We do not delete attach slabs, but defer this responsibility to create.hostBundle.
        }
    };

    /** @brief This is the base object of a Tabitha program. 
     *
     * A Bundle constains a collection of Slab, one of which is declared the `rootSlab`. 
     * In the case of an executable, this is the Slab which contains the `main` function.
     */
    struct Bundle
    {
        struct
        {
            peg::parser* pegParser = nullptr;   ///< From cpp-peglib, an object which parses source text to produce and Abstract Syntax Tree (AST)
            std::map<std::string, Slab*> slabs; ///< Gives the Slab corresponding to its unique ID.    
            std::string rootDir = "";           ///< The directory which contains rootSlab
            std::string rootSlabFilename = "";  ///< The filename corresponding to rootSlab
            Slab* rootSlab = nullptr;           ///< The Slab which contains the `main` function.
        } create; 

        struct
        {
            llvm::Function* init = nullptr;         ///< The function which deals with slab initialisation, primarily allocation of vector types in contexts 
            llvm::Function* destroy = nullptr;      ///< The function which deals with slab cleanup 
            llvm::BasicBlock* initEntry = nullptr;
            llvm::BasicBlock* destroyEntry = nullptr;
            llvm::BasicBlock* initExit = nullptr; 
        } build;

        Bundle(std::string rootSlabFilename)
        {
            create.rootSlabFilename = rootSlabFilename;
        }

        ~Bundle()
        {
            delete create.pegParser;
            for(auto pair : create.slabs) delete pair.second; 
        }
    }; 
}

