#[===[
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
]===]

cmake_minimum_required(VERSION 3.0)

project(Tabitha_SDK)

enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

if(WIN32)
    add_compile_definitions(WINDOWS)
endif()

add_subdirectory(src/tabic) 
add_subdirectory(src/tabi_core)
add_subdirectory(src/tabi_std)

target_include_directories(tabic PUBLIC include)

set_target_properties(tabic
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 17)

set_target_properties(tabi_std_cross
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_target_properties(tabi_std_raw
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_target_properties(tabi_core_cross
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_target_properties(tabi_core_raw
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

install(TARGETS tabic DESTINATION bin)
install(TARGETS tabi_std_cross tabi_std_raw tabi_core_cross tabi_core_raw DESTINATION lib)
install(DIRECTORY res/grammar DESTINATION res) 
install(FILES src/tabi_std/std.tabi DESTINATION lib) 

