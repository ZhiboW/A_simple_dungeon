#include "map.h"
#include "entity.h"
#include "saves.h"
#include <iostream>
#include <cstdlib>
#include <fstream>

#ifdef _WIN32
	#include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <stdio.h>
#endif

int main() {
	#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    entity player("Player");
    dungeon currentmap(1, 10, 10, 0.15f);
    
    vector<uint32_t> buffer = currentmap.get_start();
    player.x = buffer[0];
    player.y = buffer[1];
    player.dlvl = currentmap.level;
    
    cout << "You arrive";
    
    bool showmap = true;
    while(true){
    	if(currentmap.map[player.x][player.y].val == 2){
    		cout << "\n" << currentmap.print(1, player.x, player.y);
    		cout << "\nYou find the exit" << endl;
    		return 0;
		}
		currentmap.map[player.x][player.y].visited = true;
		unsigned char doors = currentmap.map[player.x][player.y].doors;
		if(showmap){
			cout << "\n" << currentmap.print(2, player.x, player.y);
		} else {
			showmap = true;
		}
		cout << "\nYou can move ";
		{
			vector<string> dirs;
			string directions = "";
		    if (doors & 8) dirs.push_back("north");
		    if (doors & 4) dirs.push_back("east");
		    if (doors & 2) dirs.push_back("south");
		    if (doors & 1) dirs.push_back("west");
		
		    if (dirs.empty()){
		    	directions = "nowhere";
		    	break;
			}
		    for (size_t i = 0; i < dirs.size(); i++) {
		        directions += dirs[i];
		        //2 items
		        if (dirs.size() == 2 && i == 0) {
		            directions += " and ";
		        } 
		        //more items
		        else if (dirs.size() > 2 && i < dirs.size() - 1) {
		            if (i == dirs.size() - 2) {
		                directions += ", and ";
		            } else {
		                directions += ", ";
		            }
		        }
		    }
		    cout << directions;
		}
		cout << "\n> ";
		string input = "";
		cin >> input;
		
		command cmd = get_command(input);
		//cout << "\033[H\033[J"; clear screen
		switch(cmd){
			case command::north:
				if(doors & 8){
					cout << "You move north";
					player.y--;
				} else {
					cout << "You push against the wall";
				}
				break;
			case command::east:
				if(doors & 4){
					cout << "You move east";
					player.x++;
				} else {
					cout << "You push against the wall";
				}
				break;
			case command::south:
				if(doors & 2){
					cout << "You move south";
					player.y++;
				} else {
					cout << "You push against the wall";
				}
				break;
			case command::west:
				if(doors & 1){
					cout << "You move west";
					player.x--;
				} else {
					cout << "You push against the wall";
				}
				break;
			case command::look:
				cout << "Nothing special here";
				break;
			case command::map:
				cout << "You look at your map\n" << currentmap.print(1, player.x, player.y);
				showmap = false;
				break;
			case command::mapall:
				cout << currentmap.print(0, player.x, player.y);
				showmap = false;
				break;
			case command::quit:
				return 0;
			case command::help:
				cout << "One word at a time\nMovement: \"north\", \"south\", \"east\", \"west\"\n"
						"\"look\" to look around, \"map\" to check your map\n\"quit\" to exit.";
				break;
			case command::unknown:
				cout << "Unknown command, you can always call for \"help\"";
				break;
		}
	}
}

