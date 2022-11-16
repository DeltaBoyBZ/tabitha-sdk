# Variable Declaraton 
## Introduction
A **variable declaration** is a statement which describes a variable to be used in some scope. 
In Tabitha, there are three main kinds of variable declaration:

* Stacked Variable Declaration 
* Heaped Variable Declaratoin
* Context/Dump Variable Declaration

The main difference between each type of declaration is scope. 
Stacked variables should only be used within the current *block*. 
Heaped variables can be used by any part of the code which has the address of the variable.  
Context variables should only be used within functions which have captured the relevant contexts. 

## Stacked Declaration
The most straight-forward kind of variable declaration in Tabitha is the **stacked variable declaration**. 

    function foo {
        Int x                   # this is a stacked variable declaration
        stacked Float y = 3.14  # this is also a stacked variable declaration 
    }
    
To declare a stacked variable, we need to be inside a function. 
By default, all variables declared inside a function are stacked, but we can choose to be explicit and use the `stacked` keyword. 
It is optional whether or not we specify an initial value for the variable. 
The **stack** is a piece of memory given to your program by the operating system, and is essentially a linear space in memory. 
The stack can be pushed to and popped from. 
The stack is reset upon returning from a function, 
hence the restricton that stacked variables should really only be used within the block in which they are declared. 

## Heaped Declaration 
Heaped declarations also take place within functions, but there is a difference in the syntax. 

    function foo {
        heaped Int x   = 10  # this is a heaped variable declaration  
        heaped Float y = 10  # this is also a heaped variable declaration  
    }
    
A heaped variable can be used *by name* within the block in which it is declared. 
They can also be used however by any part of the code which has the variable's address. 
Here is an example:

    attach external std 
    
    function foo {
        Addr[Int] x = bar() 
        std::printIntLn(@x)        
    }
    
    function bar -> Addr[Int] {
        heaped Int x = 10 
        return x? 
    }
    
    
The result of compiling and running this program would be that `10` is printed to the console. 
This example is **not** guaranteed to work if we had made `x` a `stacked` variable in `bar`. 
The reason for this, is that heaped variables are allocated on the memory **heap**. 
This is a large chunk of memory provided to the program by the operating system. 
It is less organised than the stack, and is not affected by a function returning. 
Data allocated on the heap persists until it is explicitly deallocated. 
Therefore, so long as we have the memory address, we can access and modify the data. 

Memory can be deallocated (or freed) by an `unheap` statement. 
Since many complex datatypes can find themselves *heaped*, the unheap statement takes a mandatory argument describing the type of data which is to be deallocated. 
For example,

    heaped Int x
    # ... 
    unheap x? as Int
    
    heaped Vec[Int, 10] x
    # ...
    unheap x? as Vec[Int, 10]
    
So we we that unheap statements take an address to some heaped data, and the type of that heaped data. 
We have to use addresses here, for the name of the variable may no longer be in scope. 

## Context/Dump Declaration
Although the usage of these variables differ from eachother, 
the mechanics of declaring context variables and dump variables are very similar. 
Declaring a variable inside a context or a dump is very simple. 

    context Data {
        Int x
        Float y
        std::String msg = "hello"
        Vec[Int, 10] scores
    }
    
    dump Const {
        Float pi = 3.14  
    }

Context and dump variables primarily reside in the data section of the program's binary. 
That means that the variables are effectively a part of the program itself, and are not just created by the code during runtime. 
The exception to this, is what happens with vectors and tables. 
The elements of these are actually stored on the heap, but the handles are in the data section. 

