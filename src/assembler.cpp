#include "assembler.hpp"
#include "pass1.hpp"
#include "pass2.hpp"
#include <iostream>

Assembler::Assembler() {
}

Assembler::~Assembler() {
}

bool Assembler::assemble(const std::string& inputFile) {
    // Clear symbol table for new assembly
    symtab.clear();
    
    // Pass 1: Build symbol table
    if (!pass1(inputFile)) {
        return false;
    }
    
    // Pass 2: Generate listing file (no object code)
    if (!pass2(inputFile)) {
        return false;
    }
    
    // Write symbol table file
    if (!writeSymbolTable(inputFile)) {
        std::cerr << "Warning: Could not write symbol table file" << std::endl;
    }
    
    return true;
}

bool Assembler::pass1(const std::string& inputFile) {
    Pass1 pass1Processor(symtab, optab);
    if (!pass1Processor.process(inputFile)) {
        return false;
    }
    startAddress = pass1Processor.getStartAddress();
    programLength = pass1Processor.getProgramLength();
    return true;
}

bool Assembler::pass2(const std::string& inputFile) {
    if (inputFile.empty()) {
        std::cerr << "Error: pass2 called with empty filename" << std::endl;
        return false;
    }
    Pass2 pass2Processor(symtab, optab, listing);
    return pass2Processor.process(inputFile, startAddress, programLength);
}

bool Assembler::writeSymbolTable(const std::string& inputFile) {
    std::string symtabFile = inputFile;
    size_t pos = symtabFile.find_last_of('.');
    if (pos != std::string::npos) {
        symtabFile = symtabFile.substr(0, pos);
    }
    symtabFile += ".st";
    
    return symtab.writeToFile(symtabFile);
}

