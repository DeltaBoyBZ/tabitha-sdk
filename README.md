# The Tabitha SDK 

## Introduction

Tabitha is a new programming language designed by myself. 
It is procedural like C, but unlike C it comes with a semi-safe approach to working with global data. 
It also features a rich type system, creating types with particular functionality through the action of *type functors*. 

## Getting Started

The easiest way to get started is to go to this repository's [GitHub pages](https://deltaboybz.github.io/tabitha-sdk). 
You can also go directly to the [Tabitha Programming Guide](https://deltaboybz.github.io/tabitha-sdk/guide/introduction.html). 

## System Requirements and Dependencies
As of now, the Tabitha SDK is only known to build on Linux. 
Specifically, the development machine was running Debian 11. 
Simple testing has also been done on Ubuntu 20.02. 

1. [The LLVM Project](https://github.com/llvm/llvm-project) - Main development used LLVM-16, however testing suggests LLVM-15 is sufficient. 

2. [cpp-peglib](https://github.com/yhirose/cpp-peglib) - A parsing library used to process Tabitha source files fed to `tabic`.

## Building the SDK  
Please see [`BUILD.md`](BUILD.md) for detailed building and installation instructions.  

## Licensing

See [NOTICE.txt](NOTICE.txt).

The Tabitha SDK is Copyright 2022 Matthew Peter Smith.

The Tabitha Compiler (`src/tabic`) and its static resources are provided under the [Apache License - Version 2.0](APACHE_LICENSE_V2.txt).

The Tabitha Core Library (`src/tabi_core`) and Tabitha Standard Library (`src/tabi_std`) files are provided under the [Mozilla Public License - Version 2.0](MOZILLA_PUBLIC_LICENSE_V2.txt). 

If in doubt, check each source file's copyright notice for its corresponding licence. 

For users, we note the following points: 

1. Software compiled using `tabic`, if otherwise original, is not considered derivative of the compiler. 
   
2. The Tabitha Core Library and Tabitha Standard Library have licenses dictating distribution on a file-by-file basis. 
   Hence sofware statically linked to their executable forms is **not**  considered derivative work.

These points should reassure you that projects written using the Tabitha language remain your own intellectual property. 
Redistributions and modifications of any of these components are however a different matter, 
and for this you should consult the respective licences. 

The Tabitha SDK has two main dependencies, each provided as a Git submodule. 
For transparency, we reproduce their licences in the directory [dependency_licences](dependency_licences)

- [The LLVM Project](llvm-project) is licensed according to [`dependency-licences/LLVM_PROJECT_LICENCE.txt`](dependency-licences/LLVM_PROJECT_LICENCE.txt). 
  
- [cpp-peglib](cpp-peglib) is licensed according to [`dependency-licences/CPP_PEGLIB_LICENCE.txt`](dependency-licences/CPP_PEGLIB_LICENCE.txt). 


