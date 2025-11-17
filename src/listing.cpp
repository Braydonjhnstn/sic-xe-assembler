#include "listing.hpp"
#include <iomanip>
#include <iostream>

ListingGenerator::ListingGenerator() : isOpen(false) {
}

ListingGenerator::~ListingGenerator() {
    if (isOpen) {
        close();
    }
}

bool ListingGenerator::open(const std::string& filename) {
    listingFile.open(filename);
    isOpen = listingFile.is_open();
    return isOpen;
}

void ListingGenerator::close() {
    if (isOpen) {
        listingFile.close();
        isOpen = false;
    }
}

void ListingGenerator::writeLine(int address, const std::string& objectCode,
                                 const std::string& sourceLine) {
    if (!isOpen) return;
    
    listingFile << std::hex << std::uppercase << std::setfill('0');
    if (address > 0) {
        listingFile << std::setw(4) << address << "  ";
    } else {
        listingFile << "      ";
    }
    
    if (!objectCode.empty()) {
        // Determine width based on object code length
        // Format 2: 4 hex digits, Format 3: 6 hex digits, Format 4: 8 hex digits
        // BYTE: variable (2, 4, or 6 hex digits)
        int width = objectCode.length();
        // Don't pad - use natural width, but ensure minimum spacing
        listingFile << objectCode;
        // Pad to ensure consistent column alignment (8 spaces for object code column)
        int padding = 8 - width;
        if (padding > 0) {
            listingFile << std::string(padding, ' ');
        }
        listingFile << "  ";
    } else {
        listingFile << "        ";
    }
    
    listingFile << sourceLine << std::endl;
}

void ListingGenerator::writeHeader(const std::string& programName, int startAddress,
                                  int programLength) {
    if (!isOpen) return;
    
    listingFile << "Program: " << programName << std::endl;
    listingFile << "Start Address: " << std::hex << std::uppercase 
                << std::setfill('0') << std::setw(4) << startAddress << std::endl;
    listingFile << "Program Length: " << std::setw(4) << programLength << std::endl;
    listingFile << std::dec << std::endl;
}

void ListingGenerator::writeError(const std::string& error) {
    if (!isOpen) return;
    listingFile << "*** ERROR: " << error << std::endl;
}

