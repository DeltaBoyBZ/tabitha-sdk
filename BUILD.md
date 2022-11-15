# Tabitha SDK Build Instructions 
Listed are the instructions to build the Tabitha SDK. 
If there are any errors, please raise them in the Issues section of the [GitHub repository](https://github.com/DeltaBoyBZ/tabitha-sdk).

## System Requirements
As of now The Tabitha SDK is known only to build on Linux. 
The main development machine ran Debian 11 on AMD64 architecture. 

## Getting the Dependencies Independently
The Tabitha SDK has two main dependencies, without which the compiler cannot function. 
You may optionally postpone gettting these, since The Tabitha SDK repo bundles them as git submodules. 
Installing all dependencies from source this way however tends to take longer. 

1. [LLVM](https://llvm.org) - Used for creating intermediate files and ultimately assembling the binaries. 
    LLVM is simple to install on Debian-based systems. 
    Basic testing indicates that LLVM-13 is sufficient. 
    You may choose a later version if you wish. 
    You may need to consult [https://apt.llvm.org](https://apt.llvm.org) for this.
    First add the follwing to your `/etc/apt/sources.list.d` in a file named `llvm.list`:

        deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye main
        deb-src http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye main 
   
    Then run:

        $ sudo apt update
        $ sudo apt install llvm-13-dev
    
    By default, this installs the LLVM headers to `/usr/include/llvm-13` and `/usr/lib/llvm-13/`,
    so make sure these are part of your `CPLUS_INCLUDE_PATH` and `LD_LIBARY_PATH` respectively. 
 
2. [cpp-peglib](https://github.com/yhirose/cpp-peglib) - Used for parsing Tabitha source files. 
    Simply clone the repository: 

        current-dir$ git clone https://github.com/yhirose/cpp-peglib

    Then add the `current-dir` to your `CPLUS_INCLUDE_PATH`. 

## Getting the Source
### Getting The Tabitha SDK Source 
To get the main source for The Tabitha SDK, clone the repository from GitHub. 

    $ git clone https://github.com/DeltaBoyBZ/tabitha-sdk

### (optional) Getting the Dependency Source
By default, this will clone only the files written expressly for the SDK, and none of the dependencies.
If you also want the dependencies, you need to run extra commands. 

    $ git submodule update --init llvm-project
    $ git submodule update --init cpp-peglib
    
These commands should clone specific commits from each respective project to your system. 
To check this has been done correctly, simply check the corresponding subdirectories are nonempty. 

    $ ls llvm-project
    $ ls cpp-peglib
     
## Building from Source
Throughout this section, we assume that you have both [CMake](https://cmake.org) and [Ninja](https://ninja-build.org).

### Create a Build Directory
[It](It) is highly recommended that you keep the build files and the original source separate.
Outside for the project folder, create a new folder which will contain everything we build. 
You can call this whatever you want, but for the sake of demonstration we will give it the obvious name. 
We shall also place this just outside the source root, so `tabitha-sdk` and `tabitha-sdk-build` are siblings in the file tree.

    $ mkdir tabitha-sdk-build
    $ cd tabitha-sdk-build
    
### (optional) Building the Dependencies
If you installed the dependencies via the other method earlier, you can skip this step. 
Otherwise:

1. Building LLVM. 
    This can take a while, which is why I suggested using a package manager as your first resort. 
    These steps work for me, but if they don't work for you, consult [Getting Started with the LLVM System](https://llvm.org/docs/GettingStarted.html).
    Inside `tabitha-sdk-build`, create a new folder. 
    
        $ mkdir llvm-build
        $ cd llvm-build

    We run CMake with some specific arguments: 
    
        $ cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang:lld" ../tabitha-sdk/llvm-project/llvm 

    If this runs successfully, then run the `ninja` command. 

        $ ninja

    You can install everything with:
    
        $ sudo ninja install 
        
    Then you should make sure the header files and libraries are in your `CPLUS_INCLUDE_PATH` and `LD_LIBARY_PATH` respectively. 
    When using this method, they should be found under `/usr/local`. 

2. Setting up `cpp-peglib`. 
    This is a header-only library, with the repository containing some optional toolsets for developers. 
    It hence requires so building, however you should  make sure `tabitha-sdk-build` is on your `CPLUS_INCLUDE_PATH`. 


### Building The Tabitha SDK
Make sure you're in the `tabitha-sdk-build` directory. 
Then run: 

    $ cmake -G Ninja ../tabitha-sdk
    $ ninja
    $ sudo ninja install 
    
Assuming these commands ran successfully, add the following lines to your `~/.bashrc`. 

    export TABI_BIN=/usr/local/bin/tabitha
    export TABI_LIB=/usr/local/lib/tabitha
    export TABI_RES=/usr/local/share/tabitha/res
    export PATH=${PATH}:${TABI_BIN} 
    export LD_LIBARY_PATH=${LD_LIBARY_PATH}:${TABI_LIB}
    
Note that `TABI_LIB` serves other purposes than being searched for linkable libraries, 
so in particular do **not** omit its definition. 

### Testing the SDK
If you have followed the above steps correctly, you should now have a working Tabitha SDK installation. 
You can test this by creating a project somewhere, 

    $ mkdir tabitha-sdk-test
    $ cd tabitha-sdk-test

and creating there a file `test.tabi`: 

    attach external std
    
    function main -> Int {
        std::printLn("Hello Tabitha!")
        return 0 
    }   

Now run:

    $ tabic test.tabi -o bc 
    $ ./test 

You should see printed to your terminal: 

    Hello Tabitha!



