#ifndef LISTING_HPP
#define LISTING_HPP

#include <string>
#include <fstream>
#include <vector>

class ListingGenerator {
public:
    ListingGenerator();
    ~ListingGenerator();
    
    bool open(const std::string& filename);
    void close();
    void writeLine(int address, const std::string& objectCode,
                   const std::string& sourceLine);
    void writeHeader(const std::string& programName, int startAddress,
                    int programLength);
    void writeError(const std::string& error);
    
private:
    std::ofstream listingFile;
    bool isOpen;
};

#endif // LISTING_HPP

