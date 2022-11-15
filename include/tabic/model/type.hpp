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

#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/Type.h"

#include<map>


/** @brief Data common to all elements of the union Type. 
 *
 * All types must have at least:
 * - a corresponding llvm::Type* 
 *
 * Note that, for instance, this list does not include a `name` for the type. 
 * Not all types have a name, for example table types are anonymous unless they 
 * are declared under an alias. 
 */
struct tabic::TypeCommon
{
    TypeClass typeClass = TYPE_NONE; 

    struct
    {
        bool topLevel = false;                  ///< Whether or not this Type is declared as a top-level object in the slab (this is relevant for cleanup) 
    } create; 

    struct
    {
        llvm::Type* llvmType = nullptr; ///< The llvm::Type* corresponding to this model Type. 
    } build; 
}; 

/** @brief One of a limited number of types labeled as *primitive* or *foundational*. 
 *
 * Tabitha has four primitive types:
 * - Integers (`Int`)
 * - Floating point numbers (`Float`)
 * - Characters (`Char`)
 * - Truth Values (`Truth`)
 *
 * These should be the same no matter the bundle, and hence should be known at the creation stage. 
 */
struct tabic::PrimitiveType
{
    TypeCommon common; 

    /* NOTE: Not sure `name` is really necessary here
    struct
    {
        std::string name = ""; ///< Name of the type e.g. `"Int"`
    } create;
    */

    PrimitiveType()
    {
        common.typeClass = TYPE_PRIMITIVE; 
    }

    ~PrimitiveType()
    {
    }
}; 

/** @brief A type constructed as a set of other known types as members. 
 *
 * These are similar to C structs.
 *
 */
struct tabic::CollectionType
{
    TypeCommon common; 
    
    struct
    {
        std::string name = "";      ///< Name of the type e.g. `"Point"`
        Slab* hostSlab = nullptr;   ///< The Slab to which the CollectionType belongs.
        ASTNode node;               ///< The PEG AST node which defines the CollectionType.
    } create; 

    struct
    {
        std::map<std::string, CollectionMember> members; ///< This gives the CollectionMember corresponding to its name.
                                                          ///These are computed in the Parsing Phase, since we need to know about
                                                          ///all Type s vailable, and these are unknown in the Creation Phase. 
    } parse; 

    CollectionType(ASTNode node, Slab* hostSlab)
    {
        common.typeClass = TYPE_COLLECTION; 
        create.hostSlab = hostSlab;
        create.node = node; 
    }

    ~CollectionType()
    {
    }
}; 

/** @brief Data relating to a member of a `CollectionType`.
 */
struct tabic::CollectionMember
{
    Type* type = nullptr;   ///< The Type of the member.   
    int index = -1;         ///< The index of the member within its CollectionType. 
                            ///This is important when referencing the member. 
}; 

/** @brief A type representing the memory address to data of a given type. 
 *
 * e.g. `Addr[Int]` is an AddressType, representing the memory address of an integer. 
 */
struct tabic::AddressType
{
    TypeCommon common;

    struct
    {
        Type* pointsTo = nullptr;   ///< The Type for which this is an address.
    } parse;

    AddressType()
    {
        common.typeClass = TYPE_ADDRESS;
    }

    ~AddressType(){}
};

/** @brief A type representing a vector of values of a given type. 
 *
 * e.g. `Vec[Int, 10]` is a VectorType, representing a vector of integers that is `10` elements long. 
 */
struct tabic::VectorType
{
    TypeCommon common;

    struct
    {
        Type* elemType = nullptr;           ///< The Type which elements of the vector belong to.
        Expression* numElem = nullptr;      ///< The Expression representing the number of elements in the vector. 
    } parse;

    VectorType()
    {
        common.typeClass = TYPE_VECTOR;
    }

    ~VectorType(){}
}; 

/** @brief A pair consisiting of a Type and a name for a table field. 
 */
struct tabic::TableField
{
    Type* type;         ///< The Type of the field. 
    std::string name;   ///< The name of the field. 
}; 

/** @brief A type representing a Codd table with columns of given types. 
 *
 * e.g. `Table[Int, Float, Addr[Char], 10]` is a TableType with three columns of types `Int`, `Float`, `Addr[Char]`, and with `10` rows. 
 */
struct tabic::TableType
{
    TypeCommon common; 
    
    struct
    {
        std::vector<TableField> fields = {};    ///< The Type belonging to each field (including the `id` and `use` fields).
        Expression* numRows;                    ///< The Expression representing the number of rows the table has. 
    } parse;

    TableType()
    {
        common.typeClass = TYPE_TABLE; 
    }

    ~TableType(){}
}; 

/** @brief Effectively a placeholder during the Creation phase. 
 */
struct tabic::AliasType
{
    TypeCommon common; 

    struct
    {
        ASTNode node = nullptr; 
        Slab* hostSlab = nullptr; 
        std::string name = "";
    } create;

    struct
    {
        Type* repType = nullptr; 
    } parse; 

    AliasType(ASTNode node, Slab* hostSlab)
    {
        common.typeClass = TYPE_ALIAS; 
        create.node = node; 
        create.hostSlab = hostSlab; 
    }
};

/** @brief This acts as a superstruct for all _____Type. 
 */
union tabic::Type
{
    TypeCommon common; 
    PrimitiveType primitive; 
    CollectionType collection; 
    AddressType address;
    VectorType vector; 
    TableType table; 
    AliasType alias;

    void destroy()
    {
        //the destruction of a Type is sensitive to its common.typeClass
        switch(common.typeClass)
        {
            case TYPE_PRIMITIVE:
                delete (PrimitiveType*) this; 
                break; 
            case TYPE_COLLECTION:
                delete (CollectionType*) this; 
                break; 
            case TYPE_ADDRESS:
                delete (AddressType*) this; 
                break; 
            default:
                break;
        }
    }
};

namespace tabic
{
    /** @brief Static collection of PrimitiveType. 
     *
     * These are defined to be static for the reason that 
     * primitive types are the same (at least in the in-memory model) no matter what. 
     */
    class SupportedPrimitives
    {
        public:
            static PrimitiveType SIZE;      ///< The Size Type `Size`.
            static PrimitiveType INT;       ///< The Integer Type `Int`.
            static PrimitiveType LONG;      ///< The Long Integer Type `Long` 
            static PrimitiveType SHORT;     ///< The Short Integer Type `Short`
            static PrimitiveType DOUBLE;    ///< The Double Precision Float Type `Double`
            static PrimitiveType FLOAT;     ///< The Floating Point Type `Float`.
            static PrimitiveType CHAR;      ///< The Character Type `Char`.
            static PrimitiveType TRUTH;     ///< The Truth Type `Truth`. 
            static PrimitiveType NONE;      ///< The None Type `None`. 
    };
}

