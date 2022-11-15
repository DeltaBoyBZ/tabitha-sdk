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
#include<iostream>
#include<tabic/util.hpp>
#include<tabic/pre.hpp>


#ifndef PREPROCESSOR_DELIMITER
#define PREPROCESSOR_DELIMITER std::string("##")
#endif

std::string tabic::preprocess(std::string src)
{
    int opener = 0; 
    int closer = 0; 
    while((opener = src.find(PREPROCESSOR_DELIMITER, closer)) != std::string::npos)
    {
        if(closer == std::string::npos) 
        {
            std::cerr << "Bad preprocessor block" << std::endl;
            break; 
        }
        int commandEnd = src.find(PREPROCESSOR_DELIMITER, opener + PREPROCESSOR_DELIMITER.length()); 
        closer = src.find(PREPROCESSOR_DELIMITER, commandEnd + PREPROCESSOR_DELIMITER.length());  
        std::string command = src.substr(opener + PREPROCESSOR_DELIMITER.length(), commandEnd - opener - PREPROCESSOR_DELIMITER.length()); 
        std::string blockSrc = src.substr(commandEnd + PREPROCESSOR_DELIMITER.length(), closer - commandEnd - 1);  
        Util::writeFile("tabic_pre.src", blockSrc); 
        std::system(command.c_str());  
        std::string blockDst  = Util::readFile("tabic_pre.dst"); 
        src = src.replace(opener, closer + 2 - opener, blockDst); 
    }
    return src; 
}


