# Contexts and Dumps
## Introduction
Along with the table type, contexts are a flagship feature of Tabitha. 
It is through contexts, that Tabitha seeks to make the use of global data tenable in a large program. 
The basic idea, is that global data is store in contexts, 
and any function that wishes to use this data must capture the context. 
This way, any bugs relating to a specific piece of data can be easily tracked down to a subset of all functions in the source code. 

Contexts also remove the need for creating types which are only used once (think context types from C). 
Furthermore, this global data need not be passed around awkwardly from one procedure to another, 
but rather any procedure that needs the data can have it. 
While this level of global data access is often seen as an issue, 
it is circumvented by the requirement that functions must explicitly show that they can access this data via the capturing syntax. 

A **dump** is similar to a context, except there are not as many restrictions as to where its members can be used. 
These are envisioned to be convenient stores for constants, especially when writing API bindings. 

## Creating a Context
Here is an example of a context: 

    context Data {
        Int x
        Float y
        std::String msg
    }
    
All a context is, is a static collection of data. 
There is only one copy of this dataset in the entire program;  contexts cannot be instanced. 

## Capturing and Using a Context
For a function to read or modify data in a context, it must first capture said context. 
Here is an example: 

    function foo <Data> {
            Data/x = 69
            Data/msg = "Hello there"
            std::printFloatLn(Data/y)
    }
    
A function may capture as many contexts as it pleases. 
Contexts are referenced by their name, and if they belong to an attached slab, 
then the slab name is a prefix. 
For example, `bar::Data` would refer to the context `Data` in slab `bar`. 

The members of a constext are referenced by a forward slash `/`.
Members can be referenced for the sake of reading or writing. 


## Creating and Using Dumps 
The syntax for definingg dumps is very similar to that of defining a context. 

    dump Const { 
        Int x = 69 
        Float y = 3.14
        std::String msg = "hello there" 
    } 
    
Functions need not capture a dump in order to utilise it. 
So for example,

    function foo {
        std::printIntLn(Const/x)
    }
    
is perfectly legal. 

## Getting Around the Rules
It is technically possible to use the members of a context without capturing the context. 
The means by which we do this is a little convoluted, and shall be considered bad practice. 
The compiler won't stop you, but Tabitha will be very upset with you personally. 
Here is an example, just so you **don't** do this:

    context Data {
        Int x = 0 
    }
    
    function foo <Data> {
        std::printIntLn(Data/x)
        foo(Data/x?)
        std::printIntLn(Data/x)
    }
    
    function bar(Addr[Int] x) {
        @x = @x + 1 
    }
    
Although `bar` has not captured the context `Data`, it may modify `Data/x`, for it has its address. 
It is necessary to keep this mechanism for e.g. to be able to pass these addresses to external API functions. 
Within Tabitha itself however, it is advised that we keep this mode of usage to a minimum. 


