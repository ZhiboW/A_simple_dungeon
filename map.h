#ifndef MAP
#define MAP

#include "utils.h"
#include <string>
#include <stack>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <sstream>

using namespace std;
//Map definition
class dungeon;
class tile;

//tile connects to other tiles in dungeon
class tile{
	public:
		int val; //tile type: -1 void, 0 normal, 1 start, 2 end
		unsigned char doors; // nesw 0000
		unsigned short danger;//spawn chance
		unsigned short loot;//loot spawn chance
		tile(int val = 0, int danger = 0, int loot = 0, unsigned char doors = 0);
		bool visited;
};

//dungeon map made up of n*m tiles
class dungeon{
	private:
		unsigned int width, height;
		vector<vector<int>> gen_path(int x, int y);
		bool is_valid(int x, int y) const;
	public:
		unsigned int level;//floor level, adds to mob base-level, multiples of 10 creates bossmob
		vector<vector<tile>> map;
		dungeon(unsigned int level = 0, unsigned int w = 25, unsigned int h = 25, float vc = 0.1f);
		
		string print(int detail = 0, unsigned int px = 0, unsigned int py = 0);
		vector<uint8_t> save() const;
		void unpack(vector<uint8_t> block);
		
		vector<uint32_t> get_start();
};

#endif
