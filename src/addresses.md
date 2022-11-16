# Addresses
## Introduction 
In Tabitha, an address is what a C programmer might call a *pointer*. 
It is a value which identifies a location in memory where data is stored. 
Addresses are very useful for developing performant applications. 

## Declaring an Address
Since they have their own types, addresses can be declared just like any other variable. 

    Addr[Int] x
    stacked Addr[Int] y
    heaped Addr[Int] z
    
The type `Addr[Int]` is one which belongs to variables representing the memory address of some integer. 
Similarly, `Addr[Float]` is for the address of a `Float`. 
We can create addresses for any type, including other addresses. 

    Addr[Addr[Int]] x
    
Here, `x` would hold the memory address to data which corresponds to another memory address. 

## Getting an Addresss
Given a variable or subvalue thereof, we can get its address in memory using the **locate** operator `?`. 

    Int x = 10
    Addr[Int] x_addr = x? 
    
The variable `x_addr` should not hold the memory address of the variable `x`. 
We can do this with any referencable value in Tabitha. 
For example, we can do it with vector elements:

    Vec[Int, 10] x
    Addr[Int] elem_ptr = x[3]?
    
The variable `elem_ptr` holds the memory address where we find the element of index `3` of vector `x`.
This also works with members of collection types. 

    collection type Point {
        Float x
        Float y
    }
    
    function foo {
        Point p 
        p.x = 10
        Addr[Float] px_ptr = p.x? 
    }
    
The variable `px_ptr` holds the memory address where we can find the value `p.x`, which happens to be `10`. 

## Querying an Address
Given some address, we can get the value found at that address using the **query operator** `@`. 

    Int a = 10
    Int b = 20
    Addr[Int] a_ptr = a?
    Addr[Int] b_ptr = b?
    Int c = @a_ptr + @b_ptr
    
Here, we have taken some extra steps in summing two variables, for the sake of demonstration. 
We have two variables `a` and `b`. 
We get their memory addresses and store them in `a_ptr` and `b_ptr` respectively. 
We then create a new variable `c` and assign to it the sum of the integer values found at `a_ptr` and `b_ptr`. 
Now `c` should have the value `30`. 

It should be noted that, as of yet, Tabitha does **not** support address arithmetic. 
That is, it does not allow one to add addresses together, or at numbers to addresses. 
This may be changed in the future. 

