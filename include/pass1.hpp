#ifndef PASS1_HPP
#define PASS1_HPP

#include <string>
#include <fstream>
#include "symtab.hpp"
#include "optab.hpp"

class Pass1 {
public:
    Pass1(SymbolTable& symtab, OpTable& optab);
    
    bool process(const std::string& inputFile);
    int getLocationCounter() const { return locctr; }
    int getProgramLength() const { return programLength; }
    int getStartAddress() const { return startAddress; }
    
private:
    SymbolTable& symtab;
    OpTable& optab;
    int locctr;
    int programLength;
    int startAddress;
    
    bool parseLine(const std::string& line, std::string& label, 
                   std::string& opcode, std::string& operand);
    void processInstruction(const std::string& label, const std::string& opcode,
                           const std::string& operand, int& locctr);
};

#endif // PASS1_HPP

