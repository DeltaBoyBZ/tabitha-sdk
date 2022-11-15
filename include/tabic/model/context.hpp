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

/** @brief A collection of static data. 
 *
 *  A Context is *capturable* in principle by any function in the Bundle. 
 *  A Context must be captured for its data to be accessed or modified. 
 */
struct tabic::Context
{
    struct
    {
        std::string name = "";      ///< The name of the Context, used for referencing. 
        Slab* hostSlab = nullptr;   ///< The Slab to which the Context belongs. 
        ASTNode node = nullptr;     ///< The ASTNode which defines the Context.
    } create;

    struct
    {
        std::map<std::string, ContextVariable*> members;    ///< Gives the ContextVariable corresponding to a name. 
    } parse; 

    Context(ASTNode node, Slab* hostSlab)
    {
        create.node = node; 
        create.hostSlab = hostSlab; 
    }
}; 

