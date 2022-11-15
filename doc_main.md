# Introduction

The Tabitha SDK is a set of tools for building Tabitha applications. 
It contains `tabic`, the first official Tabitha compiler. 
It also contains the Tabitha Core Libraries and Tabitha Standard Libraries. 

# Stages of Compilation

Tabitha compilation occurs in three main stages:
- **Creation** - The creation of top-level objects ready to be references and further processed. 
- **Parsing**  - The translation of all source code into in-memory model objects. 
- **Building** - The creation of LLVM modules according to each Slab in the memory model. 

The **In-Memory Model** is a set of structs designed to represent a Tabitha program during compilation. 

The **Creation** stage is described in include/tabic/create.hpp and implemented in src/tabic/create.cpp. 

The **Parsing** stage is described in include/tabic/parse.hpp and implemented in src/tabic/parse.cpp. 

The **Building** stage is described in include/tabic/build.hpp and implemented in src/tabic/build.cpp. 

# Bundles and Slabs

Tabitha calls its projects **Bundles**. 
A Bundle can represent either an executable, or a libary to be used by another project (be it Tabitha or otherwise).
Bundles are named as such as they are a collection of *Slabs*. 
A Slab is what is represented by a single Tabitha source file (`.tabi`). 
Tabic uses a Bundle datatype defined in include/tabic/model/project.hpp. 

# Functions 

Functions in Tabitha come in three forms: 

- Tabitha Functions - These are defined wholly within a Tabitha slab.
- External Functions - These are declared in a Tabitha slab and used as if they belong to that slab, but are defined in an external object file. 
- Core Functions - These are typically not used by Tabitha users, but are called within Tabitha code to deal with more complex operations. 
  
  
# Contexts





