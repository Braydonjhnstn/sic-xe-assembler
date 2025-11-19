//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
//

#include "pass2.hpp"
#include "utils.hpp"
#include "symtab.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

Pass2::Pass2(SymbolTable& symtab, OpTable& optab, ListingGenerator& listing)
    : symtab(symtab), optab(optab), listing(listing) {
    literalTable.clear();
}

bool Pass2::process(const std::string& inputFile, int startAddress, int programLength) {
    if (inputFile.empty()) {
        std::cerr << "Error: Empty filename" << std::endl;
        return false;
    }
    
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file: " << inputFile << std::endl;
        return false;
    }
    
    // Generate listing file with .l extension
    std::string listingFile = inputFile;
    size_t pos = listingFile.find_last_of('.');
    if (pos != std::string::npos) {
        listingFile = listingFile.substr(0, pos);
    }
    listingFile += ".l";
    
    if (!listing.open(listingFile)) {
        std::cerr << "Warning: Cannot create listing file " << listingFile << std::endl;
    }
    
    std::string line;
    int locctr = startAddress;
    bool firstLine = true;
    std::string programName = "";
    bool inControlSection = false;
    
    while (std::getline(inFile, line)) {
        std::string originalLine = line;
        std::string label, opcode, operand;
        
        if (!parseLine(line, label, opcode, operand)) {
            std::string trimmed = utils::trim(line);
            if (!trimmed.empty() && trimmed[0] == '.') {
                listing.writeLine(-1, "", originalLine); // -1 means no address for comments
            }
            continue;
        }
        
        if (firstLine && opcode == "START") {
            programName = label.empty() ? "PROG" : label;
            // Write START line with address
            listing.writeLine(startAddress, "", originalLine);
            firstLine = false;
            continue;
        }
        
        if (opcode == "END") {
            // Write END line with no address
            listing.writeLine(-1, "", originalLine); // -1 means no address
            // Don't break - continue processing other control sections
            continue;
        }
        
        // Check for directives that need special handling before normal processing
        bool extendedFormat = (opcode[0] == '+');
        std::string baseOpcode = extendedFormat ? opcode.substr(1) : opcode;
        
        if (baseOpcode == "CSECT") {
            // Process any remaining literals from previous section before starting new one
            if (inControlSection && !literalTable.empty()) {
                processLiteralPool(locctr);
            }
            locctr = 0; // Reset for new control section
            literalTable.clear(); // Clear literal table for new section
            inControlSection = true;
            // Write CSECT line with address 0
            listing.writeLine(0, "", originalLine);
            continue;
        } else if (baseOpcode == "LTORG") {
            // Write LTORG directive (but don't show address for LTORG line itself in some cases)
            listing.writeLine(locctr, "", originalLine);
            // Process literal pool
            processLiteralPool(locctr);
            continue;
        }
        
        // Check if this is a literal definition line (* =C'...' or * =X'...')
        if (label == "*" && !operand.empty() && operand[0] == '=') {
            // This is an explicit literal definition
            // Add to literal table if not present
            if (literalTable.find(operand) == literalTable.end()) {
                literalTable[operand] = locctr;
            } else {
                literalTable[operand] = locctr; // Update address
            }
            // Write the literal definition
            std::string literalValue = generateLiteralValue(operand);
            listing.writeLine(locctr, literalValue, originalLine);
            // Update location counter
            if (operand.length() > 1 && operand[1] == 'C') {
                std::string str = operand.substr(3, operand.length() - 4);
                locctr += str.length();
            } else if (operand.length() > 1 && operand[1] == 'X') {
                std::string hex = operand.substr(3, operand.length() - 4);
                locctr += (hex.length() + 1) / 2;
            }
            continue;
        }
        
        std::string objectCode = generateObjectCode(opcode, operand, locctr);
        listing.writeLine(locctr, objectCode, originalLine);
        
        // Update location counter (same logic as pass1)
        if (baseOpcode == "WORD") {
            locctr += 3;
        } else if (baseOpcode == "RESW") {
            int words = utils::stringToInt(operand);
            locctr += 3 * words;
        } else if (baseOpcode == "RESB") {
            int bytes = utils::stringToInt(operand);
            locctr += bytes;
        } else if (baseOpcode == "BYTE") {
            if (!operand.empty() && operand[0] == 'C') {
                int len = operand.length() - 3;
                locctr += len;
            } else if (!operand.empty() && operand[0] == 'X') {
                int len = (operand.length() - 3) / 2;
                locctr += len;
            }
        } else if (baseOpcode == "BASE" || baseOpcode == "END" ||
                   baseOpcode == "EXTDEF" || baseOpcode == "EXTREF" || baseOpcode == "EQU") {
            // Directives - no location counter update
        } else if (optab.contains(baseOpcode)) {
            int format = optab.getFormat(baseOpcode);
            if (extendedFormat) {
                format = 4; // Extended format is always format 4
            }
            locctr += format;
        }
    }
    
    // Process any remaining literals at end of file (only if not already processed)
    // Literals are processed at LTORG, so we don't need to process them again here
    
    if (inFile.is_open()) {
        inFile.close();
    }
    
    // Close listing file safely
    try {
        listing.close();
    } catch (...) {
        // Ignore errors during close
    }
    
    return true;
}

bool Pass2::parseLine(const std::string& line, std::string& label,
                     std::string& opcode, std::string& operand) {
    std::string trimmed = utils::trim(line);
    if (trimmed.empty() || trimmed[0] == '.') {
        return false;
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

std::string Pass2::generateObjectCode(const std::string& opcode,
                                      const std::string& operand, int locctr) {
    // Check for extended format
    bool extendedFormat = (opcode[0] == '+');
    std::string baseOpcode = extendedFormat ? opcode.substr(1) : opcode;
    
    if (baseOpcode == "WORD") {
        int value = utils::stringToInt(operand);
        return utils::intToHex(value, 6);
    } else if (baseOpcode == "BYTE") {
        if (!operand.empty() && operand[0] == 'C') {
            std::string str = operand.substr(2, operand.length() - 3);
            std::string hex;
            for (char c : str) {
                hex += utils::intToHex((int)c, 2);
            }
            return hex;
        } else if (!operand.empty() && operand[0] == 'X') {
            return operand.substr(2, operand.length() - 3);
        }
    } else if (optab.contains(baseOpcode)) {
        int opcodeValue;
        if (optab.lookup(baseOpcode, opcodeValue)) {
            int format = optab.getFormat(baseOpcode);
            if (extendedFormat) {
                format = 4;
            }
            
            // Parse addressing modes
            bool immediate = false;
            bool indirect = false;
            bool indexed = false;
            std::string symbol = operand;
            
            // Check for immediate addressing (#)
            if (!symbol.empty() && symbol[0] == '#') {
                immediate = true;
                symbol = symbol.substr(1);
            }
            // Check for indirect addressing (@)
            else if (!symbol.empty() && symbol[0] == '@') {
                indirect = true;
                symbol = symbol.substr(1);
            }
            
            // Check for indexed addressing (,X)
            size_t commaPos = symbol.find(',');
            if (commaPos != std::string::npos) {
                indexed = true;
                symbol = symbol.substr(0, commaPos);
            }
            
            // Handle literals
            if (!symbol.empty() && symbol[0] == '=') {
                // Literal - add to literal table if not already present
                // Estimate literal pool address (after current code, will be refined at LTORG)
                int estimatedLiteralAddr = locctr + 100; // Rough estimate
                if (literalTable.find(symbol) == literalTable.end()) {
                    literalTable[symbol] = estimatedLiteralAddr;
                } else {
                    estimatedLiteralAddr = literalTable[symbol];
                }
                
                // Generate object code referencing literal address
                if (extendedFormat) {
                    // Format 4: absolute address
                    int firstByte = opcodeValue | 0x03; // n=1, i=1
                    int secondByte = (indexed ? 0x80 : 0) | 0x10; // e=1 for format 4
                    secondByte |= ((estimatedLiteralAddr >> 16) & 0x0F);
                    int lowBytes = estimatedLiteralAddr & 0xFFFF;
                    int objCode = (firstByte << 24) | (secondByte << 16) | lowBytes;
                    return utils::intToHex(objCode, 8);
                } else {
                    // Format 3: PC-relative to literal pool
                    int firstByte = opcodeValue | 0x03; // n=1, i=1
                    int displacement = estimatedLiteralAddr - (locctr + 3);
                    // Ensure displacement fits in 12 bits
                    if (displacement < -2048) displacement = -2048;
                    if (displacement > 2047) displacement = 2047;
                    displacement &= 0xFFF;
                    int secondByte = (indexed ? 0x80 : 0) | 0x20; // p=1, e=0
                    secondByte |= ((displacement >> 8) & 0x0F);
                    int thirdByte = displacement & 0xFF;
                    int objCode = (firstByte << 16) | (secondByte << 8) | thirdByte;
                    return utils::intToHex(objCode, 6);
                }
            }
            
            if (format == 2) {
                // Format 2: 2 bytes = 4 hex digits
                // First byte: opcode (8 bits)
                // Second byte: register fields
                int reg1 = 0, reg2 = 0;
                
                if (baseOpcode == "CLEAR") {
                    // CLEAR r1: opcode + r1 in high 4 bits, 0 in low 4 bits
                    if (symbol == "X" || symbol == "1") reg1 = 1;
                    else if (symbol == "A" || symbol == "0") reg1 = 0;
                    else if (symbol == "S" || symbol == "4") reg1 = 4;
                    else if (symbol == "T" || symbol == "5") reg1 = 5;
                    int objCode = (opcodeValue << 8) | (reg1 << 4);
                    return utils::intToHex(objCode, 4);
                } else if (baseOpcode == "COMPR") {
                    // COMPR r1,r2: opcode + r1 in high 4 bits, r2 in low 4 bits
                    if (symbol == "A" || symbol == "0") reg1 = 0;
                    else if (symbol == "X" || symbol == "1") reg1 = 1;
                    else if (symbol == "S" || symbol == "4") reg1 = 4;
                    else if (symbol == "T" || symbol == "5") reg1 = 5;
                    // Get second register from operand
                    std::string reg2Str = operand;
                    if (commaPos != std::string::npos) {
                        reg2Str = operand.substr(commaPos + 1);
                    }
                    if (reg2Str == "A" || reg2Str == "0") reg2 = 0;
                    else if (reg2Str == "X" || reg2Str == "1") reg2 = 1;
                    else if (reg2Str == "S" || reg2Str == "4") reg2 = 4;
                    else if (reg2Str == "T" || reg2Str == "5") reg2 = 5;
                    int objCode = (opcodeValue << 8) | (reg1 << 4) | reg2;
                    return utils::intToHex(objCode, 4);
                } else if (baseOpcode == "TIXR") {
                    // TIXR r1: opcode + r1 in high 4 bits, 0 in low 4 bits
                    if (symbol == "X" || symbol == "1") reg1 = 1;
                    else if (symbol == "A" || symbol == "0") reg1 = 0;
                    else if (symbol == "S" || symbol == "4") reg1 = 4;
                    else if (symbol == "T" || symbol == "5") reg1 = 5;
                    int objCode = (opcodeValue << 8) | (reg1 << 4);
                    return utils::intToHex(objCode, 4);
                }
                return utils::intToHex(opcodeValue, 4);
            } else if (format == 3 || format == 4) {
                // Format 3/4: Calculate address/displacement
                int targetAddress = 0;
                bool usePC = false;
                bool useBase = false;
                
                // Handle immediate values
                if (immediate && utils::isNumeric(symbol)) {
                    targetAddress = utils::stringToInt(symbol);
                    usePC = false;
                    useBase = false;
                } else if (immediate && symbol[0] == 'X' && symbol[1] == '\'') {
                    // Immediate hex value like #X'05'
                    std::string hexVal = symbol.substr(2, symbol.length() - 3);
                    targetAddress = utils::hexToInt(hexVal);
                    usePC = false;
                    useBase = false;
                } else if (immediate && !symbol.empty()) {
                    // Immediate addressing with symbol: use symbol's address as immediate value
                    if (!symtab.lookup(symbol, targetAddress)) {
                        return ""; // Symbol not found
                    }
                    usePC = false;
                    useBase = false;
                } else {
                    // Lookup symbol address
                    if (symbol.empty()) {
                        // No operand (e.g., RSUB)
                        targetAddress = 0;
                        usePC = false;
                        useBase = false;
                    } else if (!symtab.lookup(symbol, targetAddress)) {
                        // Symbol not found - could be external reference
                        // Use 0x0000 as placeholder (would be resolved by linker)
                        targetAddress = 0x0000;
                    } else {
                        if (extendedFormat) {
                            // Format 4: use absolute address
                            usePC = false;
                            useBase = false;
                        } else {
                            // Format 3: try PC-relative first, then base-relative
                            int displacement = targetAddress - (locctr + 3);
                            if (displacement >= -2048 && displacement <= 2047) {
                                usePC = true;
                                useBase = false;
                                targetAddress = displacement & 0xFFF; // 12-bit displacement
                            } else {
                                // Try base-relative (assuming base register is set)
                                // For simplicity, use PC-relative with larger range
                                usePC = true;
                                useBase = false;
                                targetAddress = displacement & 0xFFF;
                            }
                        }
                    }
                }
                
                // Build object code
                // First byte: opcode (6 bits) + n (1 bit) + i (1 bit)
                int firstByte = opcodeValue;
                if (!immediate && !indirect) {
                    // Simple addressing: n=1, i=1
                    firstByte |= 0x03; // Set both n and i
                } else if (immediate) {
                    // Immediate: n=0, i=1
                    firstByte |= 0x01; // Set i only
                } else if (indirect) {
                    // Indirect: n=1, i=0
                    firstByte |= 0x02; // Set n only
                }
                
                if (format == 3) {
                    // Format 3: 3 bytes
                    // Second byte: x (bit 0) + b (bit 1) + p (bit 2) + e (bit 3, 0) + high 4 bits of disp
                    // Third byte: low 8 bits of disp
                    int secondByte = 0;
                    if (indexed) secondByte |= 0x80; // x bit
                    if (useBase) secondByte |= 0x40; // b bit
                    if (usePC) secondByte |= 0x20;   // p bit
                    // e bit is 0 for format 3
                    secondByte |= ((targetAddress >> 8) & 0x0F); // High 4 bits of displacement
                    
                    int thirdByte = targetAddress & 0xFF; // Low 8 bits
                    
                    int objCode = (firstByte << 16) | (secondByte << 8) | thirdByte;
                    return utils::intToHex(objCode, 6);
                } else {
                    // Format 4: 4 bytes
                    // Second byte: x (bit 0) + b (bit 1) + p (bit 2) + e (bit 3, 1) + high 4 bits of address
                    // Third and fourth bytes: 20-bit address
                    int secondByte = 0;
                    if (indexed) secondByte |= 0x80; // x bit
                    // b and p are 0 for format 4 (absolute addressing)
                    secondByte |= 0x10; // e bit is 1 for format 4
                    secondByte |= ((targetAddress >> 16) & 0x0F); // High 4 bits of 20-bit address
                    
                    int lowBytes = targetAddress & 0xFFFF; // Low 16 bits
                    
                    int objCode = (firstByte << 24) | (secondByte << 16) | lowBytes;
                    return utils::intToHex(objCode, 8);
                }
            }
        }
    }
    
    return "";
}

void Pass2::processLiteralPool(int& locctr) {
    // Process each literal in the table
    for (auto& lit : literalTable) {
        if (lit.first.length() < 2) continue; // Skip invalid literals
        
        // Update literal address to current location
        lit.second = locctr;
        
        // Generate literal value
        std::string literalValue = generateLiteralValue(lit.first);
        
        // Write literal entry with * label
        std::string literalLine = "* " + lit.first;
        listing.writeLine(locctr, literalValue, literalLine);
        
        // Update location counter based on literal type
        if (lit.first.length() > 1 && lit.first[1] == 'C') {
            // Character literal: length of string
            if (lit.first.length() > 4) {
                std::string str = lit.first.substr(3, lit.first.length() - 4);
                locctr += str.length();
            }
        } else if (lit.first.length() > 1 && lit.first[1] == 'X') {
            // Hex literal: half the hex string length
            if (lit.first.length() > 4) {
                std::string hex = lit.first.substr(3, lit.first.length() - 4);
                locctr += (hex.length() + 1) / 2; // Round up for odd lengths
            }
        }
    }
}

std::string Pass2::generateLiteralValue(const std::string& literal) const {
    if (literal.length() < 2) return "";
    
    if (literal[1] == 'C') {
        // Character literal: =C'...'
        if (literal.length() > 4) {
            std::string str = literal.substr(3, literal.length() - 4);
            std::string hex;
            for (char c : str) {
                hex += utils::intToHex((int)c, 2);
            }
            return hex;
        }
    } else if (literal[1] == 'X') {
        // Hex literal: =X'...'
        if (literal.length() > 4) {
            return literal.substr(3, literal.length() - 4);
        }
    }
    return "";
}

std::vector<Literal> Pass2::getLiterals() const {
    std::vector<Literal> result;
    result.reserve(literalTable.size()); // Pre-allocate to avoid reallocations
    
    for (const auto& lit : literalTable) {
        // Skip invalid literals - must start with = and have at least 2 chars
        if (lit.first.empty() || lit.first.length() < 2 || lit.first[0] != '=') {
            continue;
        }
        
        // Additional safety check
        if (lit.first.length() > 1000) {
            continue; // Skip suspiciously long literals
        }
        
        Literal l;
        // Extract name from literal (e.g., =C'EOF' -> EOF)
        if (lit.first.length() > 4) {
            if (lit.first[1] == 'C' || lit.first[1] == 'c') {
                // Character literal: =C'...'
                size_t startPos = 3;
                size_t len = lit.first.length() - 4;
                if (startPos < lit.first.length() && len > 0 && startPos + len <= lit.first.length()) {
                    l.name = lit.first.substr(startPos, len);
                } else {
                    l.name = "";
                }
            } else if (lit.first[1] == 'X' || lit.first[1] == 'x') {
                // Hex literal: =X'...'
                size_t startPos = 3;
                size_t len = lit.first.length() - 4;
                if (startPos < lit.first.length() && len > 0 && startPos + len <= lit.first.length()) {
                    l.name = "X" + lit.first.substr(startPos, len);
                } else {
                    l.name = "";
                }
            } else {
                // Other literal type
                if (lit.first.length() > 1) {
                    l.name = lit.first.substr(1);
                } else {
                    l.name = lit.first;
                }
            }
        } else if (lit.first.length() > 1) {
            l.name = lit.first.substr(1); // Fallback for short literals
        } else {
            l.name = lit.first;
        }
        
        // Generate operand value safely
        l.operand = generateLiteralValue(lit.first);
        l.address = lit.second;
        
        // Calculate length safely
        if (lit.first.length() > 4 && (lit.first[1] == 'C' || lit.first[1] == 'c')) {
            l.length = lit.first.length() - 4;
        } else if (lit.first.length() > 4 && (lit.first[1] == 'X' || lit.first[1] == 'x')) {
            size_t startPos = 3;
            size_t len = lit.first.length() - 4;
            if (startPos < lit.first.length() && len > 0 && startPos + len <= lit.first.length()) {
                std::string hex = lit.first.substr(startPos, len);
                l.length = (hex.length() + 1) / 2;
            } else {
                l.length = 0;
            }
        } else {
            l.length = 0;
        }
        
        // Only add if we have valid data
        if (!l.name.empty() || !l.operand.empty()) {
            result.push_back(l);
        }
    }
    return result;
}


