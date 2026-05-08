#ifndef UTILS
#define UTILS

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

//save utils
template<typename T>
void push_to_block(std::vector<uint8_t>& v, const T& value){
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&value);
    v.insert(v.end(), p, p + sizeof(T));
}

template<typename T>
void read_from_block(const std::vector<uint8_t>& block, size_t& cursor, T& dest){
    // Basic safety check to prevent reading past the end
    if(cursor + sizeof(T) <= block.size()){
        std::memcpy(&dest, &block[cursor], sizeof(T));
        cursor += sizeof(T);
    }
}

//keyboard
enum raw_keys{
    RAW_UP = 1000, RAW_DOWN, RAW_LEFT, RAW_RIGHT, 
    RAW_ESC = 27, RAW_ENTER = 13, RAW_TAB = 9
};
enum class command{
    north, east, south, west,
    map, save, quit, unknown, 
	look, mapall, help, menu
};

class keyreader{
private:
    std::map<int, command> keybinds;
public:
    void load_config(std::string filename);
    command handle_input();
    int get_raw_key();
};
inline void get_line_safe(std::string& out) {
#ifndef _WIN32
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO); // Turn echo and line-buffering back on
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
#endif
    std::cin >> out;
#ifndef _WIN32
    t.c_lflag &= ~(ICANON | ECHO); // Turn it off again for navigation
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
#endif
}

void generate_config();

#endif
