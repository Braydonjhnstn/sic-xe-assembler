//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// assembler.hpp
//

#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <string>
#include <vector>
#include "symtab.hpp"
#include "optab.hpp"
#include "listing.hpp"

// Main orchestrator for SIC/XE two-pass assembler
class Assembler {
public:
    Assembler();
    ~Assembler();
    
    bool assemble(const std::string& inputFile);
    
private:
    SymbolTable symtab;
    OpTable optab;
    ListingGenerator listing;
    int startAddress;
    int programLength;
    std::vector<Literal> pass2Literals;
    
    bool pass1(const std::string& inputFile);
    bool pass2(const std::string& inputFile);
    bool writeSymbolTable(const std::string& inputFile);
};

#endif // ASSEMBLER_HPP

