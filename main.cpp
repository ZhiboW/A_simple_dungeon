#include "map.h"
#include "entity.h"
#include "saves.h"
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
    //load keybinds
    keyreader listener;
    generate_config();
    listener.load_config("config.ini");
    
    entity player("Player");
    dungeon currentmap(1, 25, 25, 0.15f);
    string savename = "";
    //initialize player
    vector<uint32_t> buffer = currentmap.get_start();
    player.x = buffer[0];
    player.y = buffer[1];
    player.dlvl = currentmap.level;
    //gameplay main loop
    cout << "You arrive";
    bool showmap = true;
    int floornum = 1;
    player.vision = 1;
    while(true){
    	//hud
    	currentmap.map[player.x][player.y].visited = true;
    	currentmap.visibility(player.x, player.y, player.vision);
		unsigned char doors = currentmap.map[player.x][player.y].doors;
		if(showmap){
			cout << "\n" << currentmap.print(player.vision, player.x, player.y);
		} else {
			cout << "\n" << currentmap.print(-1, player.x, player.y);
		}
    	if(currentmap.map[player.x][player.y].val == 2){
    		cout << "\n" << currentmap.print(1, player.x, player.y);
    		cout << "\nYou find the next floor, proceed? (Y/N)" << endl;
    		int menuset = listener.get_raw_key();
				switch(menuset){
					case 'y':
						if(savename.empty()){
								savename = "autosave";
							}
							try{
								saves::save_player(savename + "p.sav", player);
			            		saves::save_floor(savename + "d.sav", currentmap);
							} catch (const std::exception& e) {
								cout << "Save failed: " << e.what();
							}
						currentmap = dungeon(floornum, 25, 25, 0.15f);
						buffer = currentmap.get_start();
			    		player.x = buffer[0];
			    		player.y = buffer[1];
						player.dlvl = currentmap.level;
						continue;
				}
    		
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
		cout << "\n[ESC] MENU";
		//input handler
		command cmd = listener.handle_input();
		
		cout << "\033[H\033[J"; //clear screen
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
			case command::menu:
				cout << "1: Save\n2: Load\n";
				{
					int menuset = listener.get_raw_key();
					switch(menuset){
						case 27:
							break;
						case '1':
							if(savename.empty()){
								cout << "Save as: ";
								get_line_safe(savename);
							}
							try{
								saves::save_player(savename + "p.sav", player);
	                    		saves::save_floor(savename + "d.sav", currentmap);
	                    		cout << "Saved as " << savename;
							} catch (const std::exception& e) {
								cout << "Save failed: " << e.what();
							}
	                    	break;
	                    case '2':
	                    	{
		                    	cout << "Load file: ";
		                    	get_line_safe(savename);
		                    	entity buffer;
		                    	dungeon buffmap;
		                    	try{
			                    	saves::load_player(savename + "p.sav", buffer);
			                    	saves::load_floor(savename + "d.sav", player.dlvl, buffmap);
			                    	player = buffer;
			                    	currentmap = buffmap;
			                    	cout << savename << " loaded";
		                    	} catch (const std::exception& e){
		                    		cout << "Load failed: " << e.what();
								}
							}
	                    	break;
	                    case '0':
	                    	cout << currentmap.print(-2, player.x, player.y, true);
							showmap = false;
							break;
						default:
							cout << "Invalid option";
							break;
					}
					break;
				}
			case command::look:
				cout << "You look around";
				showmap = true;
				break;
			case command::map:
				cout << "You look at your map";
				showmap = false;
				break;
			case command::quit:
				return 0;
			case command::help:
				cout << "Arrow keys for movement, [ESC] for menu\n"
						"[l] to look around, [m] to check your map\n[q] to exit.";
				break;
			default:
				cout << "Unknown command, [h] for help.";
				break;
		}
	}
}

