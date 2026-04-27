#include "utils.h"
#include <iostream>

int keyreader::get_raw_key(){
	#ifdef _WIN32
		int ch = _getch();
	    if (ch == 0 || ch == 224) {
	        ch = _getch();
	        switch(ch) {
	            case 72: return RAW_UP;
	            case 80: return RAW_DOWN;
	            case 75: return RAW_LEFT;
	            case 77: return RAW_RIGHT;
	        }
	    }
	    return ch;
	#else
	    struct termios oldt, newt;
	    tcgetattr(STDIN_FILENO, &oldt);
	    newt = oldt;
	    newt.c_lflag &= ~(ICANON | ECHO);
	    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	    int ch = getchar();
	    if (ch == 27) { // Escape sequence
	        struct termios tmp = newt;
	        tmp.c_cc[VMIN] = 0; tmp.c_cc[VTIME] = 0;
	        tcsetattr(STDIN_FILENO, TCSANOW, &tmp);
	        int next = getchar();
	        if (next == '[') {
	            switch(getchar()) {
	                case 'A': ch = RAW_UP; break;
	                case 'B': ch = RAW_DOWN; break;
	                case 'C': ch = RAW_RIGHT; break;
	                case 'D': ch = RAW_LEFT; break;
	            }
	        } else if (next == -1) { ch = RAW_ESC; }
	        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	    }
	    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	    return ch;
	#endif
}

void keyreader::load_config(std::string filename){
	std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t sep = line.find('=');
        if (sep == std::string::npos) continue;

        std::string key = line.substr(0, sep);
        std::string val = line.substr(sep + 1);

        int code = 0;
        if (val == "ARROW_UP") code = RAW_UP;
        else if (val == "ARROW_DOWN") code = RAW_DOWN;
        else if (val == "ARROW_LEFT") code = RAW_LEFT;
        else if (val == "ARROW_RIGHT") code = RAW_RIGHT;
        else if (val == "ESC") code = RAW_ESC;
        else code = val[0]; // Standard char

        if (key == "UP") keybinds[code] = command::north;
        else if (key == "DOWN") keybinds[code] = command::south;
        else if (key == "LEFT") keybinds[code] = command::west;
        else if (key == "RIGHT") keybinds[code] = command::east;
        else if (key == "MENU") keybinds[code] = command::menu;
        else if (key == "MAP") keybinds[code] = command::map;
        else if (key == "LOOK") keybinds[code] = command::look;
        else if (key == "QUIT") keybinds[code] = command::quit;
    }
}

command keyreader::handle_input(){
	int raw = get_raw_key();
    if (keybinds.count(raw)) return keybinds[raw];
    return command::unknown;
}
