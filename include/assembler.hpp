#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <string>
#include <vector>
#include "symtab.hpp"
#include "optab.hpp"
#include "listing.hpp"

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
    
    bool pass1(const std::string& inputFile);
    bool pass2(const std::string& inputFile);
    bool writeSymbolTable(const std::string& inputFile);
};

#endif // ASSEMBLER_HPP

