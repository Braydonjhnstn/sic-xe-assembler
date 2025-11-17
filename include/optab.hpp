#ifndef OPTAB_HPP
#define OPTAB_HPP

#include <string>
#include <unordered_map>

class OpTable {
public:
    OpTable();
    ~OpTable();
    
    bool lookup(const std::string& opcode, int& opcodeValue) const;
    bool contains(const std::string& opcode) const;
    int getFormat(const std::string& opcode) const;
    
private:
    std::unordered_map<std::string, int> opcodes;
    std::unordered_map<std::string, int> formats;
    void initializeOpcodes();
};

#endif // OPTAB_HPP

