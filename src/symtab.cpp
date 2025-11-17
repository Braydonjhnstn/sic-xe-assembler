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

bool SymbolTable::writeToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        return false;
    }
    
    // Sort symbols for consistent output
    std::vector<std::pair<std::string, int>> sortedSymbols;
    for (const auto& pair : table) {
        sortedSymbols.push_back(pair);
    }
    std::sort(sortedSymbols.begin(), sortedSymbols.end());
    
    for (const auto& pair : sortedSymbols) {
        outFile << std::left << std::setw(10) << pair.first 
                << std::right << std::hex << std::uppercase 
                << std::setfill('0') << std::setw(4) << pair.second 
                << std::dec << std::endl;
    }
    
    outFile.close();
    return true;
}

