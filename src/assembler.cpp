//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// assembler.cpp
//

#include "assembler.hpp"
#include "pass1.hpp"
#include "pass2.hpp"
#include "symtab.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

Assembler::Assembler() {
}

Assembler::~Assembler() {
}

bool Assembler::assemble(const std::string& inputFile) {
    // clear symbol table for new assembly
    symtab.clear();
    
    // pass 1: build symbol table
    if (!pass1(inputFile)) {
        return false;
    }
    
    // pass 2: generate listing file
    if (!pass2(inputFile)) {
        return false;
    }
    
    // write symbol table file
    try {
        if (!writeSymbolTable(inputFile)) {
            std::cerr << "Warning: Could not write symbol table file" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Exception writing symbol table: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Warning: Unknown error writing symbol table" << std::endl;
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
    bool result = pass2Processor.process(inputFile, startAddress, programLength);
    
    // store literals for symbol table output (call before pass2Processor is destroyed)
    // clear first to avoid any issues
    pass2Literals.clear();
    
    // only get literals if processing was successful
    if (result) {
        // get literals in a safe way
        try {
            const std::vector<Literal>& literals = pass2Processor.getLiterals();
            // use reserve and push_back to avoid potential copy issues
            pass2Literals.reserve(literals.size());
            for (const auto& lit : literals) {
                pass2Literals.push_back(lit);
            }
        } catch (const std::bad_alloc& e) {
            // memory allocation error
            pass2Literals.clear();
        } catch (const std::exception& e) {
            // other exception
            pass2Literals.clear();
        } catch (...) {
            // catch any other exception
            pass2Literals.clear();
        }
    }
    
    return result;
}

bool Assembler::writeSymbolTable(const std::string& inputFile) {
    // generate symbol table filename by replacing .sic extension with .st
    std::string symtabFile = inputFile;
    size_t pos = symtabFile.find_last_of('.');
    if (pos != std::string::npos) {
        symtabFile = symtabFile.substr(0, pos);
    }
    symtabFile += ".st";
    
    // get program name from START directive in source file
    // default to "PROG" if no START directive found
    std::ifstream file(inputFile);
    std::string programName = "PROG";
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // go through whitespace to find first non-comment line
            std::string trimmed = line;
            while (!trimmed.empty() && (trimmed[0] == ' ' || trimmed[0] == '\t')) {
                trimmed = trimmed.substr(1);
            }
            // skip comment lines (starting with .)
            if (!trimmed.empty() && trimmed[0] != '.') {
                std::istringstream iss(trimmed);
                std::string label, opcode;
                iss >> label >> opcode;
                // found START directive - use label as program name
                if (opcode == "START") {
                    programName = label.empty() ? "PROG" : label;
                    break;
                }
            }
        }
        file.close();
    }
    
    // write symbol table with program info and literals
    return symtab.writeToFile(symtabFile, programName, programLength, pass2Literals);
}

