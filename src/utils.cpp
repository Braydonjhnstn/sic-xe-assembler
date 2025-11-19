//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
//

#include "utils.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace utils {
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return "";
        }
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
    
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    bool isNumeric(const std::string& str) {
        if (str.empty()) return false;
        for (char c : str) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }
    
    int stringToInt(const std::string& str) {
        try {
            return std::stoi(str);
        } catch (...) {
            return 0;
        }
    }
    
    std::string intToHex(int value, int width) {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(width) << value;
        return ss.str();
    }
    
    int hexToInt(const std::string& hex) {
        try {
            return std::stoi(hex, nullptr, 16);
        } catch (...) {
            return 0;
        }
    }
    
    std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    bool isValidSymbol(const std::string& symbol) {
        if (symbol.empty() || symbol.length() > 6) {
            return false;
        }
        if (!std::isalpha(symbol[0])) {
            return false;
        }
        for (char c : symbol) {
            if (!std::isalnum(c)) {
                return false;
            }
        }
        return true;
    }
}

