//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// pass2.hpp
//

#ifndef PASS2_HPP
#define PASS2_HPP

#include <string>
#include <fstream>
#include <map>
#include "symtab.hpp"
#include "optab.hpp"
#include "listing.hpp"

// Second pass: generates object code and listing file
class Pass2 {
public:
    Pass2(SymbolTable& symtab, OpTable& optab, ListingGenerator& listing);
    
    bool process(const std::string& inputFile, int startAddress, int programLength);
    std::vector<Literal> getLiterals() const;
    
private:
    SymbolTable& symtab;
    OpTable& optab;
    ListingGenerator& listing;
    std::map<std::string, int> literalTable; // Maps literal string to address
    
    bool parseLine(const std::string& line, std::string& label,
                   std::string& opcode, std::string& operand);
    std::string generateObjectCode(const std::string& opcode,
                                   const std::string& operand, int locctr);
    void processLiteralPool(int& locctr);
    std::string generateLiteralValue(const std::string& literal) const;
};

#endif // PASS2_HPP

