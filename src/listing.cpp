//
// Name: Braydon Johnston REDid: 131049942
// Name: Reinaldo Roldan  REDid: 825760540
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
    
    // Check if line is a comment (starts with .)
    std::string trimmed = sourceLine;
    while (!trimmed.empty() && (trimmed[0] == ' ' || trimmed[0] == '\t')) {
        trimmed = trimmed.substr(1);
    }
    
    if (!trimmed.empty() && trimmed[0] == '.') {
        // Comment line - write as-is without address
        listingFile << sourceLine << std::endl;
        return;
    }
    
    // Write address if provided
    if (address >= 0) {
        char addrStr[10];
        snprintf(addrStr, sizeof(addrStr), "%04X", address);
        listingFile << addrStr << "    ";
    } else {
        listingFile << "                 "; // 17 spaces for no address
    }
    
    // Preserve original source line formatting, just add object code at the end
    // Remove trailing whitespace from source line
    std::string formattedLine = sourceLine;
    while (!formattedLine.empty() && (formattedLine.back() == ' ' || formattedLine.back() == '\t' || formattedLine.back() == '\n' || formattedLine.back() == '\r')) {
        formattedLine.pop_back();
    }
    
    // Calculate padding for object code (right-aligned around column 40)
    // Expected format shows object codes starting around column 40-45
    int sourceWidth = formattedLine.length();
    int targetColumn = 40;
    int padding = (targetColumn > sourceWidth) ? (targetColumn - sourceWidth) : 1;
    
    // Write source line with padding, then object code
    listingFile << formattedLine;
    if (!objectCode.empty()) {
        listingFile << std::string(padding, ' ') << objectCode;
    }
    
    listingFile << std::endl;
}

void ListingGenerator::writeHeader(const std::string& programName, int startAddress,
                                  int programLength) {
    if (!isOpen) return;
    // Don't write header - expected format doesn't have it
    // Header will be written as part of the START line
}

void ListingGenerator::writeError(const std::string& error) {
    if (!isOpen) return;
    listingFile << "*** ERROR: " << error << std::endl;
}


