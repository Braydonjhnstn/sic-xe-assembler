//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// listing.cpp
//

#include "listing.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdio>

ListingGenerator::ListingGenerator() : isOpen(false) {
}

ListingGenerator::~ListingGenerator() {
    if (isOpen && listingFile.is_open()) {
        listingFile.close();
    }
    isOpen = false;
}

bool ListingGenerator::open(const std::string& filename) {
    listingFile.open(filename);
    isOpen = listingFile.is_open();
    return isOpen;
}

void ListingGenerator::close() {
    if (isOpen && listingFile.is_open()) {
        listingFile.close();
    }
    isOpen = false;
}

void ListingGenerator::writeLine(int address, const std::string& objectCode,
                                 const std::string& sourceLine) {
    if (!isOpen || !listingFile.is_open()) return;
    
    // check if line is a comment (starts with .)
    // comments are written as-is without address or object code
    std::string trimmed = sourceLine;
    while (!trimmed.empty() && (trimmed[0] == ' ' || trimmed[0] == '\t')) {
        trimmed = trimmed.substr(1);
    }
    
    if (!trimmed.empty() && trimmed[0] == '.') {
        // comment line - write as-is without address
        listingFile << sourceLine << std::endl;
        return;
    }
    
    // write address in hex format (4 digits) if provided
    // use -1 to indicate no address (e.g., for END directive)
    if (address >= 0) {
        char addrStr[10];
        snprintf(addrStr, sizeof(addrStr), "%04X", address);
        listingFile << addrStr << "    ";
    } else {
        listingFile << "                 "; // 17 spaces for no address
    }
    
    // preserve original source line formatting, remove trailing whitespace
    std::string formattedLine = sourceLine;
    while (!formattedLine.empty() && (formattedLine.back() == ' ' || formattedLine.back() == '\t' || formattedLine.back() == '\n' || formattedLine.back() == '\r')) {
        formattedLine.pop_back();
    }
    
    // calculate padding to align object code around column 40
    // format: ADDR    SOURCE_LINE                    OBJECT_CODE
    // this padding calculation is a bit rough but it works
    int sourceWidth = formattedLine.length();
    int targetColumn = 40;
    int padding = (targetColumn > sourceWidth) ? (targetColumn - sourceWidth) : 1;
    
    // write source line with padding, then object code
    listingFile << formattedLine;
    if (!objectCode.empty()) {
        listingFile << std::string(padding, ' ') << objectCode;
    }
    
    listingFile << std::endl;
}

void ListingGenerator::writeHeader(const std::string& programName, int startAddress,
                                  int programLength) {
    if (!isOpen) return;
    // dont write header - expected format doesnt have it
    // header will be written as part of the START line
}

void ListingGenerator::writeError(const std::string& error) {
    if (!isOpen) return;
    listingFile << "*** ERROR: " << error << std::endl;
}


