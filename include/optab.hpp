//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// optab.hpp
//

#ifndef OPTAB_HPP
#define OPTAB_HPP

#include <string>
#include <unordered_map>

// Opcode table for SIC/XE instruction lookup
class OpTable {
public:
    OpTable();
    ~OpTable();
    
    bool lookup(const std::string& opcode, int& opcodeValue) const;
    bool contains(const std::string& opcode) const;
    int getFormat(const std::string& opcode) const; // Returns 2, 3, or 0 (4 determined by + prefix)
    
private:
    std::unordered_map<std::string, int> opcodes;
    std::unordered_map<std::string, int> formats;
    void initializeOpcodes();
};

#endif // OPTAB_HPP

