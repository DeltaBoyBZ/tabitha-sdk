# Functions 
Functions are the bread and butter of most languages. 
Tabitha implements functions in the sense of a procedural language (e.g. `C`). 
They have a privileged existence, and are very distinct from data. 
Functions can take arguments and return values, though are a free to do neither of these things if they so choose. 

## Tabitha Functions
A **Tabitha function** is simply a function defined wholly within Tabitha source code.  
Here is an example: 

    function foo (Int a, Int b) -> Int {
        a > b => { return a }
        b > a => { return b } 
        return a 
    }
    
This function is pure, in that any time you run it with the same arguments, the result will always be the same. 
This should make the Haskell programmers happy. 
Functions in Tabitha *can* have side-effects however. 

    attach external  std
    
    function bar (Int a) { 
        std::printLn("Enter a number: ")
        Int b = std::readInt(10) # the argument `10` is the maximum 
                                 # number of digits to read
        b > a => {
            std::printLn("Greater") 
        }
        b < a => {
            std::printLn("Lesser")
        }
        b == a => {
            std::printLn("SNAP!") 
        }
    } 

Notice that `bar` does not give a return type, and that is because this function does *not* return a value. 
It is not technically a useless function however, since it has statements performing input and output. 

## External Functions 
Not all functions used in a Tabitha program need to have been defined in Tabitha. 
Indeed, Tabitha can call procedures defined in static and dynamic libraries (often written in C). 
This allows us to write Tabitha bindings to useful libraries quite easily. 

Tabitha calls functions defined elsewhere **external functions**, and they are declared like this:

    external function internalName represents 
        externalName(ArgType1, ArgType2, ArgType3, ...) -> RetType
    
Note that no newline character is necessary here, nor does it affect the meaning of the declaration anyway. 
This is just for the sake of formatting.
Tabitha allows differing `internalName` and `externalName`. 
The reason for this, is that often an external function will have a prefix or otherwise extended name to avoid namespace pollution. 
For example, there is a function in OpenGL called `glBufferData`. 
The `gl` prefix is there to distinguish the function from any other that might be called e.g. `bufferData`. 
In Tabitha, functions are referred to according to their home slab, so we can make the name snappier without risking such pollution.  

As with Tabitha functions, giving a return type is optional. 
The argument list is also completely optional. 



