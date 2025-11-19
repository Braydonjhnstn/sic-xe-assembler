//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
//

#include "symtab.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>

SymbolTable::SymbolTable() {
}

SymbolTable::~SymbolTable() {
}

void SymbolTable::insert(const std::string& symbol, int address) {
    table[symbol] = address;
}

bool SymbolTable::lookup(const std::string& symbol, int& address) const {
    auto it = table.find(symbol);
    if (it != table.end()) {
        address = it->second;
        return true;
    }
    return false;
}

bool SymbolTable::contains(const std::string& symbol) const {
    return table.find(symbol) != table.end();
}

void SymbolTable::clear() {
    table.clear();
}

bool SymbolTable::writeToFile(const std::string& filename, const std::string& programName,
                              int programLength, const std::vector<Literal>& literals) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        return false;
    }
    
    // Ensure we can write to the file
    if (!outFile.good()) {
        outFile.close();
        return false;
    }
    
    // Write header
    outFile << "CSect   Symbol  Value   LENGTH  Flags:" << std::endl;
    outFile << "--------------------------------------" << std::endl;
    
    // Write control section info
    outFile << std::left << std::setw(8) << programName
            << std::right << std::hex << std::uppercase << std::setfill('0')
            << std::setw(6) << 0  // Value (start address)
            << "  " << std::setw(6) << programLength  // Length
            << std::setfill(' ') << std::endl;
    
    // Sort symbols for consistent output (excluding control section name)
    std::vector<std::pair<std::string, int>> sortedSymbols;
    for (const auto& pair : table) {
        if (pair.first != programName) {
            sortedSymbols.push_back(pair);
        }
    }
    std::sort(sortedSymbols.begin(), sortedSymbols.end());
    
    // Write symbols
    for (const auto& pair : sortedSymbols) {
        outFile << std::setfill(' ') << std::left << std::setw(8) << ""
                << std::setw(8) << pair.first
                << std::right << std::hex << std::uppercase << std::setfill('0')
                << std::setw(6) << pair.second
                << "          R"  // Flags: R for relocatable
                << std::dec << std::endl;
    }
    
    // Write literal table section
    if (!literals.empty()) {
        outFile << std::endl;
        outFile << "Literal Table " << std::endl;
        outFile << "Name  Operand   Address  Length:" << std::endl;
        outFile << "--------------------------------" << std::endl;
        
        for (const auto& lit : literals) {
            // Safety check - ensure we have valid data
            if (lit.name.length() > 100 || lit.operand.length() > 100) {
                continue; // Skip suspiciously large entries
            }
            outFile << std::left << std::setfill(' ') << std::setw(5) << lit.name
                    << std::setw(10) << lit.operand
                    << std::right << std::hex << std::uppercase << std::setfill('0')
                    << std::setw(4) << lit.address
                    << "     " << std::dec << lit.length << std::endl;
            if (!outFile.good()) {
                break; // Stop if file write fails
            }
        }
    }
    
    outFile.flush();
    if (outFile.is_open()) {
        outFile.close();
    }
    return true;
}

