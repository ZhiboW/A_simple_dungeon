#ifndef INVENTORY
#define INVENTORY

#include <vector>
#include <unordered_map>
#inclue <string>
#include "utils.h"
using namespace std;

class item{
	public:
		string name; // item name
		uint32_t item_id, type; // item identifier, item type, search by type, number from item list
		int cost, resell, uses;
		unordered_map statblock<uint32_t, double>; //variable sized stat block, flexible stat allocation
		void read_item(uint32_t type, uint32_t id);
};

class inv{
	public:
		string bagsize;
		unsigned long item_slots;
		unsigned long money;
		vector<int> item_count;
		vector<item> item_list;
		vector<int> equipped;
		int size_allocator();
		void equipper(item, item &);
		void unpack(vector<uint8_t> block);
};

#endif
