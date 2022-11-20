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

add_executable(tabic tabic.cpp create.cpp parse.cpp build.cpp write.cpp pre.cpp util.cpp) 
execute_process(COMMAND CMD /c llvm-config --libs OUTPUT_VARIABLE TABIC_LIBS)
execute_process(COMMAND CMD /c llvm-config --libs OUTPUT_VARIABLE TABIC_SYSTEM_LIBS)

if(DEFINED TABI_LLVM_VERSION)
    execute_process(COMMAND llvm-config-${TABI_LLVM_VERSION} --libs all
                    OUTPUT_STRIP_TRAILING_WHITESPACE 
                    OUTPUT_VARIABLE llvm_libraries)
    execute_process(COMMAND llvm-config-${TABI_LLVM_VERSION} --system-libs all
                    OUTPUT_STRIP_TRAILING_WHITESPACE 
                    OUTPUT_VARIABLE llvm_sys_libraries)
else()
    execute_process(COMMAND llvm-config --libs all
                    OUTPUT_STRIP_TRAILING_WHITESPACE 
                    OUTPUT_VARIABLE llvm_libraries)
    execute_process(COMMAND llvm-config --system-libs all
                    OUTPUT_STRIP_TRAILING_WHITESPACE 
                    OUTPUT_VARIABLE llvm_sys_libraries)
endif()

if(WIN32)
	target_link_directories(tabic PUBLIC "${TABI_LLVM_INSTALL_DIR}/lib")
endif()

target_link_libraries(tabic ${llvm_libraries})
target_link_libraries(tabic ${llvm_sys_libraries})
