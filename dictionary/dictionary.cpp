#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "dictionary.h"

int loadDict(int argc, char* argv[], char* envp[]) {
    // obtain list of files from data directory
    std::string curdir =  std::__fs::filesystem::current_path();
    std::string path = curdir + "/data";
    std::ofstream result("csvDictionary.csv");

    // loop through each file to convert it
    for (const auto & entry : std::__fs::filesystem::directory_iterator(path))
    {
        std::cout << "Processing: " << entry.path() << std::endl;
        // add each line to summary file
        std::ifstream instream(entry.path());
        if (!instream){
            instream.close();
//            throw std::runtime_error("Could not load book file: " + entry);
        }

        // SAMPLE LINE
        //<P><B>Azymous</B> (<I>a.</I>) Unleavened; unfermented.</P>

        // process line, check if starts with <P>, if not ignore
        std::string line;
        std::string const startswith = "<P>";
        std::string const boldTag = "<B>";
        std::string const endboldTag = "</B>";

        while (std::getline(instream, line)){
//            std::cout << "Line is: " << line << std::endl;
            std::string word;
            std::string definition;
            std::string type;

            if (line.rfind(startswith, 0) == 0){
//                std::cout << " -- Line starts with <P>, looking for <B>" << std::endl;
                // word is between <B></B> tages
                int startPos = line.find(boldTag);
                if (startPos != std::string::npos){
                    startPos = startPos + 3;
                    int endPos = line.find(endboldTag);
                    if (endPos != std::string::npos){
                        word = line.substr(startPos, endPos -6);
//                        std::cout << "-- Word is: " << word << std::endl;
                        result << "\"" << word << "\"" << ",";

                        // get the type
                        std::string const endOfTypeChar = ")";
                        int endOfTypePos = line.find(endOfTypeChar);
                        std::string const startOfTypeChar = "(<I>";
                        int startOfTypeCharPos = line.find(startOfTypeChar);

                        if (startOfTypeCharPos != std::string::npos && endOfTypePos != std::string::npos){
                            // move forward four spaces to move to end of <I>
                            type = line.substr(startOfTypeCharPos + 4, ((endOfTypePos - 9) - startOfTypeCharPos));
//                            std::cout << "-- Type is: " << type << std::endl;
                            result << "\"" << type << "\"" << ",";
                        }
                        else{
                            std::cout << "No type found" << std::endl;
                        }

                        // get the defintion
                        if (endOfTypePos != std::string::npos){
                            // move two forward as space always follows the )
                            endOfTypePos = endOfTypePos + 2;
                            definition = line.substr(endOfTypePos, (line.length() - endOfTypePos -4));
//                            std::cout << "-- Definition is: " << definition << std::endl;
                            result << "\"" << definition << "\"" << "\n";
                        }
                    }
                    else
                        std::cout << " -- Unable to find closing tag for <\\B>" << std::endl;
                }
                else
                    std::cout << " -- Unable to find startPos for <B>" << std::endl;
            }
//            else
//              std::cout << " -- Line doesn't start with <P>" << std::endl;
        }
    }

    // output the summary file
    result.close();

    return 0;

}