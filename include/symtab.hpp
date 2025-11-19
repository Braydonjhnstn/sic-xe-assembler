//
// Name: Braydon Johnston REDid: 131049942
// Name: Reinaldo Roldan  REDid: 825760540
//

#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct Literal {
    std::string name;
    std::string operand;
    int address;
    int length;
};

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    
    void insert(const std::string& symbol, int address);
    bool lookup(const std::string& symbol, int& address) const;
    bool contains(const std::string& symbol) const;
    void clear();
    bool writeToFile(const std::string& filename, const std::string& programName, 
                     int programLength, const std::vector<Literal>& literals) const;
    
private:
    std::unordered_map<std::string, int> table;
};

#endif // SYMTAB_HPP

