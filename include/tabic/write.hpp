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

#include"model/model.hpp"

namespace tabic
{
    /** @brief Writes the output associated with a Bundle.
     *
     * To write a Bundle, is to output all the compiled files to disk. 
     */
    void writeBundle(Bundle* bundle);

    /** @brief Writes the output associated with a Slab.
     *
     * This output is in the form of LLVM bytecode (`.bc`) files or C object files (`.o`). 
     */
    void writeSlab(Slab* slab);
}


