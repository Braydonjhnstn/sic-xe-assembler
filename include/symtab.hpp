#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <string>
#include <unordered_map>

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    
    void insert(const std::string& symbol, int address);
    bool lookup(const std::string& symbol, int& address) const;
    bool contains(const std::string& symbol) const;
    void clear();
    bool writeToFile(const std::string& filename) const;
    
private:
    std::unordered_map<std::string, int> table;
};

#endif // SYMTAB_HPP

