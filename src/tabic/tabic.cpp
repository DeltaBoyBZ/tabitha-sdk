/*
Copyright 2022 Matthew Peter Smith

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include"tabic/util.hpp"
#include"tabic/create.hpp"
#include"tabic/parse.hpp"
#include"tabic/build.hpp"
#include"tabic/write.hpp"

#include<iostream>
#include<vector>

#ifdef WINDOWS
#include<windows.h> 
#include<direct.h>
#else
#include<unistd.h>
#endif

static std::string TABI_LIB = std::getenv("TABI_LIB");
static std::vector<std::string> _libPaths = {}; 

static char CWD[256]; 
static const char* HELP_TEXT =
"tabic, the Tabitha compiler\n\
Copyright (c) Matthew Smith.\n\
\n\
tabic compiles Tabitha bundles into executables.\n\
\n\
Syntax: tabic [flags] [options] [args]\n\
\n\
args:\n\
root slab filename:  the name of the slab containing the `main` function\n\
\n\
flags:\n\
-show-peg-ast: show the initial AST as produced by cpp-peglib\n\
-show-ir: show the LLVM IR produced for each slab\n\
\n\
options:\n\
-o: directory in which to place the output\n\
-l: libraries to link with the executable\n\
-L: extra directories in which to search for libraries\n\
";

/** @brief Entry point for the Tabitha compiler.
 *
 * Expected arguments:
 * (positional) root slab filename
 * -o output executable filename
 * -l libraries to link
 * -L library directories (in addition to those in LD_LIBRARY_PATH)
 *
 *  @param argc Argument count. 
 *  @param argv Argument array. 
 */
int main(int argc, const char** argv)
{
    //get all _libPaths from TABI_LIB
    int k = 0;
    int k0 = 0; 
#ifdef WINDOWS
    while((k = TABI_LIB.find(";", k0)) != std::string::npos)
    {
        _libPaths.push_back(TABI_LIB.substr(k0, k - k0));
        k0 = k+1; 
    }     
#else
    while((k = TABI_LIB.find(":", k0)) != std::string::npos)
    {
        _libPaths.push_back(TABI_LIB.substr(k0, k - k0));
        k0 = k+1; 
    }     
#endif
    _libPaths.push_back(TABI_LIB.substr(k0, TABI_LIB.length() - k0)); 
#ifdef WINDOWS
    _getcwd(CWD, 256);
#else
    getcwd(CWD, 256);
#endif
    //Parse command line arguments
    std::string rootSlabFilename     = "";
    std::string outputDirectory      = std::string(CWD); 
    std::vector<std::string> linkDirectories = {}; 
    std::vector<std::string> linkLibraries   = {}; 
    std::vector<std::string> staticLibraries   = {}; 
    bool showAST    = false; 
    bool showIR     = false; 
    bool showHelp   = false; 
    bool rawBuild   = false; 
    bool cStart = false;
    {
        int cursor = 1;
        while(cursor < argc)
        {
            std::string arg(argv[cursor]);
            if(arg == "-o")
            {
                cursor++;
                outputDirectory += std::string("/") + argv[cursor];
            }
            else if(arg == "-l")
            {
                cursor++;
                linkLibraries.push_back(argv[cursor]);
            }
            else if(arg == "-ls")
            {
                cursor++;
                staticLibraries.push_back(argv[cursor]); 
            }
            else if(arg == "-L")
            {
                cursor++;
                linkDirectories.push_back(argv[cursor]);
            }
            else if(arg == "-h" || arg == "--help")
            {
                showHelp = true; 
            }
            else if(arg == "--show-ast")
            {
                showAST = true; 
            }
            else if(arg == "--show-ir")
            {
                showIR = true;
            }
            else if(arg == "--raw")
            {
                rawBuild = true; 
            }
            else if(arg == "--c-start")
            {
                cStart = true; 
            }
            else
            {
                rootSlabFilename = arg;
            }
            cursor++;
        }
    }
    if(showHelp)
    {
        std::cout << HELP_TEXT << std::endl; 
        return 0; 
    }
    tabic::Util::flags["show-ast"]  = showAST; 
    tabic::Util::flags["show-ir"]   = showIR; 
    tabic::Util::flags["c-start"] = cStart;
    tabic::Util::options["o"]       = new std::string(outputDirectory); 
    tabic::Util::args["rootSlabFilename"] = new std::string(rootSlabFilename); 

    tabic::Bundle* bundle;
    tabic::CreateStatus createStatus = tabic::createBundle(rootSlabFilename, CWD, &bundle); 
    if(createStatus == tabic::CREATE_STATUS_FAIL) return 1;
    tabic::ParseStatus parseStatus = tabic::parseBundle(bundle); 
    if(parseStatus == tabic::PARSE_STATUS_FAIL) return 2; 
    tabic::buildBundle(bundle);
    tabic::writeBundle(bundle);
    
    //Now link everything
    std::string linkCommand; 
    if(rawBuild)
    {
        linkCommand = "ld.lld " + outputDirectory + "/*.o -o " + bundle->create.rootSlab->create.name + " -ltabi_std_raw -ltabi_core_raw -e_tabi_start ";
    }
    else
    {
#ifdef WINDOWS
        linkCommand = "gcc " + outputDirectory + "/*.o -o " + bundle->create.rootSlab->create.name + " -ltabi_std_cross -ltabi_core_cross ";
#else
        linkCommand = "clang -no-pie " + outputDirectory + "/*.o -o " + bundle->create.rootSlab->create.name + " -lm -ltabi_std_cross -ltabi_core_cross ";
#endif
    }

    for(std::string &dir : _libPaths)
    {
        linkCommand += "-L\"" + dir + "\" ";
    }
    for(std::string &dir : linkDirectories)
    {
        linkCommand += "-L\"" + dir + "\" ";
    }
    for(std::string &lib : linkLibraries)
    {
        linkCommand += "-l" + lib + " "; 
    }
    for(std::string &lib : staticLibraries)
    {
        linkCommand += "-l:lib" + lib + ".a "; 
    }
    std::system(linkCommand.c_str());
    return 0;
}
