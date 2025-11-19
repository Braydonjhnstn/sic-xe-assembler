//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// pass1.cpp
//

#include "pass1.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

Pass1::Pass1(SymbolTable& symtab, OpTable& optab)
    : symtab(symtab), optab(optab), locctr(0), programLength(0), startAddress(0) {
}

bool Pass1::process(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << inputFile << std::endl;
        return false;
    }
    
    std::string line;
    bool firstLine = true;
    
    // process each line of source file
    while (std::getline(file, line)) {
        std::string label, opcode, operand;
        // skip comment lines and empty lines
        if (!parseLine(line, label, opcode, operand)) {
            continue;
        }
        
        // handle START directive on first line to set initial address
        if (firstLine) {
            if (opcode == "START") {
                // START directive specifies starting address in hex
                startAddress = utils::hexToInt(operand);
                locctr = startAddress;
            } else {
                // no START directive - default to address 0
                startAddress = 0;
                locctr = 0;
            }
            firstLine = false;
        }
        
        // process instruction and update location counter
        processInstruction(label, opcode, operand, locctr);
    }
    
    // calculate program length (difference between final and start address)
    programLength = locctr - startAddress;
    file.close();
    return true;
}

bool Pass1::parseLine(const std::string& line, std::string& label,
                      std::string& opcode, std::string& operand) {
    std::string trimmed = utils::trim(line);
    if (trimmed.empty() || trimmed[0] == '.') {
        return false; // Comment or empty line
    }
    
    // split into tokens by whitespace
    std::istringstream iss(trimmed);
    std::vector<std::string> tokens;
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return false;
    }
    
    // handle extended format (+ prefix)
    bool extendedFormat = false;
    
    // determine if first token is a label or opcode
    // if there are 2+ tokens, first is likely a label, second is opcode
    // if theres 1 token, its an opcode
    if (tokens.size() == 1) {
        // single token - must be opcode
        label = "";
        std::string firstToken = tokens[0];
        if (firstToken[0] == '+') {
            extendedFormat = true;
            opcode = "+" + utils::toUpper(firstToken.substr(1));
        } else {
            opcode = utils::toUpper(firstToken);
        }
        operand = "";
    } else if (tokens.size() == 2) {
        // two tokens - could be "LABEL OPCODE" or "OPCODE OPERAND"
        // check if second token starts with + (extended format opcode)
        std::string secondToken = tokens[1];
        if (secondToken[0] == '+' || utils::toUpper(secondToken) == "START" || 
            utils::toUpper(secondToken) == "RESW" || utils::toUpper(secondToken) == "RESB" ||
            utils::toUpper(secondToken) == "BYTE" || utils::toUpper(secondToken) == "WORD" ||
            utils::toUpper(secondToken) == "EQU" || utils::toUpper(secondToken) == "CSECT" ||
            utils::toUpper(secondToken) == "EXTDEF" || utils::toUpper(secondToken) == "EXTREF" ||
            optab.contains(utils::toUpper(secondToken))) {
            // second token is opcode, first is label
            label = tokens[0];
            if (secondToken[0] == '+') {
                extendedFormat = true;
                opcode = "+" + utils::toUpper(secondToken.substr(1));
            } else {
                opcode = utils::toUpper(secondToken);
            }
            operand = "";
        } else {
            // first token is opcode, second is operand
            label = "";
            std::string firstToken = tokens[0];
            if (firstToken[0] == '+') {
                extendedFormat = true;
                opcode = "+" + utils::toUpper(firstToken.substr(1));
            } else {
                opcode = utils::toUpper(firstToken);
            }
            operand = tokens[1];
        }
    } else {
        // three or more tokens - could be "LABEL OPCODE OPERAND..." or "OPCODE OPERAND OPERAND..."
        // check if second token is a known opcode/directive
        std::string secondToken = tokens[1];
        bool secondIsOpcode = (secondToken[0] == '+' || 
                              utils::toUpper(secondToken) == "START" || 
                              utils::toUpper(secondToken) == "RESW" || 
                              utils::toUpper(secondToken) == "RESB" ||
                              utils::toUpper(secondToken) == "BYTE" || 
                              utils::toUpper(secondToken) == "WORD" ||
                              utils::toUpper(secondToken) == "EQU" || 
                              utils::toUpper(secondToken) == "CSECT" ||
                              utils::toUpper(secondToken) == "EXTDEF" || 
                              utils::toUpper(secondToken) == "EXTREF" ||
                              utils::toUpper(secondToken) == "BASE" ||
                              utils::toUpper(secondToken) == "LTORG" ||
                              utils::toUpper(secondToken) == "END" ||
                              optab.contains(utils::toUpper(secondToken)));
        
        if (secondIsOpcode) {
            // "LABEL OPCODE OPERAND..."
            label = tokens[0];
            if (secondToken[0] == '+') {
                extendedFormat = true;
                opcode = "+" + utils::toUpper(secondToken.substr(1));
            } else {
                opcode = utils::toUpper(secondToken);
            }
            // combine remaining tokens as operand (preserving commas)
            operand = "";
            for (size_t i = 2; i < tokens.size(); i++) {
                if (i > 2) operand += " ";
                operand += tokens[i];
            }
        } else {
            // "OPCODE OPERAND OPERAND..." (no label)
            label = "";
            std::string firstToken = tokens[0];
            if (firstToken[0] == '+') {
                extendedFormat = true;
                opcode = "+" + utils::toUpper(firstToken.substr(1));
            } else {
                opcode = utils::toUpper(firstToken);
            }
            // combine remaining tokens as operand (preserving commas)
            operand = "";
            for (size_t i = 1; i < tokens.size(); i++) {
                if (i > 1) operand += " ";
                operand += tokens[i];
            }
        }
    }
    
    return true;
}

void Pass1::processInstruction(const std::string& label, const std::string& opcode,
                               const std::string& operand, int& locctr) {
    // handle explicit literal definition (* =C'...' or * =X'...')
    // these are processed in pass1 to update location counter
    if (label == "*" && !operand.empty() && operand[0] == '=') {
        // calculate literal size and update location counter
        if (operand.length() > 1 && operand[1] == 'C') {
            // character literal: length equals string length
            std::string str = operand.substr(3, operand.length() - 4);
            locctr += str.length();
        } else if (operand.length() > 1 && operand[1] == 'X') {
            // hex literal: length is half the hex string length
            std::string hex = operand.substr(3, operand.length() - 4);
            locctr += (hex.length() + 1) / 2;
        }
        return; // dont process as regular instruction
    }
    
    // insert label into symbol table if present
    if (!label.empty() && label != "*") {
        if (symtab.contains(label)) {
            std::cerr << "Error: Duplicate symbol " << label << std::endl;
        } else {
            symtab.insert(label, locctr);
        }
    }
    
    // check for extended format instruction (+ prefix)
    bool extendedFormat = (opcode[0] == '+');
    std::string baseOpcode = extendedFormat ? opcode.substr(1) : opcode;
    
    // handle assembler directives
    if (baseOpcode == "START" || baseOpcode == "END" || baseOpcode == "BASE" || 
        baseOpcode == "LTORG" || baseOpcode == "EXTDEF" || baseOpcode == "EXTREF" ||
        baseOpcode == "EQU" || baseOpcode == "CSECT") {
        // directives dont consume memory space (no location counter update)
        if (baseOpcode == "LTORG") {
            // LTORG processes literals - handled in pass2
        } else if (baseOpcode == "EQU") {
            // EQU defines a symbol with a value
            if (!label.empty()) {
                // for EQU *, use current locctr
                if (operand == "*") {
                    symtab.insert(label, locctr);
                } else {
                    // for EQU expression, would need expression evaluator
                    // for now, just insert at current location (TODO: fix this later maybe)
                    // this isnt perfect but it should work for most cases
                    symtab.insert(label, locctr);
                }
            }
        } else if (baseOpcode == "CSECT") {
            // CSECT starts a new control section - reset location counter to 0
            // keep symbol table for cross-section references (EXTREF/EXTDEF)
            locctr = 0;
        }
    } else if (baseOpcode == "WORD") {
        // WORD directive: 3 bytes per word
        locctr += 3;
    } else if (baseOpcode == "RESW") {
        // RESW reserves words: 3 bytes per word
        int words = utils::stringToInt(operand);
        locctr += 3 * words;
    } else if (baseOpcode == "RESB") {
        // RESB reserves bytes: 1 byte per byte
        int bytes = utils::stringToInt(operand);
        locctr += bytes;
    } else if (baseOpcode == "BYTE") {
        // BYTE directive: size depends on type
        if (!operand.empty() && operand[0] == 'C') {
            // character constant: 1 byte per character
            int len = operand.length() - 3; // remove C'...'
            locctr += len;
        } else if (!operand.empty() && operand[0] == 'X') {
            // hex constant: 1 byte per 2 hex digits
            int len = (operand.length() - 3) / 2; // remove X'...'
            locctr += len;
        }
    } else if (optab.contains(baseOpcode)) {
        // instruction opcode - get format and update location counter
        int format = optab.getFormat(baseOpcode);
        if (extendedFormat) {
            format = 4; // extended format is always format 4
        }
        locctr += format;
    } else if (baseOpcode != "*") {
        // * is a special label for literals, not an opcode
        // this shouldnt happen often but good to catch it
        std::cerr << "Error: Invalid opcode " << baseOpcode << std::endl;
    }
}

