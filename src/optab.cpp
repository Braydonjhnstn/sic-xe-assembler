//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
//

#include "optab.hpp"
#include <map>

OpTable::OpTable() {
    initializeOpcodes();
}

OpTable::~OpTable() {
}

void OpTable::initializeOpcodes() {
    // SIC/XE instruction set (format 2/3/4)
    // Format 3 instructions
    opcodes["ADD"] = 0x18;
    opcodes["AND"] = 0x40;
    opcodes["COMP"] = 0x28;
    opcodes["DIV"] = 0x24;
    opcodes["J"] = 0x3C;
    opcodes["JEQ"] = 0x30;
    opcodes["JGT"] = 0x34;
    opcodes["JLT"] = 0x38;
    opcodes["JSUB"] = 0x48;
    opcodes["LDA"] = 0x00;
    opcodes["LDCH"] = 0x50;
    opcodes["LDL"] = 0x08;
    opcodes["LDX"] = 0x04;
    opcodes["LDB"] = 0x68;
    opcodes["LDT"] = 0x74;
    opcodes["MUL"] = 0x20;
    opcodes["OR"] = 0x44;
    opcodes["RD"] = 0xD8;
    opcodes["RSUB"] = 0x4C;
    opcodes["STA"] = 0x0C;
    opcodes["STCH"] = 0x54;
    opcodes["STL"] = 0x14;
    opcodes["STX"] = 0x10;
    opcodes["SUB"] = 0x1C;
    opcodes["TD"] = 0xE0;
    opcodes["TIX"] = 0x2C;
    opcodes["WD"] = 0xDC;
    
    // Format 2 instructions
    opcodes["CLEAR"] = 0xB4;
    opcodes["COMPR"] = 0xA0;
    opcodes["TIXR"] = 0xB8;
    
    // Format for each instruction (2 = format 2, 3 = format 3, 4 = format 4)
    formats["ADD"] = 3;
    formats["AND"] = 3;
    formats["COMP"] = 3;
    formats["DIV"] = 3;
    formats["J"] = 3;
    formats["JEQ"] = 3;
    formats["JGT"] = 3;
    formats["JLT"] = 3;
    formats["JSUB"] = 3;
    formats["LDA"] = 3;
    formats["LDCH"] = 3;
    formats["LDL"] = 3;
    formats["LDX"] = 3;
    formats["LDB"] = 3;
    formats["LDT"] = 3;
    formats["MUL"] = 3;
    formats["OR"] = 3;
    formats["RD"] = 3;
    formats["RSUB"] = 3;
    formats["STA"] = 3;
    formats["STCH"] = 3;
    formats["STL"] = 3;
    formats["STX"] = 3;
    formats["SUB"] = 3;
    formats["TD"] = 3;
    formats["TIX"] = 3;
    formats["WD"] = 3;
    formats["CLEAR"] = 2;
    formats["COMPR"] = 2;
    formats["TIXR"] = 2;
}

bool OpTable::lookup(const std::string& opcode, int& opcodeValue) const {
    auto it = opcodes.find(opcode);
    if (it != opcodes.end()) {
        opcodeValue = it->second;
        return true;
    }
    return false;
}

bool OpTable::contains(const std::string& opcode) const {
    return opcodes.find(opcode) != opcodes.end();
}

int OpTable::getFormat(const std::string& opcode) const {
    auto it = formats.find(opcode);
    if (it != formats.end()) {
        return it->second;
    }
    return 0;
}

