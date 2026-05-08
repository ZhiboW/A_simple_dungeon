#include "map.h"

tile::tile(int val, int danger, int loot, unsigned char doors){
	this->val = val;
	this->danger = danger;
	this->loot = loot;
	this->doors = doors;
	visited = false;
	visible = false;
}


dungeon::dungeon(unsigned int lev, unsigned int w, unsigned int h, float vc){
	level = lev;
	width = w;
	height = h;
	srand(time(NULL));
	//mapsize
	map.resize(width, vector<tile>(height, tile(0,0,0,0)));
	//void tiles
	for(unsigned int x = 0; x < width; x++){
		for(unsigned int y = 0; y < height; y++){
			if(((float)rand() / RAND_MAX) < vc){
				map[x][y].val = -1;
				map[x][y].visited = true;
			}
		}
	}
	//start tile
	int sx = rand()%width;
	int sy = rand()%height;
	map[sx][sy].val = 1;
	map[sx][sy].visited = false;
	//mainpath
	vector<vector<int>> main_path = gen_path(sx, sy);
	//end tile
	if(main_path.size() > 1){
		int idx = 1 + rand() % (main_path.size() -1);
		map[main_path[idx][0]][main_path[idx][1]].val = 2;
	}
	//connect the rest
	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			if(!map[x][y].visited){
				gen_path(x, y);
			}
		}
	}
	
	//reset visited flag
	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			map[x][y].visited = false;
		}
	}
}

//path maker
vector<vector<int>> dungeon::gen_path(int sx, int sy){
	vector<vector<int>> pathlist;
	stack<vector<int>> stk;
	
	stk.push({sx, sy});
	map[sx][sy].visited = true;
	pathlist.push_back({sx, sy});
	
	int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };
    unsigned char masks[] = {8,4,2,1};
    unsigned char anti[] = {2,1,8,4};
    
    while(!stk.empty()) {
    	int x = stk.top()[0];
    	int y = stk.top()[1];
    	
    	vector<int> nbrs;
    	for(int i = 0; i < 4; i++){
    		int nx = x + dx[i];
    		int ny = y + dy[i];
    		if(is_valid(nx, ny) && !map[nx][ny].visited){
    			nbrs.push_back(i);
			}
		}
		
		if(!nbrs.empty()){
			int dir = nbrs[rand() % nbrs.size()];
			int nx = x + dx[dir];
    		int ny = y + dy[dir];
    		
    		map[x][y].doors |= masks[dir];
    		map[nx][ny].doors |= anti[dir];
    		map[nx][ny].visited = true;
    		
    		pathlist.push_back({nx, ny});
    		stk.push({nx,ny});
		} else {
			stk.pop();
		}
	}
	
	for(auto& coord: pathlist){
		int x = coord[0];
    	int y = coord[1];
    	
    	for(int i = 0; i < 4; i++){
    		int nx = x + dx[i];
    		int ny = y + dy[i];
    		
    		if (is_valid(nx, ny) && map[nx][ny].val != -1) { 
                if ((rand() % 100) < 5) { 
                    map[x][y].doors |= masks[i];
                    map[nx][ny].doors |= anti[i];
                }
            }
    	}
	}
	return pathlist;
}

bool dungeon::is_valid(int x, int y) const {
    return (x >= 0 && x < (int)width && y >= 0 && y < (int)height);
}

string dungeon::print(int detail, unsigned int px, unsigned int py, bool wallpeek){
	stringstream ss;
	
	const char* box[] = {
        " ",          //0
        "\u2574",     //1
        "\u2577",     //2
        "\u2510",     //3
        "\u2576",     //4
        "\u2500",     //5
        "\u250c",     //6
        "\u252c",     //7
        "\u2575",     //8
        "\u2518",     //9
        "\u2502",     //A
        "\u2524",     //B
        "\u2514",     //C
        "\u2534",     //D
        "\u251c",     //E
        "\u253c"      //F
    };
	
	
	if(detail >= 0) { //vision radius
        for(int yoffset = -(int)detail; yoffset <= (int)detail; yoffset++){
        	for(int xoffset = -(int)detail; xoffset <= (int)detail; xoffset++){
        		int cx = (int)px + xoffset;
                int cy = (int)py + yoffset;
                //outofbounds check
                if(!is_valid(cx, cy)){
                	ss << (wallpeek ? "#" : " ");
                	continue;
				}
				//fog of war
				if (!wallpeek && !map[cx][cy].visited && !map[cx][cy].visible && !(cx == px && cy == py)) {
	                ss << " "; 
	                continue;
	            }
				//tile drawing
				if(map[cx][cy].val == -1) {
	                ss << "#"; // Void tile
	            }else{
	            	if(!wallpeek && !map[cx][cy].visible && map[cx][cy].visited){
	            		ss << "\033[90m";
					}
	            	if(map[cx][cy].val == 1) {
	                    ss << "\033[1;32m";
	                } else if(map[cx][cy].val == 2) {
	                    ss << "\033[1;31m";
	                }
	                //player tile
	                if(cx == px && cy == py){
	                	ss << "\033[7m";
					}
	                
	                ss << box[map[cx][cy].doors & 0xF];
	                ss << "\033[0m";
            	}
			}
			ss << "\n";
		}
    }else{
    	//cropping mask
		vector<uint32_t> lims = {0, width - 1, 0, height -1};
		
		if(detail == -1){ //visited tiles only
			lims = {width, 0, height, 0};
			
			for(unsigned int y = 0; y < height; y++) {
	        	for(unsigned int x = 0; x < width; x++) {
	        		if(map[x][y].visited || (x == px && y == py)) {
	                    if (x < lims[0]) lims[0] = x;
	                    if (x > lims[1]) lims[1] = x;
	                    if (y < lims[2]) lims[2] = y;
	                    if (y > lims[3]) lims[3] = y;
	                }
	        	}
	        }
		}
		
		for(unsigned int y = lims[2]; y <= lims[3]; y++) {
	        for(unsigned int x = lims[0]; x <= lims[1]; x++) {
	        	//fog of war
				if (!wallpeek && !map[x][y].visited && !(x == px && y == py)) {
	                ss << " "; 
	                continue;
	            }
	        	//tile drawing
	            if(map[x][y].val == -1) {
	                ss << "#"; // Void tile
	            }else{
	            	if(detail >= 0 && !wallpeek && !map[x][y].visible && map[x][y].visited){
	            		ss << "\033[90m";
					}
	            	if(map[x][y].val == 1) {
	                    ss << "\033[1;32m";
	                } else if(map[x][y].val == 2) {
	                    ss << "\033[1;31m";
	                }
	                //player tile
	                if(x == px && y == py){
	                	ss << "\033[7m";
					}
	                
	                ss << box[map[x][y].doors & 0xF];
	                ss << "\033[0m";
	            }
	        }
	        ss << "\n";
	    }
	} 
	
    string buffer = ss.str();
    buffer.pop_back();
    return buffer;
}

void dungeon::visibility(unsigned int px, unsigned int py, int radius){
	for(unsigned int y = 0; y < height; y++) {
        for(unsigned int x = 0; x < width; x++) {
            map[x][y].visible = false;
        }
    }
    map[px][py].visible = true;
    int dx[] = { 0, 1, 0, -1 }; //N, E, S, W
    int dy[] = { -1, 0, 1, 0 };
    unsigned char masks[] = { 8, 4, 2, 1 };
    //straight lines until wall
	for(int i = 0; i < 4; i++) {
        unsigned int cx = px;
        unsigned int cy = py;
		
        for(int d = 0; d < radius; d++) {
            if (!(map[cx][cy].doors & masks[i])) break;

            cx += dx[i];
            cy += dy[i];

            if (!is_valid(cx, cy)) break;

            map[cx][cy].visible = true;
        }
    }
}

vector<uint32_t> dungeon::get_start(){
	vector<uint32_t> coords;
	for(unsigned int y = 0; y < height; y++) {
        for(unsigned int x = 0; x < width; x++) {
        	if(map[x][y].val == 1){
        		coords = {x,y};
        		return coords;
			}
		}
	}
}

vector<uint8_t> dungeon::save() const {
	vector<uint8_t> block;
	
	push_to_block(block, (uint32_t) level);
	push_to_block(block, (uint32_t) width);
	push_to_block(block, (uint32_t) height);
	
	for(unsigned int y = 0; y < height; y++) {
        for(unsigned int x = 0; x < width; x++) {
            const tile& t = map[x][y];
            
            push_to_block(block, t.val);
            push_to_block(block, t.danger);
            push_to_block(block, t.loot);
            push_to_block(block, t.doors);
            
            uint8_t vis = t.visited ? 1 : 0;
            push_to_block(block, vis);
        }
    }

    return block;
}

void dungeon::unpack(vector<uint8_t> block){
    if(block.empty()) return;
	
	size_t cursor = 0;
	uint32_t slvl, w, h;
	
	read_from_block(block, cursor, slvl);
	read_from_block(block, cursor, w);
	read_from_block(block, cursor, h);
	
	this->level = slvl;
	this->width = w;
	this->height = h;
	
	map.assign(width, vector<tile>(height, tile(0,0,0,0)));
	
	for(unsigned int y = 0; y < height; y++) {
        for(unsigned int x = 0; x < width; x++) {
            tile& t = map[x][y];
            
            read_from_block(block, cursor, t.val);
            read_from_block(block, cursor, t.danger);
            read_from_block(block, cursor, t.loot);
            read_from_block(block, cursor, t.doors);
            
            uint8_t exp;
            read_from_block(block, cursor, exp);
            t.visited = (exp == 1);
        }
    }
}
