# Tables
## Introduction
As the creator of Tabitha, I can tell you that Tabitha's development stemmed from a smaller project, Tabular-C++. 
To read about this project, and maybe get into using it, visit the website [https://deltaboybz.github.io/tabular-cpp](https://deltaboybz.github.io/tabular-cpp). 
At my heart, I am a C programmer, but was frustrated that there was no easy way to implement column-based data tables in C. 
I created this toolset to generate such code as C++ classes, but it all ultimately felt a little clunky (though the toolset does work well). 

I was thus inspired to create Tabitha, which would have a type system allowing for the quick creation of such tables. 
Tabitha would also support important operations on these tables at its very core. 
So here I present to you the results of this effort. 
I must admit that other language features have received more attention at this point, 
however this still stands as one of the solid reasons I think people could benefit from using Tabitha. 

## Declaring a Table
Tables are declared like any other variable, through use of the `Table` functor. 

    Table[Int a, Float b, 10] x     
    stacked Table[Int a, Float b, 10] y
    heaped Table[Int a, Float b, 10] z
    
All of these statements will create tables with two **fields** (that is, columns), and `10` rows.  
Unlike the case with vectors, there is no such thing as a fuzzy table; we must always know the size of our table. 
Table rows have associated with them a hidden flag which indiciates to Tabitha's core libraries whether or not that row is being used. 
When a table is first declared, all rows are marked as unused. 

## Inserting Rows
Before using the table in any other way, we must insert some rows.
We do this with a special statement. 

    Table[Int a, Float b, 10] x
    Int id 
    insert (10, 3.14) into x > id

The `insert` statement should be read as follows:

*Insert the row with ordered elements* `(10, 3.14)` *into table* `x` *, and store the corresponding row's ID into the variable* `id` *.*

If we wish to deal specifically with this data in the future, then we must retain this `id`. 
The ID is distinct from the row's index (which tells us about where the elements are in memory). 
The table will not change if there are no unused rows in the table. 

## Deleting Rows 
Deleting a row effectively marks that row as being unused (and hence we may insert into it).
For example,

    delete row 3 from x 
    
would delete the row with ID `3` from table `x`.  

## Referencing Individual Elements 
We may reference individual elements with the syntax `[table].[field]<[id]>`. 
For example,

    x.b<id> = 2.718
    
will assign `2.718` to the element in field `b` of `x` corresponding to the row with ID `id`. 

## Mesuring a Table
Despite having a fixed number of rows, a table will usually have some rows which remain unused. 
To get the number of rows actually being used, we can use a `measure` statement. 

    measure x > n # stores the number of used rows in table `x`, in the integer `n`. 
    
Note that the `n`  must have been declared before this statement. 
    
## Crunching a Table 
In principle, the used rows of a table could be anywhere. 
A table might have two consequetive used rows, then an unused row, then a bunch of more used rows. 
For operations such as iterating over rows, this is not ideal. 
Therefore we have the `crunch` statement.

    crunch x > id  
    
A `crunch` statement pushes all used rows to the top of the table. 
The `> id` is optional, storing the ID of the topmost row in the integer variable `id`. 
Again, `id` must have been decalred beforehand. 

## Treating Fields as Vectors
There are situations where it is convenient to perform some operation on successive member's of a table's field. 
Since Tabitha's syntax has it so that table elements are referenced by ID, it is useful to memorise the following pattern for when these situations arise. 

    Table[Int a, Float b, 10] table
    # ...
    # do whatever with the table
    # ...
    Vec[Float, _] b_vec
    Int top
    crunch table > top
    label table.b<top>? as b_vec

We can now refer to elements within the field `table.b` as we would elements of a vector,
with the indices representing actual row numbers as opposed to IDs. 
    
