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
#include "tabic/write.hpp"
#include "tabic/util.hpp"

#include "llvm/Support/FileSystem.h"
#include "llvm/Bitcode/BitcodeWriter.h"

#include"llvm/Target/TargetMachine.h"
#include"llvm/Target/TargetOptions.h"
#include"llvm/Support/TargetParser.h"
#include"llvm/Support/TargetSelect.h"
#include"llvm/Support/Host.h"
#include"llvm/IR/PassManager.h"
#include"llvm/IR/LegacyPassManager.h"
#include"llvm/MC/TargetRegistry.h"

#include"llvm/IR/Verifier.h"
#include"llvm/Support/ToolOutputFile.h"
#include"llvm/IR/AssemblyAnnotationWriter.h"

#include<sys/types.h>
#include<sys/stat.h>

#define WINDOWS 

#ifdef WINDOWS
#include<windows.h> 
#else
#include<unistd.h>
#endif

void tabic::writeBundle(Bundle* bundle)
{
    for(auto pair : bundle->create.slabs)
    {
        Slab* slab = pair.second;
        writeSlab(slab);
    }
}

void tabic::writeSlab(Slab* slab)
{
    if(Util::flags["show-ir"])
    {
        std::string ir; 
        llvm::raw_string_ostream OS(ir); 
        slab->build.llvmModule->print(OS, new llvm::AssemblyAnnotationWriter);
        std::cout << ir << std::endl; 
    } 
    
    //verify IR
    llvm::verifyModule(*slab->build.llvmModule);

    std::string bcFilename  = slab->create.id + ".bc";
    std::string objFilename = slab->create.id + ".o";
    for(int i = 0; i < bcFilename.length(); i++)
    {
        if(bcFilename[i] == '/') 
        {
            bcFilename[i] = '_';
            objFilename[i] = '_'; 
        }
    }

    std::string outputDirectory = *(std::string*)Util::options["o"];  

    struct stat st = {0};
#ifdef WINDOWS
    CreateDirectory((LPCSTR)outputDirectory.c_str(), NULL);
#else
    if(stat(outputDirectory.c_str(), &st) == -1)
    {
        mkdir(outputDirectory.c_str(), 0700); 
    }
#endif

    std::error_code EC;
    llvm::raw_fd_ostream OS(outputDirectory + "/" + bcFilename, EC, llvm::sys::fs::OpenFlags::OF_None);
    llvm::WriteBitcodeToFile(*slab->build.llvmModule, OS);
    OS.flush(); 


    //emitting object code
    auto target_triple = llvm::sys::getDefaultTargetTriple();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    std::string err;
    auto target = llvm::TargetRegistry::lookupTarget(target_triple, err);
    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt; 
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    RM = llvm::Reloc::Model::PIC_;
    auto target_machine = target->createTargetMachine(target_triple, CPU, Features, opt, RM);
    slab->build.llvmModule->setDataLayout(target_machine->createDataLayout());
    slab->build.llvmModule->setTargetTriple(target_triple);
    llvm::raw_fd_ostream objOut(outputDirectory + "/" + objFilename, EC, llvm::sys::fs::OF_Text);
    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;
    target_machine->addPassesToEmitFile(pass, objOut, nullptr, FileType); 
    pass.run(*slab->build.llvmModule);
    objOut.flush();
}

