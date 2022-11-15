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

#include<map>

/** @brief A static collection of data, which does not need to be explicitly captured in order to be used.
 * 
 * Tabitha good practice does not recommend extensive use of this, but these may be useful for the sake of 
 * implementing global constants (especially for an API binding) or for debugging purposes. 
 */
struct tabic::Dump
{
    struct
    {
        Slab* hostSlab = nullptr;
        std::string name = "";  ///< The name used to refer to the Dump within hostSlab.
        ASTNode node = nullptr; 
    } create;

    struct
    {
        std::map<std::string, DumpVariable*> members;     ///< Gives the member corresponding to its name. 
    } parse;

    Dump(ASTNode node, Slab* hostSlab)
    {
        create.node = node; 
        create.hostSlab = hostSlab;
    }
}; 
