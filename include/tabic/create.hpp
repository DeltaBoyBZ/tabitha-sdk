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

#include"tabic/model/model.hpp"

#include<stdexcept>

namespace tabic
{

    typedef enum CreateStatus 
    {
        CREATE_STATUS_NONE, CREATE_STATUS_SUCCESS, CREATE_STATUS_FAIL
    } CreateStatus; 

    /** @brief The exception thrown when an alias is used more than once,
     * hence risking ambiguity when referencing. 
     */
    class DoubleAlias : std::exception 
    {
        public:
            std::string alias = "";

            DoubleAlias(std::string alias) : alias(alias) {}

            const char* what() const throw()
            {
                return "Used an alias twice.";
            }
    };

    /** @brief Creates the main bundle and all slabs under that bundle.
     *
     * @param rootSlabFilename The name of the root slab,
     * i.e. the slab which contains the `main` function. 
     */
    CreateStatus createBundle(std::string rootSlabFilename, std::string cwd, Bundle** bundle); 

    /** @brief Creates the Slab corresponding to the given \p filename,
     * or returns the slab if one with the corresponding filename
     * has already been created. 
     *
     * @param relativeID The ID corresponding to the Slab. 
     * This is either relative to the working directory, 
     * or relative to the \p hostSlab.
     * @param hostBundle The bundle under which the Slab exists. 
     * @param hostSlab The Slab to which the Slab we seek is to be attached (is `nullptr` in the case of the root Slab).
     */
    Slab* getOrCreateSlab(SlabDomain domain, std::string relativeID, Bundle* hostBundle, Slab* hostSlab); 

    /** @brief Creates the Slab appropriate for the given \p id.  
     *
     * @param id The ID corresponding the Slab.
     * @param hostBundle The Bundle which hosts the Slab. 
     */
    Slab* createSlab(std::string id, Bundle* hostBundle);

    /** @brief Creates the TabithaFunction described by the given node. 
     *
     * @param node The ASTNode which defines the TabithaFunction. 
     * @param hostSlab The Slab to which the TabithaFunction belongs.
     */
    TabithaFunction* createTabithaFunction(ASTNode node, Slab* hostSlab);

    /** @brief Creates the ExternalFunction described by the given node. 
     *
     * @param node The ASTNode which defines the ExternalFunction.
     * @param hostSlab The Slab to which the ExternalFunction belongs. 
     */
    ExternalFunction* createExternalFunction(ASTNode node, Slab* hostSlab); 

    /** @brief Creates the CollectionType described by the given node.
     *
     * @param node The ASTNode which defines the CollectionType. 
     * @param hostSlab The Slab to which the CollectionType belongs. 
     */
    CollectionType* createCollectionType(ASTNode node, Slab* hostSlab);

    /** @brief Reserves a Type based on an ALIAS_TYPE node. 
     *
     * @param node The ASTNode which defines the Type. 
     * @param hostSlab The Slab to which the Type belongs. 
     */
    AliasType* createAliasType(ASTNode node, Slab* hostSlab); 

    /** @brief Creates the Context described by the given \p node. 
     *
     * @param node The ASTNode which defines the Context. 
     * @param hostSlab The Slab to which the Context belongs. 
     */
    Context* createContext(ASTNode node, Slab* hostSlab); 

    /** @brief Create the Dump described by the given \p node. 
     *
     * @param node The ASTNode which defines the Context.
     * @param hostSlab The Slab to which the Dump belongs. 
     */
    Dump* createDump(ASTNode node, Slab* hostSlab);
}

