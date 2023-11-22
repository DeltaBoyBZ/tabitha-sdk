# The Tabitha SDK 

## Introduction

Tabitha is a new programming language designed by myself. 
It is procedural like C, but unlike C it comes with a semi-safe approach to working with global data. 
It also features a rich type system, creating types with particular functionality through the action of *type functors*. 

## Development Status

This version of the SDK was a proof-of-concept, and is not
being actively developed. There is a new version of the
compiler being privately developed, which I ham hoping to
release in 2024. 

The new version of the SDK makes a fair few changes to what
is presented here. The new version in particular sports an
improved version of the slab and bundle system. The biggest
change is in the design of the compiler itself. Development
efforts have been focused far more on creating a stable,
performant, and extensible framework for compiling Tabitha
code (and maybe some other languages in the future).

The language itself is mostly unchanged aside from some
keywords. The aim of Tabitha is still to de-stigmatise
global data, and to optimise for relational state. To this
end, I therefore welcome critiques of the language as it is
presented in this repository and in the [Tabitha Programming Guide](https://deltaboybz.github.io/tabitha-sdk/guide/introduction.html)

Work on Tabitha has been slow over the past couple of
months, since I must admit a case of burnout with the
project. I have been taking a break from developing the
compiler itself and instead have been re-examining my
philosophy on software design more generally. More
specifically, I have been considering what it really means
to be an data-oriented programmer and have been
experimenting with patterns which reject the abstraction of
data into entities. It is possible that these developments
could somehow make their way into Tabitha e.g. through type
functors (like `Table`). I am very wary however of *feature
creep* and  perhaps should just focus on getting the base
compiler to the same usable standard as the public one. 

If you have read the above, I wish I could refund your time.
That being said, you are free to enjoy the Tabitha language
as it stands. :)

## Getting Started

If you are on Debian/Ubuntu or Windows, you can check out the project's [Releases](https://github.com/DeltaBoyBZ/tabitha-sdk/releases).
Otherwise, you will need to build from source (see below). 

The easiest way to learn is to go to this repository's [GitHub pages](https://deltaboybz.github.io/tabitha-sdk). 
You can also go directly to the [Tabitha Programming Guide](https://deltaboybz.github.io/tabitha-sdk/guide/introduction.html). 

## System Requirements and Dependencies
The Tabitha SDK builds on both Linux and Windows, though the Windows version is less-throughly tested. 
The main development machine was running Debian 11. 

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


