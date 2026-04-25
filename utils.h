#ifndef UTILS
#define UTILS

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

template<typename T>
void push_to_block(std::vector<uint8_t>& v, const T& value) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&value);
    v.insert(v.end(), p, p + sizeof(T));
}

template<typename T>
void read_from_block(const std::vector<uint8_t>& block, size_t& cursor, T& dest) {
    // Basic safety check to prevent reading past the end
    if (cursor + sizeof(T) <= block.size()) {
        std::memcpy(&dest, &block[cursor], sizeof(T));
        cursor += sizeof(T);
    }
}


enum class command{
    north, east, south, west,
    map, save, quit, unknown, 
	look, mapall, help
};

inline command get_command(std::string input) {
    if (input.empty()) return command::unknown;
    char c = tolower(input[0]);
    
    switch(c) {
        case 'n': return command::north;
        case 'e': return command::east;
        case 's': return command::south;
        case 'w': return command::west;
        case 'm': return command::map;
        case '0': return command::mapall;
        case 'q': return command::quit;
        case 'v': return command::save;
        case 'l': return command::look;
        case 'h': return command::help;
        default:  return command::unknown;
    }
}
#endif
