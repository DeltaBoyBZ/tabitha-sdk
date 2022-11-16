# Getting Started

## Downloading the SDK 

## Setting Up the Development Environment 

## Writing your First Program 
It is traditional for the first application written in any language to be one which simplye prints, 
*"Hello World!"* to the console. 
We see no reason the break this tradition. 
Create a file `hello.tabi`, in a new folder `tabi-hello` (call these anything you like but these are the names we are using in the text): 

    attach external std
    
    function main -> Int {
        std::printLn("Hello World!")
        return 0 
    }
    
This program can be compiled with the command: 

    $ tabic hello.tabi -o bc 
    
The folder `tabi-hello` should now look something like this: 

    tabi-hello
    |-- bc
    |-- |-- EXTERNAL_std.bc
    |-- |-- EXTERNAL_std.o
    |-- |-- LOCAL_hello.bc
    |-- |-- LOCAL_hello.o
    |-- hello
    |-- hello.tabi

We of course have our original source file `hello.tabi`, but there is also now an executable `hello`. 
Furthermore, there is a new folder `bc` (named by the `-o` flag), which contains some intermediate binary files. 
Running the executeable, you should see something like this: 

    $ ./hello
    Hello World!

And there we have it, our first Tabitha program! 
It's not very exciting, but you will soon be able to write a whole lot more with the language. 


