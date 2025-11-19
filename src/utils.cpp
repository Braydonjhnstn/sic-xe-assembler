//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
// Cs530-03-Fall2025
// Assignment 2 SIC/XE Assembler
// utils.cpp
//

#include "utils.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace utils {
    // remove leading and trailing whitespace from string
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return "";
        }
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
    
    // split string into tokens using delimiter
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    // check if string contains only numeric digits
    bool isNumeric(const std::string& str) {
        if (str.empty()) return false;
        for (char c : str) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }
    
    // convert string to integer, return 0 on error
    int stringToInt(const std::string& str) {
        try {
            return std::stoi(str);
        } catch (...) {
            return 0; // return 0 if conversion fails
        }
    }
    
    // convert integer to uppercase hexadecimal string with specified width
    std::string intToHex(int value, int width) {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(width) << value;
        return ss.str();
    }
    
    // convert hexadecimal string to integer, return 0 on error
    int hexToInt(const std::string& hex) {
        try {
            return std::stoi(hex, nullptr, 16);
        } catch (...) {
            return 0; // return 0 if conversion fails
        }
    }
    
    // convert string to uppercase
    std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    // validate SIC/XE symbol: 1-6 chars, starts with letter, alphanumeric only
    // this is pretty basic validation but should catch most errors
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

