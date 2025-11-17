#include <iostream>
#include <string>
#include <vector>
#include "assembler.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No input files provided." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <input_file1.sic> [input_file2.sic ...]" << std::endl;
        return 1;
    }
    
    std::vector<std::string> inputFiles;
    for (int i = 1; i < argc; i++) {
        inputFiles.push_back(argv[i]);
    }
    
    bool allSuccess = true;
    for (const auto& inputFile : inputFiles) {
        Assembler assembler;
        if (!assembler.assemble(inputFile)) {
            std::cerr << "Assembly failed for: " << inputFile << std::endl;
            allSuccess = false;
        }
    }
    
    return allSuccess ? 0 : 1;
}

