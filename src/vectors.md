# Vectors 
## Declaring a Vector
A vector is declared just like any other variable. 
For example,

    Vec[Int, 10] x 
    stacked Vec[Int, 10] y
    heaped Vec[Int, 10] z 

will declare a vector of integers `10` elements long. 
The vector itself is formally an pointer as far as the compiler is concerned. 
This pointer we refer to as the **handle** to the vector, leading us to the location in memory where the elements are stored. 

If a vector is declared as `heaped`, then not only is the handle stored on the heap, but so are the elements. 
By default however, like all variables, the handle and elements are stored on the stack. 

## Dealing With the Elements. 
Elements of a vector are referenced by their index in the vector.  
For example,
    
    Vec[Int, 10] x
    x[3] = 10 

will set element `3` of vector `x` to `10`. 
The *first* element of the vector is indexed by `x[0]`. 
We can use any expression to index a vector element, so long as that expression is of integer type. 
Because the value of this expression can thus only be determined at runtime, the compiler does check whether or not the reference is in bounds (complying with the vector's length). 
Hence it is possible to cause a *segmentation fault* at runtime; a program may query memory which has not been allocated. 
This is an issue with which C programmers are very familiar, however can come as a shock to programmers coming from other languages which do runtime checks.
These checks are skipped over for the sake of performance. 
It is incumbent on the developer to place this checks in their code, if they desire the extra safety. 

## Setting Multiple Elements

Assigning to multiple elements individually by their index can be very tedious. 
Tabitha hence has a syntactic sugar for this. 

    Vec[Int, 10] x
    set vector x from 3 as (23, -5, 11)

The `set vector` statement sets as many elements of the vector,
as their are expressions in the comma-separated tuple `(23, -5, 11)`. 
This assignment starts `from` the element with index `3`. 
Once again, their are no compile-time or runtime checks as to whether we are remaining within the vector bounds. 

## Fuzzy Vectors

Sometimes we don't know in-advance what the length of our vector is going to be. 
The type system accounts for this, by having a special type of vector called a **fuzzy vector**. 
A fuzzy vector is simply a vector with no specified length. 
They are declared with a null argument for a length with the usual `Vec` functor. 
For example, 

    Vec[Int, _] x
    
will declare a fuzzy vector of integers. 
To attach this vector to memory, we use the `label` statement. 
For example, we can write something like,

    Vec[Int, 10] x # just a normal vector
    Vec[Int,  _] y # a fuzzy vector
    label x[0]? as y 
    
which make it so the fuzzy vector `y` represents the data held in `x`. 

I envision that fuzzy vectors will be used most as the arguments to functions which want to take any number of values of a particular type. 
Take this example:

    attach external std

    function sum (Vec[Int, _] x, Int numTerms) -> Int {
        Int s = 0
        Int n = 0
        loop {
            s = s + x[n]
            n = n + 1
        } while n < numTerms
        return s
    }

    function main -> Int {
        Vec[Int, 5] x 
        set vector x from 0 as (1, 2, 3, 4, 5) 
        Vec[Int, _] y
        label x[0]? as y
        Int s = sum(y, 5)
        std::printIntLn(s)
        return 0
    }
    
We pass a fuzzy vector to the `sum` function, letting the function know how many elements are in the vector. 
Note that we could also do it like this:

    attach external std

    function sum (Addr[Int] x, Int numTerms) -> Int {
        Vec[Int, _] y
        label x as y 
        Int s = 0
        Int n = 0
        loop {
            s = s + y[n]
            n = n + 1
        } while n < numTerms
        return s
    }

    function main -> Int {
        Vec[Int, 5] x 
        set vector x from 0 as (1, 2, 3, 4, 5) 
        Int s = sum(x[0]?, 5)
        std::printIntLn(s)
        return 0
    }
    
We pass an address to the function, and the function interprets it as an address the the first element of the vector. 

Unlike in C, addresses do not enjoy a dual existence as simple pointers, and handles for arrays. 
In order to reference elements of an array encoded by and address, the variable must be of vector type. 
We cannot use the index notation on an address. 
Really, fuzzy vectors are there to allow us to treat addresses as vectors, though with an extra step. 


