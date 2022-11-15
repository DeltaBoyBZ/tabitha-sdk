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
#include<fstream>
#include<sstream>
#include<stdexcept>
#include<map>

namespace tabic
{
    class CannotReadFile : std::exception
    {
        public:
            std::string filename; 
            CannotReadFile(std::string filename) : filename(filename) { }
            const char* what() const throw()
            {
                return "Failed to read file.";
            }
    }; 

    class Util
    {
        public: 
            static std::map<std::string, bool> flags; 
            static std::map<std::string, void*> options; 
            static std::map<std::string, void*> args; 

            static std::string readFile(std::string filename)
            {
                std::ifstream reader(filename); 
                if(!reader.good()) throw CannotReadFile(filename); 
                std::stringstream contents; 
                contents << reader.rdbuf();
                reader.close();
                return contents.str(); 
            }
            static void writeFile(std::string filename, std::string contents)
            {
                std::ofstream writer(filename);     
                writer << contents; 
                writer.close(); 
            }
    };
}
