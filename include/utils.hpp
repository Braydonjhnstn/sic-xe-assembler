//
// Name: Braydon Johnston REDid: 131049942 Edoras: cssc2115
// Name: Reinaldo Roldan  REDid: 825760540 Edoras: cssc2131
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

namespace utils {
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    bool isNumeric(const std::string& str);
    int stringToInt(const std::string& str);
    std::string intToHex(int value, int width = 0);
    int hexToInt(const std::string& hex);
    std::string toUpper(const std::string& str);
    bool isValidSymbol(const std::string& symbol);
}

#endif // UTILS_HPP

