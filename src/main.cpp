//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// main.cpp
//

#include <iostream>
#include <string>
#include <vector>
#include "assembler.hpp"

int main(int argc, char* argv[]) {
    // check for command line arguments
    if (argc < 2) {
        std::cerr << "Error: No input files provided." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <input_file1.sic> [input_file2.sic ...]" << std::endl;
        return 1;
    }
    
    // collect all input file names from command line
    std::vector<std::string> inputFiles;
    for (int i = 1; i < argc; i++) {
        inputFiles.push_back(argv[i]);
    }
    
    // process each input file
    bool allSuccess = true;
    for (const auto& inputFile : inputFiles) {
        try {
            // create assembler instance and process file
            Assembler assembler;
            if (!assembler.assemble(inputFile)) {
                std::cerr << "Assembly failed for: " << inputFile << std::endl;
                allSuccess = false;
            }
        } catch (const std::exception& e) {
            // handle standard exceptions
            std::cerr << "Exception during assembly: " << e.what() << std::endl;
            allSuccess = false;
        } catch (...) {
            // handle any other exceptions
            std::cerr << "Unknown exception during assembly" << std::endl;
            allSuccess = false;
        }
    }
    
    // force cleanup before exit (ensure all output is flushed)
    std::cout.flush();
    std::cerr.flush();
    
    // return 0 if all files assembled successfully, 1 otherwise
    return allSuccess ? 0 : 1;
}

