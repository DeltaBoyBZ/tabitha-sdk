# The Type System
## Primitive Types
Like most type systems, Tabitha has a privileged set of types it considers foundational. 
These are, in no particular order: 

* `Int` - The Integer Type - The counting numbers $1, 2, 3, \dots$, their negative counterparts $-1, -2, -3, \dots$, and zero ($0$). 
* `Float` - The Floating Point Type - Numbers on the real number line - e.g. 5.77, 3.14, -66.1. 
* `Char` - The Character Type - Characters representable according the ASCII standard - e.g. 'a', 'z', '!'. 
* `Truth` - The Truth Type - Either `true` or `false` - Other languages might call this a *Boolean*.  
* `None` - The lack of a type.  
* `Long` - Long integers. 
* `Short` - Short integers.
* `Double` - Double precision floating point numbers. 

The first few types should be understandable to most people. 
It is possible for one to question the utility of `None`, but this is usually used for *functions* which don't return a value (but may have side-effects). 
It is also used in some function declarations to signify generic pointers as `Addr[None]`.  
The point of `Long` is to store integers which require more than the usual 32 bytes to encode. 
The point of `Short` is to store integers which do not require as much has 32 bytes, for situations where memory is at a premium. 
The `Double` type is necessary for some scientific applications. 

## Collection Types
Primitives are capable of a lot, but at some point it is useful to define more complex types. 
Tabitha's **collection types** are very similar to *structs* in C. 
Here is an example. 

    collection type Point {
        Float x
        Float y
    }
    
This snippet defines a new type `Point` which contains two `Float` members. 
Collection types can contain any number of members, and these members can be of any type. 

    collection type Person {
        Int age
        Float height
        Char favouriteLetter
    }
    
## Address Types
Sometimes it is more useful to deal with the *memory address* of data, rather than the data itself. 
This can be for the sake of convenience, or it may be to allow direct mutability by subroutines. 
Furthermore, performance can be gained from using pointers effectively. 

In Tabitha, memory addresses have their own types which reflect the type of data found at that address. 
For example, `Addr[Int]` is the type for a memory address where we find an integer. 
We can apply `Addr` to any type to obtain a new type. 
We can even construct something like `Addr[Addr[Int]`, which is the type for data representing a memory address, at which you find data for another memory address. 

Tabitha calls `Addr` a **functor**, which acts on Tabitha types.
It should be noted however that the application of that term has no real deep meaning from category theory, as far as the language is concerned. 
It is just a helpful label for `Addr` along with similar objects. 
    
## Vector Types
Another example of a Tabitha functor is the `Vec` functor. 
This is a slightly more complex functor, taking two arguments. 
For example `Vec[Int, 10]` is a type for data representing a vector of `Int` data, `10` elements long. 
Individual elements of the vector can be referenced by their index:

There are also **fuzzy vectors**, which are vectors without a defined length. 
These can be tricky to deal with, but we will get to that in the dedicated chapter on [Vectors](chapter-vectors). 

## Table Types
Beyond vectors, Tabitha also has **tables**. 
This is again represented by a functor `Table`. 
This functor can take an arbitrary number of arguments. 
For example,

    Table[Int a, Float b, Float c, 10]

is a type representing a table with three **fields** (the columns) and `10` rows. 
Each field has a type and a name. 
For example, there is a field named `b` whose elements are of type `Float`. 
Each row of a table also has a unique integer ID. 

Tables are the namesake of Tabitha. 
They were the initial inspiration for the language, since I noticed there was no nice way to quickly create column-based tables in C. 
By column-based, I mean that the columns represent contiguous sections of memory.  
These tables may effectively be used as relational models within software, and following the Codd rules offer a promising replacement for the hierarchical object model.

## Alias Types
Using the functors listed above, we can create a rich variety of types. 
However as our types get more complex, our fingers get worn out typing out the \dots Well, types.  
To solve this, Tabitha allows us to attach a more snappy name to types. 
For example,

    alias type String represents Addr[Char]
    
is found in the Tabitha standard slab. 


