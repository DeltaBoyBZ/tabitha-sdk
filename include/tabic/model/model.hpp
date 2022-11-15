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

#include"project.hpp"
#include"type.hpp"
#include"context.hpp"
#include"dump.hpp"
#include"function.hpp"
#include"statement.hpp"
#include"variable.hpp"
#include"expression.hpp"

#include<cpp-peglib/peglib.h>

#define NODE_LOOP(node, sub) for(ASTNode sub : node->nodes)
#define NODE_CHECK(node, name0) if(node->name == name0)
#define NODE_OP(node, sub, name0) NODE_LOOP(node, sub) NODE_CHECK(sub, name0)


