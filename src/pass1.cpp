//
// Name: Braydon Johnston REDid: 131049942
// Name: Reinaldo Roldan  REDid: 825760540
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
    
    while (std::getline(file, line)) {
        std::string label, opcode, operand;
        if (!parseLine(line, label, opcode, operand)) {
            continue;
        }
        
        if (firstLine) {
            if (opcode == "START") {
                startAddress = utils::hexToInt(operand);
                locctr = startAddress;
            } else {
                startAddress = 0;
                locctr = 0;
            }
            firstLine = false;
        }
        
        processInstruction(label, opcode, operand, locctr);
    }
    
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
    
    // Split into tokens by whitespace
    std::istringstream iss(trimmed);
    std::vector<std::string> tokens;
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return false;
    }
    
    // Handle extended format (+ prefix)
    bool extendedFormat = false;
    
    // Determine if first token is a label or opcode
    // If there are 2+ tokens, first is likely a label, second is opcode
    // If there's 1 token, it's an opcode
    if (tokens.size() == 1) {
        // Single token - must be opcode
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
        // Two tokens - could be "LABEL OPCODE" or "OPCODE OPERAND"
        // Check if second token starts with + (extended format opcode)
        std::string secondToken = tokens[1];
        if (secondToken[0] == '+' || utils::toUpper(secondToken) == "START" || 
            utils::toUpper(secondToken) == "RESW" || utils::toUpper(secondToken) == "RESB" ||
            utils::toUpper(secondToken) == "BYTE" || utils::toUpper(secondToken) == "WORD" ||
            utils::toUpper(secondToken) == "EQU" || utils::toUpper(secondToken) == "CSECT" ||
            utils::toUpper(secondToken) == "EXTDEF" || utils::toUpper(secondToken) == "EXTREF" ||
            optab.contains(utils::toUpper(secondToken))) {
            // Second token is opcode, first is label
            label = tokens[0];
            if (secondToken[0] == '+') {
                extendedFormat = true;
                opcode = "+" + utils::toUpper(secondToken.substr(1));
            } else {
                opcode = utils::toUpper(secondToken);
            }
            operand = "";
        } else {
            // First token is opcode, second is operand
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
        // Three or more tokens - could be "LABEL OPCODE OPERAND..." or "OPCODE OPERAND OPERAND..."
        // Check if second token is a known opcode/directive
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
            // Combine remaining tokens as operand (preserving commas)
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
            // Combine remaining tokens as operand (preserving commas)
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
    // Handle literal definition (* =C'...' or * =X'...')
    if (label == "*" && !operand.empty() && operand[0] == '=') {
        // This is a literal definition - just update location counter
        if (operand.length() > 1 && operand[1] == 'C') {
            std::string str = operand.substr(3, operand.length() - 4);
            locctr += str.length();
        } else if (operand.length() > 1 && operand[1] == 'X') {
            std::string hex = operand.substr(3, operand.length() - 4);
            locctr += (hex.length() + 1) / 2;
        }
        return; // Don't process as regular instruction
    }
    
    if (!label.empty() && label != "*") {
        if (symtab.contains(label)) {
            std::cerr << "Error: Duplicate symbol " << label << std::endl;
        } else {
            symtab.insert(label, locctr);
        }
    }
    
    // Check for extended format
    bool extendedFormat = (opcode[0] == '+');
    std::string baseOpcode = extendedFormat ? opcode.substr(1) : opcode;
    
    if (baseOpcode == "START" || baseOpcode == "END" || baseOpcode == "BASE" || 
        baseOpcode == "LTORG" || baseOpcode == "EXTDEF" || baseOpcode == "EXTREF" ||
        baseOpcode == "EQU" || baseOpcode == "CSECT") {
        // Directives - do nothing for location counter
        if (baseOpcode == "LTORG") {
            // LTORG processes literals - for now, we'll handle this in pass2
        } else if (baseOpcode == "EQU") {
            // EQU defines a symbol - handle if needed
            if (!label.empty()) {
                // For EQU *, use current locctr
                if (operand == "*") {
                    symtab.insert(label, locctr);
                } else {
                    // For EQU expression, would need expression evaluator
                    // For now, just insert at current location
                    symtab.insert(label, locctr);
                }
            }
        } else if (baseOpcode == "CSECT") {
            // CSECT starts a new control section - reset location counter
            // Keep symbol table for cross-section references (EXTREF/EXTDEF)
            locctr = 0;
        }
    } else if (baseOpcode == "WORD") {
        locctr += 3;
    } else if (baseOpcode == "RESW") {
        int words = utils::stringToInt(operand);
        locctr += 3 * words;
    } else if (baseOpcode == "RESB") {
        int bytes = utils::stringToInt(operand);
        locctr += bytes;
    } else if (baseOpcode == "BYTE") {
        if (!operand.empty() && operand[0] == 'C') {
            int len = operand.length() - 3; // Remove C'...'
            locctr += len;
        } else if (!operand.empty() && operand[0] == 'X') {
            int len = (operand.length() - 3) / 2; // Remove X'...'
            locctr += len;
        }
    } else if (optab.contains(baseOpcode)) {
        int format = optab.getFormat(baseOpcode);
        if (extendedFormat) {
            format = 4; // Extended format is always format 4
        }
        locctr += format;
    } else if (baseOpcode != "*") {
        // * is a special label, not an opcode
        std::cerr << "Error: Invalid opcode " << baseOpcode << std::endl;
    }
}

